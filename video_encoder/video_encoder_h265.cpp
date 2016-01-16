#include <sys/time.h>

#include "video_encoder_h265.h"
#include "encoder_types.h"

Video_Encoder_H265::Video_Encoder_H265():_x265_encoder(NULL), _x265_nal(NULL),
    _x265_picture(NULL), _x265_param(NULL), _callback(NULL)
{

}

Video_Encoder_H265::~Video_Encoder_H265()
{

}

bool Video_Encoder_H265::operator ()(uint8_t **yuv_data, int *linesize, int top_or_bottom_field)
{
    x265_nal *pp_nal = NULL;
    uint32_t pi_nal = 0;

    for (int i = 0; i < 3; ++i)
    {
        _x265_picture->planes[i] = yuv_data[i];
        _x265_picture->stride[i] = linesize[i];
    }

    x265_picture pic_out;
    if(_insert_idr)
    {
        pic_out.sliceType = X265_TYPE_IDR;
        _insert_idr = false;
    }
    else
    {
        pic_out.sliceType = X265_TYPE_AUTO;
    }


    /*When the last of the raw input pictures has been sent to the encoder,
     * x265_encoder_encode() must still be called repeatedly with a pic_in argument of 0,
     * indicating a pipeline flush, until the function returns a value less than or equal to 0
     * (indicating the output bitstream is complete).*/
    int rc = x265_encoder_encode(_x265_encoder, &pp_nal, &pi_nal, _x265_picture, &pic_out);
    if(rc < 0)
    {
        //error...
        printf("x265_encoder_encode error~\n");
        return false;
    }

        //1 if a picture and access unit were output

    gettimeofday(&_tv_current, NULL);
    struct timeval tv_result;
    timersub(&_tv_current, &_tv_start, &tv_result);
    u_int64_t  ts = tv_result.tv_sec * 1000000 + tv_result.tv_usec;
    ts -= 1000;
    ts = ( ts * 90000 ) / 1000000;

    for(unsigned int i=0; i<pi_nal; ++i)
    {
//        printf("@@@@@@@@@@@@@@@@@@@@@@@@%d%d%d, size=%d\n", i, i, i, pp_nal[i].sizeBytes);
        (*_callback)(pp_nal[i].payload, pp_nal[i].sizeBytes, pp_nal[i].type, ts);
    }

//    x265_stats *stats;
//    uint32_t statsSizeBytes;
    //At any time during this process, the application may query running statistics from the encoder:
//    x265_encoder_get_stats(_x265_encoder, stats, statsSizeBytes);

    return true;
}

bool Video_Encoder_H265::init(Video_Encoder::NALU_CB *cb, const Encoder_Param &param)
{
    //alloc & set param
    _x265_param = x265_param_alloc();
    if(_x265_param == NULL)
    {
        printf("x265_param_alloc error~\n");
        return false;
    }
//    x265_param_default(_x265_param);
    /* x265_preset_names[] = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo", 0 };
     x265_tune_names[] = { "psnr", "ssim", "grain", "zerolatency", "fastdecode", 0 };*/

   int rc = x265_param_default_preset(_x265_param, x265_preset_names[4], x265_tune_names[3]);
   if(rc != 0)
   {
       //error...
       printf("x265_param_default_preset error~\n");
       return false;
   }
   /*x265_profile_names[] = { "main", "main10", "mainstillpicture", 0 };*/

   rc = x265_param_apply_profile(_x265_param, x265_profile_names[0]);
   if(rc != 0)
   {
       //error...
       printf("x265_param_apply_profile error~\n");
       return false;
   }

    _x265_param->sourceHeight = param.height;
    _x265_param->sourceWidth = param.width;

    /*The output of the encoder is a series of NAL packets, which are always returned concatenated in consecutive memory.
     * HEVC streams have SPS and PPS and VPS headers which describe how the following packets are to be decoded.
     * If you specified --repeat-headers then those headers will be output with every keyframe.
     * Otherwise you must explicitly query those headers using:int x265_encoder_headers(x265_encoder *, x265_nal **pp_nal, uint32_t *pi_nal);*/
    _x265_param->bRepeatHeaders = 1;
    _x265_param->internalCsp = X265_CSP_I420;
    _x265_param->fpsNum = param.fps;
    _x265_param->fpsDenom = 1;



//    x265_param_parse(_x265_param, const char *name, const char *value);

    /*******
    x265_encoder_parameters() may be used to get a copy of the param structure from the encoder after it has been opened,
       in order to see the changes made to the parameters for auto-detection and other reasons.
       x265_encoder_reconfig() may be used to reconfigure encoder parameters mid-encode:
       *********/

//    rc = x265_encoder_reconfig(_x265_encoder, _x265_param);

    _x265_picture = x265_picture_alloc();
    if(_x265_picture == NULL)
    {
        if(_x265_param)
        {
            x265_param_free(_x265_param);
        }
        printf("x265_picture_alloc error~\n");
        return false;
    }

    x265_picture_init(_x265_param, _x265_picture);

    //Analysis Buffers
//    rc = x265_alloc_analysis_data(_x265_picture);
//    if(rc == 0)
//    {
//        //error...
//    }
//    x265_free_analysis_data(_x265_picture);


    //
    _x265_encoder = x265_encoder_open(_x265_param);
    if(_x265_encoder == NULL)
    {
        if(_x265_param)
        {
            x265_param_free(_x265_param);
        }

        if(_x265_picture)
        {
            x265_picture_free(_x265_picture);
        }

        printf("x265_encoder_open error~\n");
        return false;
    }

    _callback = cb;

    gettimeofday(&_tv_start, NULL);

    return true;
}

void Video_Encoder_H265::release()
{
    if(_x265_param)
    {
        x265_param_free(_x265_param);
    }

    if(_x265_picture)
    {
        x265_picture_free(_x265_picture);
    }

    if(_x265_encoder)
    {
        x265_encoder_close(_x265_encoder);
    }

    x265_cleanup() ;
}
