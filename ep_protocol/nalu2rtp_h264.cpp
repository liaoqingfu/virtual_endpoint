#include <cstring>

#include "nalu2rtp_h264.h"
#include "protocol_defines.h"
#include "protocol_types.h"

Nalu2Rtp_H264::Nalu2Rtp_H264()
    :_rtp_buf(new unsigned char[RTP_MAX_BUF_SIZE])
{

}

Nalu2Rtp_H264::~Nalu2Rtp_H264()
{
    delete _rtp_buf;
}

int Nalu2Rtp_H264::pack_and_send(unsigned char *raw_data, int len, unsigned int timestamp)
{
    static unsigned short sequence = 0;
    static char delim1[4] = {0x00, 0x00, 0x00, 0x01};
    static char delim2[3] = {0x00, 0x00, 0x01};
    unsigned char* nc_pos = raw_data;
    int nc_len = len;
    Rtp_Header* rtp_header = (Rtp_Header*)_rtp_buf;

    rtp_header->version = 2;
    rtp_header->padbit = 0;
    rtp_header->extbit = 0;
    rtp_header->cc = 0;
    rtp_header->payload_type = RTP_PAYLOAD_TYPE_H264;
    rtp_header->ssrc = htonl(_ssrc);
    rtp_header->timestamp = htonl(timestamp);


    if( 0 == memcmp(delim1, raw_data, sizeof(delim1) ) )
    {
        nc_pos += sizeof(delim1);
        nc_len -= sizeof(delim1);
    }
    else if( 0 == memcmp(delim2, raw_data, sizeof(delim2)) )
    {
        nc_pos += sizeof(delim2);
        nc_len -= sizeof(delim2);
    }
    else
    {
        ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: err nalu data!!!\n"), __PRETTY_FUNCTION__) );
        return -1;
    }



    if(nc_len - sizeof(Nalu_Type) <= RTP_FRAGMENT_SIZE)
    {
        rtp_header->sequence = htons(++sequence);
        memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE, nc_pos, nc_len);
        rtp_sendto(_rtp_buf, nc_len + RTP_FIXED_HEADER_SIZE);
        return 0;
    }


    Nalu_Type* nalu_type = (Nalu_Type*)nc_pos;
    nc_len -= sizeof(Nalu_Type);

    Fu_Indicator fu_indicator;
    Fu_Header   fu_header;

    fu_indicator.type = FU_A;
    fu_indicator.nri = nalu_type->nri;
    fu_indicator.fbit = nalu_type->fbit;

    fu_header.reserved = 0;
    fu_header.type = nalu_type->type;

    int count = ((nc_len + RTP_FRAGMENT_SIZE -1) /  RTP_FRAGMENT_SIZE );
    for(int i = 0; i < count; ++i)
    {
        rtp_header->sequence = htonl(++sequence);

        if(0 == i)
        {
            fu_header.start = 1;
            fu_header.end = 0;
        }
        else if(i == count - 1)
        {
            fu_header.start = 0;
            fu_header.end = 1;
        }
        else
        {
            fu_header.start = 0;
            fu_header.end = 0;
        }

        int rtp_len = RTP_FIXED_HEADER_SIZE;
        memcpy(_rtp_buf + rtp_len, &fu_indicator, sizeof(Fu_Indicator));
        rtp_len += sizeof(Fu_Indicator);
        memcpy(_rtp_buf+rtp_len, &fu_header, sizeof(Fu_Header));
        rtp_len += sizeof(Fu_Header);
        int bulk = RTP_FRAGMENT_SIZE < (nc_len - i * RTP_FRAGMENT_SIZE) ? RTP_FRAGMENT_SIZE : (nc_len - i * RTP_FRAGMENT_SIZE);
        memcpy(_rtp_buf+rtp_len, nc_pos + i*RTP_FRAGMENT_SIZE, bulk);
        rtp_len += bulk;
        rtp_sendto(_rtp_buf, rtp_len);
    }

    return 0;
}

