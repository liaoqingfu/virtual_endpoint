#include <cstdio>
#include <cstring>

#include "rtp2nalu_h264.h"
#include "protocol_types.h"
#include "protocol_defines.h"

Rtp2Nalu_H264::Rtp2Nalu_H264(Handler_Cb *handler_cb)
    :Rtp_Recv_Handler(handler_cb)
{
}

Rtp2Nalu_H264::~Rtp2Nalu_H264()
{
    if(_nalu_buf)
    {
        delete _nalu_buf;
        _nalu_buf = NULL;
    }
}

bool Rtp2Nalu_H264::rtp_parse(unsigned char *rtp_buf, int rtp_buf_len)
{
    static char nalu_start_code[] = {0x00, 0x00, 0x00, 0x01};
    Rtp_Header* rtp_header = (Rtp_Header*)rtp_buf;
    unsigned char* nalu = rtp_buf + RTP_FIXED_HEADER_SIZE;
    int nalu_len = rtp_buf_len - RTP_FIXED_HEADER_SIZE;
    Nalu_Type* nalu_type = (Nalu_Type*)nalu;

    if( rtp_buf_len <= RTP_FIXED_HEADER_SIZE )
    {
        fprintf(stderr, "%s: not rtp packet\n", __FUNCTION__);
        return false;
    }

    if( rtp_header->version != 2)
    {
        fprintf(stderr, "%s: rtp version err: %d\n", __FUNCTION__, rtp_header->version);
        return false;
    }

    if( rtp_header->payload_type != RTP_PAYLOAD_TYPE_H264)
    {
        fprintf(stderr, "%s: rtp payload err: %d\n", __FUNCTION__, rtp_header->payload_type);
        return false;
    }

    //Non-interleaved Mode:Type[1-23,24,28] packetization-mode=1
    if(nalu_type->type >= 1 && nalu_type->type <= 23)
    {
        memcpy(_nalu_buf, nalu_start_code, sizeof(nalu_start_code) );
        _nalu_len  = sizeof(nalu_start_code);
        memcpy(_nalu_buf+_nalu_len, nalu, nalu_len);
        _nalu_len += nalu_len;
        return true;
    }
    else if(FU_A == nalu_type->type )
    {
        Fu_Indicator* fu_indicator = (Fu_Indicator*)nalu;
        Fu_Header*  fu_header = (Fu_Header*)( nalu+sizeof(Fu_Indicator) );
        int nalu_header_len = sizeof(Fu_Indicator) + sizeof(Fu_Header);

        if(1 == fu_header->start && 1 == fu_header->end)
        {
            fprintf(stderr, "%s: FU_A: start==end==1\n", __FUNCTION__);
            return false;
        }
        else if( 1 == fu_header->start )
        {
            Nalu_Type nalu_type;

            memset(_nalu_buf, 0, sizeof(RTP_MAX_BUF_SIZE) );
            memcpy(_nalu_buf, nalu_start_code, sizeof(nalu_start_code) );
            _nalu_len = sizeof(nalu_start_code);

            nalu_type.fbit = fu_indicator->fbit;
            nalu_type.nri = fu_indicator->nri;
            nalu_type.type = fu_header->type;

            memcpy(_nalu_buf+_nalu_len, &nalu_type, sizeof(nalu_type) );
            _nalu_len += sizeof(nalu_type);

            memcpy(_nalu_buf+_nalu_len, nalu + nalu_header_len, nalu_len - nalu_header_len );
            _nalu_len += nalu_len - nalu_header_len;

            return false;
        }
        else if(1 == fu_header->end )
        {
            if( _nalu_len <= 0)
            {
                fprintf(stderr, "%s: FU_A: end-----_nalu_len=%d\n", __FUNCTION__, _nalu_len);
                return false;
            }

            memcpy(_nalu_buf+_nalu_len, nalu + nalu_header_len, nalu_len - nalu_header_len );
            _nalu_len += nalu_len - nalu_header_len;

            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:nalu_len = %d!\n"), __PRETTY_FUNCTION__, _nalu_len));

//            out_buf_len = _nalu_len;
//            memcpy(out_buf, _nalu_buf, out_buf_len);

//            _nalu_len = 0;
            return true;
        }
        else
        {
            if( _nalu_len <= 0)
            {
                fprintf(stderr, "%s: FU_A: between-----_nalu_len=%d\n", __FUNCTION__, _nalu_len);
                return false;
            }

            memcpy(_nalu_buf+_nalu_len, nalu + nalu_header_len, nalu_len - nalu_header_len );
            _nalu_len += nalu_len - nalu_header_len;
            return false;
        }
    }
    else
    {
        fprintf(stderr, "%s: nalu_type.type=%d(not support)\n", __FUNCTION__, nalu_type->type );
        return false;
    }

    return false;
}

