#ifndef Video_Decoder_FFmpeg_H
#define Video_Decoder_FFmpeg_H

#include "video_decoder.h"

struct AVCodec;
struct AVCodecContext;
struct AVFrame;

class Video_Decoder_FFmpeg: public Video_Decoder
{
public:
    Video_Decoder_FFmpeg();
    ~Video_Decoder_FFmpeg();

    bool   operator () (unsigned char *nalu, int nalu_len );
    bool   decode (unsigned char *nalu, int nalu_len);

private:
    bool    init( YUV_CB *cb, Codec_Type codec_type);
    void   release();
    bool   codec_init( Codec_Type codec_type);

private:

    YUV_CB *_cb;

    AVCodec *_codec;
    AVCodecContext *_context;
    AVFrame *_picture;
};

#endif
