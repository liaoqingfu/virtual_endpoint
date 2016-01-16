#ifndef VIDEO_DECODER_V3_H
#define VIDEO_DECODER_V3_H

#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "ffmpeg_lib.h"

#include "video_decoder.h"
#include "get_slice_type.h"
#include "../../suyun_sdk/spend_timer_logger.h"
#define LOG_INFO  printf
#define LOG_ERROR printf
#define LOG_DEBUG printf
#define LOG_NOTICE printf
#define VIDEO_WIDTH_V2  1920
#define VIDEO_HEIGHT_V2 1080

#pragma pack(1)

//#define DEBUG
class Video_Decoder_Impl_V3:public Video_Decoder
{
public:
    Video_Decoder_Impl_V3():
        cb(NULL), codec(NULL), context(NULL),
        picture(NULL), opts(NULL), first_i_found(false),
        yuv(NULL), yuv_len(0)
    {
    }
    virtual ~Video_Decoder_Impl_V3()
    {}

public:
    bool   init(YUV_CB *_cb,int codec_id=CODECID_H264)
    {

        cb = _cb;

        bool fr = codec_init();

        yuv = new unsigned char[VIDEO_WIDTH_V2 * VIDEO_HEIGHT_V2 * 2];
        if(NULL == yuv)
        {
            LOG_ERROR("%s : new yuv failed!\n", __FUNCTION__);
            //printf("%s: new yuv failed!\n", __FUNCTION__);
            return false;
        }

        ffmpeg_lib::av_init_packet(&av_pkt);

        return fr;
    }

