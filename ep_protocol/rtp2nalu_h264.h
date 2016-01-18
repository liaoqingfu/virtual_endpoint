#ifndef RTP2NALU_H264_H
#define RTP2NALU_H264_H

#include "rtp_recv_handler.h"

class Rtp2Nalu_H264 : public Rtp_Recv_Handler
{
public:
    Rtp2Nalu_H264(Handler_Cb* handler_cb);
    virtual ~Rtp2Nalu_H264();

    virtual bool rtp_parse(unsigned char* rtp_buf, int rtp_buf_len, unsigned char* out_buf, int& out_buf_len);

private:
    unsigned char*      _nalu_buf;
    int         _nalu_len;
};

#endif // RTP2NALU_H264_H
