#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>


#include "rtp.h"
#include "video_decoder_factory.h"
#include "rtp2nalu_h264.h"

using namespace std;

int _nalu_offset = 0;
int init_recvsock ( unsigned int port );
bool rtp_2_nalu( unsigned char * nalu_buf, unsigned char *rtp, int rtp_len);

int yuv_handler(uint8_t **data, int w, int h, int *linesize)
{
    static int fd = open("xxx.yuv", O_CREAT | O_TRUNC | O_RDWR, 0666);

    write(fd, data[0], linesize[0]*h);
    write(fd, data[1], linesize[1]*h/2);
    write(fd, data[2], linesize[2]*h/2);

    return 0;
}

int main()
{
#if 0
    int fd = init_recvsock(9998);
    unsigned char recv_buf[2048] = {0};
    unsigned char* nalu_buf = new unsigned char[1920*1080*3];
    struct sockaddr_in  src_addr;
    socklen_t src_addr_len;
#endif

    Video_Decoder::YUV_CB cb = std::bind(yuv_handler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    Video_Decoder* video_decoder = Video_Decoder_Factory::create(Video_Decoder::H264, &cb);

#if 1
    Rtp2Nalu_H264::Handler_Cb rtp2nalu_cb = std::bind(&Video_Decoder::operator () , video_decoder, std::placeholders::_1, std::placeholders::_2);

    Rtp2Nalu_H264 rtp2nalu(&rtp2nalu_cb);
    rtp2nalu.start(6666);

    while(1)
    {
        usleep(1000* 1000);
    }
#endif

#if 0
    while(1)
    {
        int rc = recvfrom(fd, recv_buf, 2048, 0, (struct sockaddr*)&src_addr, &src_addr_len);
        if(rc <= 0)
        {
            close(fd);
            return -1;
        }

        bool ret = rtp_2_nalu(nalu_buf, recv_buf, rc);
        if( ret )
        {
            (*video_decoder)(nalu_buf, _nalu_offset);
        }

    }
#endif
    return 0;
}


int init_recvsock ( unsigned int port )
{
    int nRecvBuf = 1024 * 1024;
    int videorecvfd = -1;
    struct sockaddr_in  servaddr;
    int rtn = 0;

    videorecvfd = socket ( AF_INET, SOCK_DGRAM, 0 );
    rtn = setsockopt ( videorecvfd, SOL_SOCKET, SO_RCVBUF, ( const char* ) & nRecvBuf, sizeof ( int ) );

    bzero ( &servaddr, sizeof ( servaddr ) );
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
    servaddr.sin_port        = htons( port );
    //extern int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
    ::bind ( videorecvfd, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );

    return videorecvfd;
}

bool rtp_2_nalu( unsigned char * nalu_buf, unsigned char *rtp, int rtp_len)
{
    static char delim[4] = {0x00, 0x00, 0x00, 0x01};
    rtp_header_t *rtp_header = (rtp_header_t *)rtp;
    unsigned char *nalu = rtp + RTP_FIXED_HEADER_SIZE;
    int nalu_len = rtp_len - RTP_FIXED_HEADER_SIZE;
    nalu_type_t *nalu_type = (nalu_type_t *)nalu;
    nalu_type_t nt;
    fu_indicator_t *fu_indicator = NULL;
    fu_header_t *fu_header = NULL;

    /* 非法RTP包 */
    if (rtp_len <= RTP_FIXED_HEADER_SIZE)
    {
        printf ( "not rtp pack!\n");
        return false;
    }
    /* RTP版本号不正确 */
    else if (rtp_header->version != 2)
    {
        printf ( "rtp version error!!\n");
        return false;
    }
    /* 载荷不是H264 */
    else if (rtp_header->payload_type != RTP_PAYLOAD_TYPE_H264)
    {
        printf ( "not h264 packet!!\n");
        return false;
    }
    /* 暂不处理序列号、时间戳、SSRC和其他字段 */

    /* 如果是单NALU包 */
    if (nalu_type->type >= 1 && nalu_type->type <= 23)
    {
        /* 添加NALU的分隔符后拷贝 */
        _nalu_offset = 0;
        if(nalu_type->type == 7 || nalu_type->type == 8)
        {
            //            printf("---sps or pps--len %d--\n", rtp_len-RTP_FIXED_HEADER_SIZE);

        }
        memcpy(nalu_buf, delim, sizeof(delim));
        _nalu_offset += sizeof(delim);
        memcpy(nalu_buf + _nalu_offset, nalu, nalu_len);
        _nalu_offset += nalu_len;

        return true;
        //        (*callback)(buf, sizeof(delim) + nalu_len);
    }
    /* 如果是FU-A分片包 */
    else if (nalu_type->type == RTP_H264_FU_A)
    {
        //        printf("FU-A\n");
        fu_indicator = (fu_indicator_t *)nalu;
        fu_header = (fu_header_t *)(nalu + sizeof(fu_indicator_t));

        /* 错误的分片包 */
        if (fu_header->start == 1 && fu_header->end == 1)
        {
            /* 丢弃此RTP包和整个NALU */
            printf("FU-A==start = 1, end = 1\n");
            _nalu_offset = 0;
            return false;
        }
        /* 第一个分片包 */
        else if (fu_header->start == 1)
        {

            //            printf("FU-A11111111111111111\n");
            /* 丢弃上个NALU */
            _nalu_offset = 0;

            nt.fbit = fu_indicator->fbit;
            nt.nri  = fu_indicator->nri;
            nt.type = fu_header->type;

            memcpy(nalu_buf, delim, sizeof(delim));
            _nalu_offset += sizeof(delim);

            memcpy(nalu_buf + _nalu_offset, &nt, sizeof(nt));
            _nalu_offset += sizeof(nt);

            memcpy(nalu_buf + _nalu_offset, nalu + sizeof(fu_indicator_t) + sizeof(fu_header_t), nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            _nalu_offset += (nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            return false;
        }
        /* 最后一个分片包 */
        else if (fu_header->end == 1)
        {
            //            printf("FU-A333333333333333\n");
            /* 如果前面没收到分片包 */
            if (_nalu_offset == 0)
            {
                return false;
            }

            /* 丢失的中间的分片包暂不考虑 */

            memcpy(nalu_buf + _nalu_offset, nalu + sizeof(fu_indicator_t) + sizeof(fu_header_t), nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            _nalu_offset += (nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            //            printf("nalu len = %d\n", _nalu_offset);
            return true;
        }
        /* 中间的分片包 */
        else
        {
            //            printf("FU-A222222222222222\n");
            /* 如果前面没收到分片包 */
            if (_nalu_offset == 0)
            {
                printf ( "FU-A2222---LOST\n");
                return false;
            }
            /* 丢失的中间的分片包暂不考虑 */

            memcpy(nalu_buf + _nalu_offset, nalu + sizeof(fu_indicator_t) + sizeof(fu_header_t), nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            _nalu_offset += (nalu_len - sizeof(fu_indicator_t) - sizeof(fu_header_t));
            return false;
        }
    }
    /* 其他类型暂不处理 */
    else
    {
        printf("rtp type %c error!\n", nalu_type->type);
        return false;
    }
    return false;
}
