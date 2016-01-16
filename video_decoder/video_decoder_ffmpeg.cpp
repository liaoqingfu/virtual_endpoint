
#include <unistd.h>
#include <iostream>
#include <cstdio>

#ifdef __cplusplus
extern "C"{
#endif
//#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

#include "video_decoder_ffmpeg.h"
#include "get_slice_type.h"


Video_Decoder_FFmpeg::Video_Decoder_FFmpeg():
    _cb(NULL), _codec(NULL), _context(NULL),
    _picture(NULL)
{
}

Video_Decoder_FFmpeg::~Video_Decoder_FFmpeg()
{
    //    release();
}

bool   Video_Decoder_FFmpeg::init(YUV_CB *cb, Codec_Type codec_type)
{
    _cb = cb;

    return codec_init(codec_type);
}

void   Video_Decoder_FFmpeg::release()
{
    if(_codec)
    {
        avcodec_close ( _context );
        _codec = NULL;
    }
    if ( _context != NULL )
    {
        avcodec_free_context ( &_context );
        _context = NULL;
    }
    if ( _picture != NULL )
    {
        av_frame_free ( &_picture );
        _picture = NULL;
    }
}

bool   Video_Decoder_FFmpeg::operator () (unsigned char *nalu, int nalu_len )
{
    return decode(nalu,nalu_len);
}

//#define DEBUG
bool   Video_Decoder_FFmpeg::decode (unsigned char *nalu, int nalu_len)
{
#ifdef DEBUG

    static  FILE* const hf = fopen("nalu.264","wb");
    if(hf)
        fwrite(nalu,1,nalu_len,hf);
    //return true;
#endif

    int len = 0;
    int got_picture = 0;
    AVPacket av_pkt;
    av_init_packet(&av_pkt);
    len = nalu_len;

    av_pkt.data = (unsigned char*)nalu;
    av_pkt.size = nalu_len;

    while( av_pkt.size > 0)
    {
        try
        {
            len = avcodec_decode_video2 ( _context, _picture, &got_picture, &av_pkt );
            if (len < 0)
            {
                printf("Error while decoding frame\n");
                return false;
            }
            if(len == 0)
            {
                printf("no frame while decoding frame \n");
                return false;
            }
        }
        catch(...)
        {
            printf("capture error while decoding frame \n");
            return false;
        }
#if 0
        if ( got_picture )
        {
            // printf("decode got pic type:%d\n",picture->pict_type);
            if(NULL == _sws_context && _rescale)
            {

                _picture_sws->format = AV_PIX_FMT_YUV420P;
                _picture_sws->width  = _context->width;
                _picture_sws->height = _context->height;
                printf("contex picture fmx:%d ,AV_PIX_FMT_YUV420P:%d\n",_context->pix_fmt,AV_PIX_FMT_YUV420P);
                av_image_alloc(_picture_sws->data, _picture_sws->linesize, _context->width, _context->height,
                               AV_PIX_FMT_YUV420P, 1);
                _sws_context = sws_getCachedContext(_sws_context,
                                                    _context->width,
                                                    _context->height,
                                                    _context->pix_fmt,
                                                    _context->width,
                                                    _context->height,
                                                    AV_PIX_FMT_YUV420P,
                                                    SWS_BILINEAR,
                                                    NULL,
                                                    NULL,
                                                    NULL
                                                    );
                if(_sws_context == NULL)
                {
                    printf ( "%s sws_getContext failed\n", __FUNCTION__ );
                    return false;
                }
                else
                {
                    printf( "%s sws_getContext success", __FUNCTION__ );
                }

            }
        }

        if(_rescale)
        {
            sws_scale
                    (
                        _sws_context,
                        (uint8_t const * const *)_picture->data,
                        _picture->linesize,
                        0,
                        _context->height,
                        _picture_sws->data,
                        _picture_sws->linesize
                        );
            (*_cb)(_context->width, _context->height,(unsigned char**) _picture_sws->data, _picture_sws->linesize);
        }
        else
        {
#endif
            if(_context->width <= 0 || _context->height <= 0)
            {
                return false;
            }
            (*_cb)((unsigned char**) _picture->data, _context->width, _context->height,_picture->linesize);

            av_pkt.size -= len;
            av_pkt.data += len;
    }

    return true;
}


bool   Video_Decoder_FFmpeg::codec_init(Codec_Type codec_type)
{
    bool ret = false;
    do{
        avcodec_register_all();
        if(Video_Decoder::H264 == codec_type)
        {
            _codec = avcodec_find_decoder ( AV_CODEC_ID_H264 );
        }
        else if(Video_Decoder::H265 == codec_type)
        {
            _codec = avcodec_find_decoder ( AV_CODEC_ID_H265 );
        }
        else
        {
            printf("%s: avcodec_find_decoder err: %d\n", __FUNCTION__, codec_type);
            break;
        }

        if( !_codec )
        {
            printf ( "%s codec : CODEC_ID_H264 not found!\n", __FUNCTION__ );
            break;
        }

        _context = avcodec_alloc_context3(_codec);
        if ( _context == NULL )
        {
            printf ( "%s avcodec_alloc_context : alloc context failed!\n", __FUNCTION__ );
            break;
        }

        if ( avcodec_open2 ( _context, _codec, NULL ) < 0 )
        {
            printf ( "%s codec : CODEC_ID_H264 open failed!\n", __FUNCTION__ );
            break;
        }
        _picture = av_frame_alloc();

        if ( _picture == NULL )
        {
            printf ( "%s avcodec_alloc_frame : alloc frame failed!\n", __FUNCTION__ );
            break;
        }

        ret = true;
    }while(0);

    if(ret == true)
    {
        printf ( "%s ffmpeg init success!\n", __FUNCTION__);
    }
    else
    {
        release();
    }

    return ret;
}





