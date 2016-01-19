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

int Nalu2Rtp_H264::pack_and_send(unsigned char *nalu, int nalu_len, unsigned int ts)
{
#if 0
    {
        static unsigned short sequence = 0;
        int  i, count, bulk;
        int  nc_len, np_len;
        unsigned char *nc = NULL, *np = NULL;
        char delim1[4] = {0x00, 0x00, 0x00, 0x01};
        char delim2[3] = {0x00, 0x00, 0x01};
        Rtp_Header *rtp_header = (Rtp_Header *)_rtp_buf;

        //printf ( "11111111111111111111111111 : rtp_header = %p\n", rtp_header);
        Nalu_Type    nalu_type;
        Fu_Indicator fu_indicator;
        Fu_Header    fu_header;

        if (memcmp(nalu, delim1, sizeof(delim1)) == 0 )
        {
            nc = nalu + sizeof(delim1);
            nc_len = nalu_len - sizeof(delim1);
        }
        else if (memcmp(nalu, delim2, sizeof(delim2)) == 0 )
        {
            nc = nalu + sizeof(delim2);
            nc_len = nalu_len - sizeof(delim2);
        }
        else
        {
            return -1;
        }

        np = nc + sizeof(nalu_type);
        np_len = nc_len - sizeof(nalu_type);

        rtp_header->version = 2;
        rtp_header->padbit = 0;
        rtp_header->extbit = 0;
        rtp_header->cc = 0;
        rtp_header->markbit= 0;
        rtp_header->payload_type = RTP_PAYLOAD_TYPE_H264;
        rtp_header->timestamp = htonl(ts);// * (sample_rate / fps));
        rtp_header->ssrc = htonl(_ssrc);

        if (np_len <= RTP_FRAGMENT_SIZE)
        {
            rtp_header->sequence = htons(sequence++);
            memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE, nc, nc_len);
            rtp_sendto(_rtp_buf, RTP_FIXED_HEADER_SIZE + nc_len);

            return 0;
        }


        nalu_type = *(Nalu_Type*)nc;
        fu_indicator.fbit  = nalu_type.fbit;
        fu_indicator.nri   = nalu_type.nri;
        fu_indicator.type  = RTP_H264_FU_A;
        fu_header.reserved = 0;
        fu_header.type     = nalu_type.type;

        count = (np_len + RTP_FRAGMENT_SIZE - 1)/RTP_FRAGMENT_SIZE;
        for (i = 0; i < count; i++)
        {
            rtp_header->sequence = htons(sequence++);

            if (i == 0)
            {
                fu_header.start = 1;
                fu_header.end   = 0;
            }

            else if (i == (count - 1))
            {
                fu_header.start = 0;
                fu_header.end   = 1;
            }

            else
            {
                fu_header.start = 0;
                fu_header.end   = 0;
            }

            memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE, &fu_indicator, sizeof(fu_indicator));
            memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE + sizeof(fu_indicator), &fu_header, sizeof(fu_header));
            bulk = RTP_FRAGMENT_SIZE < (np_len - i*RTP_FRAGMENT_SIZE) ? RTP_FRAGMENT_SIZE : (np_len - i*RTP_FRAGMENT_SIZE);
            memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE + sizeof(fu_indicator) + sizeof(fu_header), np + i*RTP_FRAGMENT_SIZE, bulk);
            rtp_sendto(_rtp_buf, RTP_FIXED_HEADER_SIZE + sizeof(fu_indicator) + sizeof(fu_header) + bulk);
        }

    }
#else
    static unsigned short sequence = 0;
    static unsigned char delim1[4] = {0x00, 0x00, 0x00, 0x01};
    static unsigned char delim2[3] = {0x00, 0x00, 0x01};
    unsigned char* nc_pos = nalu;
    int nc_len = nalu_len;
    Rtp_Header* rtp_header = (Rtp_Header*)_rtp_buf;

    rtp_header->version = 2;
    rtp_header->padbit = 0;
    rtp_header->extbit = 0;
    rtp_header->cc = 0;
    rtp_header->markbit = 0;
    rtp_header->payload_type = RTP_PAYLOAD_TYPE_H264;
    rtp_header->timestamp = htonl(ts);
    rtp_header->ssrc = htonl(_ssrc);

    if( 0 == memcmp(delim1, nalu, sizeof(delim1) ) )
    {
        nc_pos += sizeof(delim1);
        nc_len -= sizeof(delim1);
    }
    else if( 0 == memcmp(delim2, nalu, sizeof(delim2)) )
    {
        nc_pos += sizeof(delim2);
        nc_len -= sizeof(delim2);
    }
    else
    {
        ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: err nalu data!!!\n"), __PRETTY_FUNCTION__) );
        return -1;
    }

    if(nc_len <= RTP_FRAGMENT_SIZE)
    {
        rtp_header->sequence = htons(sequence++);
        memcpy(_rtp_buf + RTP_FIXED_HEADER_SIZE, nc_pos, nc_len);
        rtp_sendto(_rtp_buf, nc_len + RTP_FIXED_HEADER_SIZE);
        return 0;
    }


    Nalu_Type* nalu_type = (Nalu_Type*)nc_pos;
    nc_pos += sizeof(Nalu_Type);
    nc_len -= sizeof(Nalu_Type);

    Fu_Indicator fu_indicator;
    Fu_Header   fu_header;

    fu_indicator.type = FU_A;
    fu_indicator.nri = nalu_type->nri;
    fu_indicator.fbit = nalu_type->fbit;

    fu_header.reserved = 0;
    fu_header.type = nalu_type->type;

    int rtp_len, bulk;
    int count = (nc_len + RTP_FRAGMENT_SIZE -1) /  RTP_FRAGMENT_SIZE;

    rtp_header->version = 2;
    rtp_header->padbit = 0;
    rtp_header->extbit = 0;
    rtp_header->cc = 0;
    rtp_header->markbit = 0;
    rtp_header->payload_type = RTP_PAYLOAD_TYPE_H264;
    rtp_header->timestamp = htonl(ts);
    rtp_header->ssrc = htonl(_ssrc);

    for(int i = 0; i < count; i++)
    {
        rtp_header->sequence = htons(sequence++);

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

        rtp_len = RTP_FIXED_HEADER_SIZE;
        memcpy(_rtp_buf + rtp_len, &fu_indicator, sizeof(Fu_Indicator));
        rtp_len += sizeof(Fu_Indicator);
        memcpy(_rtp_buf+rtp_len, &fu_header, sizeof(Fu_Header));
        rtp_len += sizeof(Fu_Header);
        bulk = RTP_FRAGMENT_SIZE < (nc_len - i * RTP_FRAGMENT_SIZE) ? RTP_FRAGMENT_SIZE : (nc_len - i * RTP_FRAGMENT_SIZE);
        memcpy(_rtp_buf+rtp_len, nc_pos + i*RTP_FRAGMENT_SIZE, bulk);
        rtp_len += bulk;
        rtp_sendto(_rtp_buf, rtp_len);
    }
#endif
    return 0;
}

