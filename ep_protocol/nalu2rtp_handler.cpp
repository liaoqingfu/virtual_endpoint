#include <cstring>
#include <ctime>

#include "nalu2rtp_handler.h"

Nalu2Rtp_Handler::Nalu2Rtp_Handler()
    :_allocator(8)
{
    srand( time(NULL) );
    _ssrc =  rand();
}

Nalu2Rtp_Handler::~Nalu2Rtp_Handler()
{

}

int Nalu2Rtp_Handler::start(int ip, int port)
{
    _send_fd = _udp_socket.init_sendsock();
    _dst_addr.sin_family = AF_INET;
    _dst_addr.sin_addr.s_addr = ip;
    _dst_addr.sin_port = htons(port);

    msg_queue()->activate();
    activate();

    return 0;
}

void Nalu2Rtp_Handler::stop()
{
    msg_queue()->deactivate();
    wait();
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int Nalu2Rtp_Handler::svc()
{
    Nalu_Buf* nalu_buf = NULL;
    int rc = -1;
    while(true)
    {
        rc = getq(nalu_buf);
        if( -1 == rc )
        {
            ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: getq(deactived) error!\n"), __PRETTY_FUNCTION__) );
            break;
        }
        if( !nalu_buf )
        {
            ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: getq(NULL) error!\n"), __PRETTY_FUNCTION__) );
            break;
        }

        static int fd = ::open("xxx.nalu", O_CREAT | O_TRUNC | O_RDWR, 0666);
        write(fd, nalu_buf->buf, nalu_buf->len);

        pack_and_send(nalu_buf->buf, nalu_buf->len, nalu_buf->timestamp);
        _allocator.free(nalu_buf);
    }

    ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: thread exit!!!!\n"), __PRETTY_FUNCTION__) );
    return 0;
}

int Nalu2Rtp_Handler::operator ()(unsigned char *raw_data, int len, int, unsigned int timestamp)
{
    if( !raw_data )  return -1;

    Nalu_Buf* nalu_buf = (Nalu_Buf*)_allocator.malloc();
    nalu_buf->len = len;
    nalu_buf->timestamp = timestamp;
    memcpy(nalu_buf->buf, raw_data, len);
    putq(nalu_buf);

    return 0;
}

int Nalu2Rtp_Handler::rtp_sendto(unsigned char *nalu_buf, int nalu_len)
{
    return sendto(_send_fd, nalu_buf, nalu_len, 0, (const sockaddr*)&_dst_addr, sizeof(_dst_addr) );
}

