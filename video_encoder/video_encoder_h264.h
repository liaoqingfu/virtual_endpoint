#ifndef VIDEO_ENCODER_H264_H
#define VIDEO_ENCODER_H264_H

#include <cstdint>

#ifdef __cplusplus
extern "C"{
#endif

#include <x264.h>

#ifdef __cplusplus
}
#endif

#include "video_encoder.h"
#include "encoder_types.h"

class Video_Encoder_H264: public Video_Encoder
{
public:
    Video_Encoder_H264();
    ~Video_Encoder_H264();

    bool operator () (uint8_t **yuv_data, int *linesize, int top_or_bottom_field = 0);
    void dump_config();

private:
    bool init(Video_Encoder::NALU_CB *cb, const Encoder_Param& param);
    void release();

    bool codec_init(const Encoder_Param& param);
    void codec_release();
    int  set_encoder_param(const Encoder_Param& encode_param);

private:
    x264_t*                _x264_h;
    x264_picture_t   _x264_pic;//合成时用的缓冲
    x264_param_t     _x264_param;

    unsigned char*  _nalu_buf;
    int _nalu_buf_size;

    Video_Encoder::NALU_CB *_nalu_cb;

    int64_t             _i_frame;
    unsigned int    _sample_rate;
    struct timeval _tv_start;
    struct timeval _tv_current;
    std::uint8_t*   _img_plane;  //just for free  x264_picture_clean
};

#endif //VIDEO_ENCODER_H264_H
