#ifndef VIDEO_ENCODER_H265_H
#define VIDEO_ENCODER_H265_H

#include "x265.h"
#include "video_encoder.h"

class Video_Encoder_H265: public Video_Encoder
{
public:
    Video_Encoder_H265();
    ~Video_Encoder_H265();

    virtual bool operator () (uint8_t **yuv_data, int *linesize, int top_or_bottom_field = 0);

private:
    virtual bool init(Video_Encoder::NALU_CB *cb, const Encoder_Param &param);
    virtual void release();

private:
    x265_encoder* _x265_encoder;
    x265_nal* _x265_nal;
    x265_picture* _x265_picture;
    x265_param* _x265_param;
    Video_Encoder::NALU_CB* _callback;

    uint32_t _sample_rate;
    struct timeval _tv_start;
    struct timeval _tv_current;
};

#endif // VIDEO_ENCODER_H265_H
