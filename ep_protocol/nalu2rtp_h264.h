#ifndef NALU2RTP_H264_H
#define NALU2RTP_H264_H

#include "nalu2rtp_handler.h"

class Nalu2Rtp_H264 : public Nalu2Rtp_Handler
{
public:
    Nalu2Rtp_H264();
    ~Nalu2Rtp_H264();

    virtual int pack_and_send(unsigned char*, int len, unsigned int timestamp);

private:
    unsigned char*  _rtp_buf;
};

#endif // NALU2RTP_H264_H
