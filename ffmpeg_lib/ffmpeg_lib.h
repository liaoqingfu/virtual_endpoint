#ifndef __FMPEG_LIB_H__
#define __FMPEG_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USE_DLOPEN
#define __STDC_CONSTANT_MACROS

#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"

class ffmpeg_lib {
public:
    static bool init();
    static void release();
    typedef void (*avcodec_init_func) (void);
    typedef void (*avcodec_register_func)(AVCodec *codec);
    typedef void (*avcodec_register_all_func) (void);
    typedef int (*avcodec_close_func) (AVCodecContext *);
    //typedef AVCodec *(*avcodec_find_decoder_func) (enum CodecID id);
    //typedef AVCodec *(*avcodec_find_encoder_func) (enum CodecID id);
    //new
    typedef AVCodec *(*avcodec_find_decoder_func) (enum AVCodecID id);
    typedef AVCodec *(*avcodec_find_encoder_func) (enum AVCodecID id);

    //attribute_deprecated
    //typedef AVCodecContext *(*avcodec_alloc_context_func) (void);
    //typedef AVCodecContext *(*avcodec_alloc_context3_func)(AVCodec  *codec);

    //new
    typedef AVCodecContext *(*avcodec_alloc_context3_func)(AVCodec const *codec);
    typedef AVFrame *(*avcodec_alloc_frame_func) (void);

    //attribute_deprecated
    //typedef int (*avcodec_open_func) (AVCodecContext *, AVCodec *);
    //typedef int (*avcodec_open2_func)(AVCodecContext *, AVCodec*, AVDictionary **);
    //new
    typedef int (*avcodec_open2_func)(AVCodecContext *, AVCodec const*, AVDictionary **);

    typedef int (*avcodec_decode_video2_func) (AVCodecContext *, AVFrame *, int *, const AVPacket *);

    typedef int (*avcodec_encode_video_func) (AVCodecContext * avctx, uint8_t * buf,
                                              int buf_size, const AVFrame * pict);
    typedef void (*av_free_func) (void *ptr);
    typedef void (*avcodec_set_dimensions_func)(AVCodecContext *ctx, int width, int height);
    //not used
    //typedef int (*avpicture_alloc_func) (AVPicture * picture, int pix_fmt, int width, int height);
    typedef void (*avpicture_free_func) (AVPicture * picture);
    typedef void (*av_init_packet_func) (AVPacket * pkt);
    //const
    //typedef int (*avpicture_fill_func) (AVPicture * picture, const uint8_t * ptr,
                                        //enum PixelFormat pix_fmt, int width, int height);
    //typedef int (*avpicture_fill_func) (AVPicture * picture, uint8_t* ptr,
      //                                      enum PixelFormat pix_fmt, int width, int height);

    typedef int (*avpicture_layout_func)(const AVPicture* src, enum PixelFormat pix_fmt, int width, int height,
                                         unsigned char *dest, int dest_size);
    typedef int (*avpicture_get_size_func)(enum PixelFormat pix_fmt, int width, int height);


    typedef int (*sws_scale_func)(struct SwsContext *context, const uint8_t* const srcSlice[], const int srcStride[],
                                  int srcSliceY, int srcSliceH, uint8_t* const dst[], const int dstStride[]);



    typedef struct SwsContext * ( *sws_getContext_func)(int srcW, int srcH, enum PixelFormat srcFormat,
                                                        int dstW, int dstH, enum PixelFormat dstFormat,
                                                        int flags, SwsFilter *srcFilter,
                                                        SwsFilter *dstFilter, const double *param);
    typedef void (*sws_freeContext_func)(struct SwsContext *swsContext);

public:
    static avcodec_register_func avcodec_register;
    static avcodec_decode_video2_func avcodec_decode_video2;
    static avcodec_encode_video_func avcodec_encode_video;
    //static avpicture_fill_func avpicture_fill;
    static sws_scale_func sws_scale;
    static avpicture_layout_func avpicture_layout;
    static avpicture_get_size_func  avpicture_get_size;
    static av_init_packet_func av_init_packet;

    //static avcodec_init_func avcodec_init;
    static avcodec_register_all_func avcodec_register_all;
    static avcodec_close_func avcodec_close;
    static avcodec_find_decoder_func avcodec_find_decoder;
    static avcodec_find_encoder_func avcodec_find_encoder;

    //static avcodec_alloc_context_func avcodec_alloc_context;
    static avcodec_alloc_context3_func avcodec_alloc_context3;
    static avcodec_alloc_frame_func avcodec_alloc_frame;
    //static avcodec_open_func avcodec_open;
    static avcodec_open2_func avcodec_open2;

    static av_free_func av_free;
    //static avpicture_alloc_func avpicture_alloc;
    static avpicture_free_func avpicture_free;

    static sws_getContext_func sws_getContext;
    static sws_freeContext_func sws_freeContext;

    static avcodec_set_dimensions_func avcodec_set_dimensions;

    static void *avcodec_handle;
    static void *avutil_handle;
    static void *swscale_handle;

private:
    static bool _is_init;
};

#ifdef __cplusplus
}
#endif

#endif
