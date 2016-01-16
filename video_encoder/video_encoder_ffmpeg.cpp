#include "video_encoder_ffmpeg.h"
#include "../../suyun_sdk/log.h"
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
video_encoder_ffmpeg::video_encoder_ffmpeg():_codec(0),_codec_contex(0),_codec_param(0),_inited(false)
{

}

video_encoder_ffmpeg::~video_encoder_ffmpeg()
{

}
bool video_encoder_ffmpeg::operator () (uint8_t **yuv_data, int *linesize, int top_or_bottom)
{
    AVPacket pkt;

    bool ret =false;
    do
    {
        for(int i=0;i<3;++i)
        {
            _encode_frame->data[i] = yuv_data[i];
            _encode_frame->linesize[i] = linesize[i];
        }
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;


         int  got_output = 1;
        /* encode the image */
        ret = avcodec_encode_video2(_codec_contex, &pkt, _encode_frame, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            break;
        }
        if (got_output) {
            int is_key_frame = AV_PKT_FLAG_KEY &pkt.flags;
//            if(pkt.flags!=0)
            if(is_key_frame)
                fprintf(stderr,"got pic flags:%d,key frame:%d\n",_codec_contex->flags,is_key_frame);

            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~zfl~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            gettimeofday(&tv_current, NULL);
            struct timeval tv_result;
            timersub(&tv_current, &tv_start, &tv_result);

            u_int64_t  tt = tv_result.tv_sec * 1000000 + tv_result.tv_usec;

            tt -= 1000;
            tt = ( tt * sample_rate ) / 1000000;

            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//            (*_cb)(pkt.data,pkt.size,is_key_frame,0);
            (*_cb)(pkt.data,pkt.size,is_key_frame,tt);
            printf("e 1~~~~~~~%d\n", pkt.size);
            av_free_packet(&pkt);
        }

        /* get the delayed frames */
        for ( int i=0,got_output = 1; got_output; i++) {
            ret = avcodec_encode_video2(_codec_contex, &pkt, NULL, &got_output);
            if (ret < 0) {
                fprintf(stderr, "Error encoding frame left\n");
                break;
            }
            if (got_output) {
                int is_key_frame = AV_PKT_FLAG_KEY &_codec_contex->flags;

                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~zfl~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                gettimeofday(&tv_current, NULL);
                struct timeval tv_result;
                timersub(&tv_current, &tv_start, &tv_result);

                u_int64_t  tt = tv_result.tv_sec * 1000000 + tv_result.tv_usec;

                tt -= 1000;
                tt = ( tt * sample_rate ) / 1000000;

                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//                (*_cb)(pkt.data,pkt.size,is_key_frame,0);
                (*_cb)(pkt.data,pkt.size,is_key_frame,tt);
                printf("e 2~~~~~~~%d\n", pkt.size);
                av_free_packet(&pkt);
            }
        }
        ret = true;
    }while(0);
    return ret;



}
int video_encoder_ffmpeg::set_encoder_param(T_encode_param &param)
{
    _codec_contex->width = param.w;
    _codec_contex->height = param.h;
    _codec_contex->time_base = AVRational{param.fps,1};
    _codec_contex->bit_rate = param.bitrate*1024;
    _codec_contex->gop_size = param.gop;
    return 0;

}
int video_encoder_ffmpeg::get_encoder_param(T_encode_param &param)
{
     param.w = _codec_contex->width;
     param.h = _codec_contex->height;
     param.fps = _codec_contex->framerate.den/_codec_contex->time_base.num;
     param.bitrate = _codec_contex->bit_rate/1024;
     param.gop = _codec_contex->gop_size;
    return 0;
}
bool video_encoder_ffmpeg::init(NALU_CB *cb,T_encode_param &param)
{
    if(param.codec_id == 0)
    {
        param.codec_id = AV_CODEC_ID_HEVC;
    }

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~zfl~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    sample_rate = 90000;
    gettimeofday(&tv_start, NULL);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~zfl~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    avcodec_register_all();
    _encode_param = param;
    _cb = cb;
    bool ret = false;
    do{
        /* find the mpeg1 video encoder */
        _codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);//((AVCodecID)param.codec_id);
        if (!_codec) {
            fprintf(stderr, "Codec not found :%d\n",param.codec_id);
            break;
        }

        _codec_contex = avcodec_alloc_context3(_codec);
        if (!_codec_contex) {
            fprintf(stderr, "Could not allocate video codec context\n");
            break;
        }
//        _codec_contex->flags|= CODEC_FLAG_GLOBAL_HEADER;
        /* put sample parameters */
        _codec_contex->bit_rate = param.bitrate*1024;
        /* resolution must be a multiple of two */
        _codec_contex->width = param.w;
        _codec_contex->height = param.h;
        /* frames per second */
        _codec_contex->time_base = (AVRational){1,param.fps};
        /* emit one intra frame every ten frames
             * check frame pict_type before passing frame
             * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
             * then gop_size is ignored and the output of encoder
             * will always be I frame irrespective to gop_size
             */
        _codec_contex->gop_size = param.fps;
        _codec_contex->max_b_frames = 1;
        _codec_contex->pix_fmt = AV_PIX_FMT_YUV420P;


        //H.264
        if(_codec_contex->codec_id == AV_CODEC_ID_H264) {
            av_dict_set(&_codec_param, "preset", "ultrafast", 0);
            av_dict_set(&_codec_param, "tune", "zerolatency", 0);
            av_dict_set(&_codec_param, "profile", "baseline", 0);
//            av_dict_set(&_codec_param, "qmin", "0", 0);
//            av_dict_set(&_codec_param, "qmax", "69", 0);
//            av_dict_set(&_codec_param, "qdiff", "4", 0);
        }
        //H.265
        if(_codec_contex->codec_id == AV_CODEC_ID_H265){
            av_dict_set(&_codec_param, "x265-params", "qp=20", 0);
            av_dict_set(&_codec_param, "preset", "ultrafast", 0);
            av_dict_set(&_codec_param, "tune", "zero-latency", 0);
//            av_dict_set(&_codec_param, "qmin", "0", 0);
//            av_dict_set(&_codec_param, "qmax", "69", 0);
//            av_dict_set(&_codec_param, "qdiff", "4", 0);
//            av_dict_set(&_codec_param, "profile", "main", 0);
        }
        /* open it */
        if (avcodec_open2(_codec_contex, _codec, &_codec_param) < 0) {
            fprintf(stderr, "Could not open codec\n");
            break;
        }

        _encode_frame = av_frame_alloc();
        if (!_encode_frame) {
            fprintf(stderr, "Could not allocate video frame\n");
            break;
        }
        _encode_frame->format = _codec_contex->pix_fmt;
        _encode_frame->width  = _codec_contex->width;
        _encode_frame->height = _codec_contex->height;

        /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
        //        ret = av_image_alloc(_encode_frame->data, _encode_frame->linesize,
        //                             _encode_frame->width, _encode_frame->height,
        //                             _encode_frame->format, 32);
        //        if (ret < 0) {
        //            fprintf(stderr, "Could not allocate raw picture buffer\n");
        //            break;
        //        }

        ret = true;

        _inited = true;
    }while(0);
    return ret;
}
void video_encoder_ffmpeg::release()
{
    if(_codec){
        avcodec_close(_codec_contex);
        _codec= NULL;
    }
    if(_codec_contex){

        av_free(_codec_contex);
        _codec_contex = NULL;
    }
    if(_encode_frame){
        //av_freep(&_encode_frame->data[0]);
        av_frame_free(&_encode_frame);
        _encode_frame = NULL;
    }
    _inited = false;
//    return 0;
}