    int   release()
    {
        codec_release();

        if(yuv != NULL)
        {
            delete [] yuv;
            yuv = NULL;
        }

        yuv_len = 0;

        delete this;
        return 0;
    }
    bool   operator () (unsigned char *nalu, int nalu_len)
    {
        return decode(nalu,nalu_len);
    }
    bool   decode (unsigned char *nalu, int nalu_len)
    {
#ifdef DEBUG
        Spend_Timer_Logger timer_log("decode frame",false);
#endif
        int len = 0;
        int got_picture = 0;

        unsigned char i_start[] = {0x00, 0x00, 0x01};
        unsigned char i_start1[] = {0x00, 0x00, 0x00, 0x01};

        //unsigned char i_sps1[] = {0x00, 0x00, 0x00, 0x01, 0x67};
        //unsigned char i_pps2[] = {0x00, 0x00, 0x00, 0x01, 0x68};
        //unsigned char i_slice[] = {0x00, 0x00, 0x00, 0x01, 0x65};

        uint8_t* nalu_buf = (uint8_t*)nalu;
        len = nalu_len;

        int v = -1;
        unsigned int slice_type = 0;

        if ( !first_i_found )
        {
            if(0 == memcmp(nalu_buf, i_start, 3))
            {
                v = (*(nalu_buf + 3)) & 0x1F;
                if( v == 1)
                {
                    unsigned int index1 = 0;
                    get_ue_value(nalu_buf + 4, len - 4, index1);

                    int slice_type = get_ue_value(nalu_buf + 4, len - 4, index1);
                    //LOG_NOTICE("\nslice_type = %d \n", slice_type);
                    if( (slice_type == 2) || (slice_type == 4) || (slice_type == 7) || (slice_type == 9)) //
                    {
                        first_i_found = true;
                        LOG_NOTICE("first i found i v = %d slice_type = %u \n", v, slice_type);
                    }
                    else
                    {
                        return true;
                    }
                }
                else if( (v == 7) ) //|| (v == 5)
                {
                    first_i_found = true;
                    LOG_NOTICE("first i found ii v = %u\n", v);
                }
                else
                    return true;
            }
            else if(0 == memcmp(nalu_buf, i_start1, 4))
            {
                v = (*(nalu_buf + 4)) & 0x1F;
                if( v == 1)
                {
                    //哥伦布解码ue(2)
                    unsigned int index1 = 0;
                    get_ue_value(nalu_buf +5, len -5, index1);

                    slice_type = get_ue_value(nalu_buf +5, len -5, index1);
                    //LOG_INFO("slice_type = %u \n",slice_type);
                    if( (slice_type == 4) || (slice_type == 7) || (slice_type == 9) ) //(slice_type == 2) ||
                    {
                        first_i_found = true;
                        LOG_NOTICE("first i found iii v = %d slice_type = %u \n", v, slice_type);
                    }
                    else
                    {
                        return true;
                    }

                }
                else if( (v == 7)  )//|| (v == 5)
                {
                    first_i_found = true;
                    LOG_NOTICE("first i found iiii v = %u\n", v);
                }
                else
                    return true;
            }
            else
                return true;
        }

        {
            got_picture = 0;

            av_pkt.data = (unsigned char*)nalu;
            av_pkt.size = nalu_len;
            if(1)
            {
                while( av_pkt.size > 0)
                {
                    len = ffmpeg_lib::avcodec_decode_video2 ( context, picture, &got_picture, &av_pkt );

                    if (len < 0)
                    {
                        LOG_ERROR("Error while decoding frame \n");
                        break;
                    }
#ifdef DEBUG
                    timer_log.print_elapsed();
#endif
                    if ( got_picture )
                    {
//                        printf("[width %d][height %d][linesize %d %d %d] [pid:%d]", context->width, context->height, picture->linesize[0], picture->linesize[1], picture->linesize[2], getpid());
                        (*cb)(context->width, context->height, picture->data, picture->linesize);
//                        static int fd = open("gggg", O_RDWR|O_CREAT|O_TRUNC);
//                        for(int i=0; i<3; ++i)
//                        {
//                            write(fd, picture->data[i], picture->linesize[i]);
//                        }
//                        for(int i=0; i<3; ++i)
//                        {
//                            write(fd, picture->data[i], picture->linesize[i]);
//                        }
//                        for(int i=0; i<3; ++i)
//                        {
//                            write(fd, picture->data[i], picture->linesize[i]);
//                        }
                    }
                    av_pkt.size -= len;
                    av_pkt.data += len;
                }
            }
        }
        return true;
    }

private:
    bool   codec_init()
    {
        char const *dll_name = NULL;

        if ( 1 )
        {
            dll_name = "avcodec-54.dll";
        }

        if ( !ffmpeg_lib::init ( ) )
        {
            LOG_ERROR ( "%s ffmpeg_lib : ffmpeg_lib init error!\n", __FUNCTION__ );
            //Sleep ( 1000 );
            return  false;
        }

        ffmpeg_lib::avcodec_register_all();

        //ffmpeg_lib::av_dict_set(&opts, "b", "2.5M", 0);

        /* find the h264 video decoder */
        codec = ffmpeg_lib::avcodec_find_decoder ( CODEC_ID_H264 );
        if ( codec == NULL )
        {
            LOG_ERROR ( "%s codec : CODEC_ID_H264 not found!\n", __FUNCTION__ );

            return false;
        }

        context = ffmpeg_lib::avcodec_alloc_context3(codec);
        if ( context == NULL )
        {
            LOG_ERROR ( "%s avcodec_alloc_context : alloc context failed!\n", __FUNCTION__ );
            return false;
        }

        context->flags |= ~CODEC_FLAG_GLOBAL_HEADER;
        //context->flags |= CODEC_FLAG_QSCALE;

        //if(codec->capabilities&CODEC_CAP_TRUNCATED)
        //{
        //    context->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */
        //}

        /* open it */
        if ( ffmpeg_lib::avcodec_open2 ( context, codec, &opts ) < 0 )
        {
            LOG_ERROR ( "%s codec : CODEC_ID_H264 open failed!\n", __FUNCTION__ );
            return false;
        }

        picture = ffmpeg_lib::avcodec_alloc_frame();
        if ( picture == NULL )
        {
            LOG_ERROR ( "%s avcodec_alloc_frame : alloc frame failed!\n", __FUNCTION__ );
            return false;
        }

        {
            LOG_DEBUG ( "%s", "ffmpeg init success!\n" );
        }

        return true;
    }

    void   codec_release()
    {
        //picture如何释放?
        //return;
        if ( context != NULL )
        {
            ffmpeg_lib::avcodec_close ( context );
            ffmpeg_lib::av_free ( context );
        }
        if ( picture != NULL )
        {
            ffmpeg_lib::av_free ( picture );
        }
    }

    bool   codec_reinit()
    {
        codec_release();
        return codec_init();
    }

private:
    Video_Decoder::YUV_CB *cb;

    AVCodec *codec;
    AVCodecContext *context;
    AVFrame *picture;

    AVDictionary *opts;

    bool first_i_found;

    unsigned char *yuv;
    int	yuv_len;

    AVPacket av_pkt;
};

#pragma pack()

#endif
