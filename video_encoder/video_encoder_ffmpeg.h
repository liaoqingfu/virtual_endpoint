#ifndef VIDEO_ENCODER_FFMPEG_H
#define VIDEO_ENCODER_FFMPEG_H
#include "video_encoder.h"

struct AVCodec;
struct AVCodecContext;
struct AVDictionary;
struct AVFrame;
class video_encoder_ffmpeg:public Video_Encoder
{
public:
    video_encoder_ffmpeg();
    ~video_encoder_ffmpeg();
     bool operator () (uint8_t **yuv_data, int *linesize, int top_or_bottom = 0);
     bool init(NALU_CB *cb,T_encode_param &param);
     void release();
     int set_encoder_param(T_encode_param &param);
     int get_encoder_param(T_encode_param &param);
     bool _inited;
private:
    AVCodec *_codec;
    AVCodecContext *_codec_contex;
    AVDictionary *_codec_param;
    AVFrame *_encode_frame;
    T_encode_param _encode_param;
    NALU_CB *_cb;
    unsigned int sample_rate;
    struct timeval tv_start;
    struct timeval tv_current;
};

#endif // VIDEO_ENCODER_FFMPEG_H
