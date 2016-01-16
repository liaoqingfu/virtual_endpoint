
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>



#include "video_encoder.h"
#include "video_encoder_h264.h"
#include "encoder_types.h"

#define RTP_BASE_TIME 90000

Video_Encoder_H264::Video_Encoder_H264() :
    _x264_h(NULL),
    _nalu_buf(NULL),
    _nalu_buf_size(0),
    _nalu_cb(NULL),
    _i_frame(0),
    _sample_rate(0),
    _img_plane(NULL)
{
}

Video_Encoder_H264::~Video_Encoder_H264()
{

}

bool Video_Encoder_H264::operator () (uint8_t **yuv_data, int *linesize, int )
{
    x264_picture_t pic_out;
    int i_nal = 0;
    x264_nal_t *nal = NULL;

    _x264_pic.i_pts = ( int64_t ) _i_frame * _x264_param.i_fps_den;
#if 1
    /* Do not force any parameters */
    _x264_pic.i_type = X264_TYPE_AUTO;
    _x264_pic.i_qpplus1 = 0;

    //x264_h->param.i_slice_max_size = 1000;
    //x264_h->param.i_slice_max_mbs = 748;
    //        x264_h->i_idr_pic_id = 5;
    //        x264_h->param.i_frame_reference = 2;
    //        x264_h->param.i_frame_reference = 1;
    //        x264_h->param.i_frame_reference = X264_Lib_V3::x264_clip3( x264_h->param.i_frame_reference, 1, 16 );
    //        x264_h->param.analyse.b_mixed_references = x264_h->param.analyse.b_mixed_references && x264_h->param.i_frame_reference > 1;
    //        x264_h->frames.i_max_ref0 = x264_h->param.i_frame_reference;
    //        x264_h->frames.i_max_dpb  = 2;
#endif


//    uint8_t *src    = NULL;
//    uint8_t *dst    = NULL;
//    //Y
//    src    = yuv_data[0];
//    dst    = _x264_pic.img.plane[0];

//    for(int i = 0; i < _x264_param.i_height; ++i)
//    {
//        memcpy(dst, src, linesize[0]);
//        src += linesize[0];
//        dst += _x264_pic.img.i_stride[0];
//    }
//    //U
//    src    = yuv_data[1];
//    dst    = _x264_pic.img.plane[1];
//    for(int i = 0; i < _x264_param.i_height / 2; ++i)
//    {
//        memcpy(dst, src, linesize[1]);
//        src += linesize[1];
//        dst += _x264_pic.img.i_stride[1];
//    }
//    //V
//    src    = yuv_data[2];
//    dst    = _x264_pic.img.plane[2];
//    for(int i = 0; i < _x264_param.i_height / 2; ++i)
//    {
//        memcpy(dst, src, linesize[2]);
//        src += linesize[2];
//        dst += _x264_pic.img.i_stride[2];
//    }

    for(int i = 0; i < 3; ++i)
    {
        _x264_pic.img.plane[i] = yuv_data[i];
        _x264_pic.img.i_stride[i] = linesize[i];
    }

#if 0//WRITE_TOP
        static int xxfd = 0;
        if ( xxfd == 0 )
            xxfd = open ( "./top.yuv", O_CREAT | O_TRUNC | O_RDWR );
        write ( xxfd, _x264_pic.img.plane[0], _x264_pic.img.i_stride[0]*_encode_param.height );
        write ( xxfd, _x264_pic.img.plane[1], _x264_pic.img.i_stride[1]*_encode_param.height / 2 );
        write ( xxfd, _x264_pic.img.plane[2], _x264_pic.img.i_stride[2]*_encode_param.height / 2 );
#endif

    if(_insert_idr)
    {
        _x264_pic.i_type = X264_TYPE_IDR;
        _insert_idr = false;
    }
    else
    {
        _x264_pic.i_type = X264_TYPE_AUTO;
    }

    int frame_size = x264_encoder_encode ( _x264_h, &nal, &i_nal, &_x264_pic, &pic_out );

    if (  frame_size <= 0 )
    {
//        static int xxfd = 0;
//        if ( xxfd == 0 )
//            xxfd = open ( "/tmp/aaa.yuv", O_CREAT | O_TRUNC | O_RDWR , 777);
//        write ( xxfd, _x264_pic.img.plane[0], _x264_pic.img.i_stride[0]*_encode_param.height );
//        write ( xxfd, _x264_pic.img.plane[1], _x264_pic.img.i_stride[1]*_encode_param.height / 2 );
//        write ( xxfd, _x264_pic.img.plane[2], _x264_pic.img.i_stride[2]*_encode_param.height / 2 );

        fprintf( stderr,"%s : x264 encode failed\n", "operator()");
        return false;
    }

    int lens = 0;
    for (int i = 0; i < i_nal; i++)
    {
        lens += nal[i].i_payload;
    }

#if 0
    int is_iframe = 0;
    if(pic_out.i_type == X264_TYPE_IDR || pic_out.i_type==X264_TYPE_I
            ||pic_out.i_type == X264_TYPE_KEYFRAME)
    {
        //fprintf(stderr,"x264_encoder_encode type:%d\n",pic_out.i_type);
        is_iframe = 1;
    }  
#endif

    if(_nalu_buf_size < lens)
    {
        if(_nalu_buf != NULL)
        {
            delete [] _nalu_buf;
            _nalu_buf = NULL;
        }

        _nalu_buf_size = lens < (1920 * 1088 * 2) ? (1920 * 1088 * 2) : (lens * 2);

        _nalu_buf = new unsigned char [_nalu_buf_size + 256];
    }

    {
        gettimeofday(&_tv_current, NULL);
        struct timeval tv_result;
        timersub(&_tv_current, &_tv_start, &tv_result);

        u_int64_t  tt = tv_result.tv_sec * 1000000 + tv_result.tv_usec;

        tt -= 1000;
        tt = ( tt * _sample_rate ) / 1000000;

        for (int i = 0; i < i_nal; i++)
        {
            (*_nalu_cb)(nal[i].p_payload, nal[i].i_payload, pic_out.i_type, tt);
        }
    }

    _i_frame++;

    return true;
}

bool Video_Encoder_H264::init(Video_Encoder::NALU_CB *cb, const Encoder_Param &param)
{
    _nalu_cb = cb;
    _sample_rate = RTP_BASE_TIME;
    gettimeofday(&_tv_start, NULL);

   return codec_init(param);
}

void Video_Encoder_H264::release()
{
    codec_release();
    if(NULL != _nalu_buf)
        delete [] _nalu_buf;
}

bool Video_Encoder_H264::codec_init(const Encoder_Param& param)
{
    int rc = x264_param_default_preset(&_x264_param, "fast", "zerolatency");//fast  zerolatency "ultrafast", "zerolatency");//
    if(rc != 0)
    {
        fprintf(stderr,"_x264_param_default_preset error!");
        return false;
    }

    rc = x264_param_apply_profile(&_x264_param, x264_profile_names[0]);
    if(rc != 0)
    {
        fprintf(stderr,"_x264_param_apply_profile error!");
        return false;
    }

    set_encoder_param(param);


    if ( ( _x264_h = x264_encoder_open( &_x264_param ) ) == NULL )
    {
        fprintf(stderr,"%s : 264_encoder_open failed\n", "x264_encoder_open");
        return false;
    }


    //???????????????
    x264_picture_alloc ( &_x264_pic, X264_CSP_I420, _x264_param.i_width, _x264_param.i_height );
    _img_plane = _x264_pic.img.plane[0];
    //fprintf(stderr,"_x264_pic.img.i_stride[0] =%d _x264_pic.img.i_stride[1]=%d %d \n", _x264_pic.img.i_stride[0], _x264_pic.img.i_stride[1],_x264_pic.img.i_stride[2]);

    //会议通知模式，背景颜色采用蓝底
    if ( 1 )
    {
        memset ( _x264_pic.img.plane[0], 0xFF, _x264_param.i_width * _x264_param.i_height );
        memset ( _x264_pic.img.plane[1], 0x80, _x264_param.i_width * _x264_param.i_height / 4 );
        memset ( _x264_pic.img.plane[2], 0x80, _x264_param.i_width * _x264_param.i_height / 4 );

    }
    else   //其他模式，背景颜色采用黑底
    {
        memset ( _x264_pic.img.plane[0], 0x00, _x264_param.i_width * _x264_param.i_height );
        memset ( _x264_pic.img.plane[1], 0x80, _x264_param.i_width * _x264_param.i_height / 4 );
        memset ( _x264_pic.img.plane[2], 0x80, _x264_param.i_width * _x264_param.i_height / 4 );
    }

    dump_config();
    return true;
}
void Video_Encoder_H264::codec_release()
{
    _x264_pic.img.plane[0] = _img_plane;
    x264_picture_clean(&_x264_pic);
    x264_encoder_close(_x264_h);
}

void Video_Encoder_H264::dump_config()
{
    x264_encoder_parameters(_x264_h, &_x264_param);
    fprintf(stderr,"method:%d,w = %d, h = %d, qp = %d-%d-%d,gop = %d, fps:%d,bitrate = %d vbv_buffer_size= %d vbv_max= %d\n",
            _x264_param.rc.i_rc_method,_x264_param.i_width, _x264_param.i_height,
            _x264_param.rc.i_qp_min, _x264_param.rc.i_qp_max, _x264_param.rc.i_qp_step,
            _x264_param.i_keyint_max, _x264_param.i_fps_num,_x264_param.rc.i_bitrate,
            _x264_param.rc.i_vbv_buffer_size, _x264_param.rc.i_vbv_max_bitrate );
    fprintf(stderr,"f_rf_constant=%f, f_rate_toleranc=%f\n",_x264_param.rc.f_rf_constant, _x264_param.rc.f_rate_tolerance);

    fprintf(stderr, "i_me_method= %d, i_subpel_refine= %d\n", _x264_param.analyse.i_me_method, _x264_param.analyse.i_subpel_refine);

}
int Video_Encoder_H264::set_encoder_param(const Encoder_Param &encode_param)
{
    _x264_param.i_width  = encode_param.width;
    _x264_param.i_height = encode_param.height;

    _x264_param.rc.i_bitrate = encode_param.bitrate - encode_param.bitrate/10;                   /*6144设置平均码率 */
    _x264_param.rc.i_vbv_buffer_size = encode_param.bitrate/ encode_param.fps*5 ;
    _x264_param.rc.i_vbv_max_bitrate = encode_param.bitrate - encode_param.bitrate / 10;

    _x264_param.rc.i_qp_step = encode_param.qp_step;
    _x264_param.rc.i_qp_min = encode_param.qp_min;
    _x264_param.rc.i_qp_max = encode_param.qp_max;

    _x264_param.i_keyint_max = encode_param.gop_len;
    _x264_param.i_keyint_min = encode_param.gop_len;
    _x264_param.i_fps_num = encode_param.fps;
    _x264_param.i_fps_den = 1;                         /* ==1 better */

    _x264_param.i_level_idc=30;
    _x264_param.rc.i_rc_method = X264_RC_ABR;      /* 码率控制参数X264_RC_ABR,X264_RC_CQP*/
    _x264_param.b_repeat_headers = 1;                 /* 重复SPS/PPS 放到关键帧前面 */
    _x264_param.b_cabac = 1;                          /*熵编码 ==1 better*/
    _x264_param.i_threads = 4;                        /*= 4 并行编码多帧 */
    _x264_param.b_sliced_threads = 0;                 /*0*/
    _x264_param.b_vfr_input = 0;                       /*???== 0*/

    _x264_param.b_interlaced = 0;                      /* 隔行扫描 */

    _x264_param.i_frame_reference = 1;                 /* 参考帧最大数目 ==1*/

    _x264_param.i_nal_hrd = 0;                         /*sei*/
    //_x264_param.b_annexb  = 1;
    //_x264_param.analyse.b_transform_8x8 = 1;
    //_x264_param.analyse.intra = X264_ANALYSE_I4x4 | X264_ANALYSE_PSUB8x8;
    //_x264_param.analyse.intra = X264_ANALYSE_I8x8 | X264_ANALYSE_I4x4 | X264_ANALYSE_PSUB8x8 | X264_ANALYSE_PSUB16x16;
    //_x264_param.analyse.inter = X264_ANALYSE_I8x8 | X264_ANALYSE_I4x4 | X264_ANALYSE_PSUB8x8 | X264_ANALYSE_PSUB16x16;
    //_x264_param.analyse.intra = X264_ANALYSE_I8x8 | X264_ANALYSE_I4x4 | X264_ANALYSE_PSUB8x8 | X264_ANALYSE_PSUB16x16;
    //_x264_param.analyse.inter = X264_ANALYSE_I8x8 | X264_ANALYSE_I4x4 | X264_ANALYSE_PSUB8x8 | X264_ANALYSE_PSUB16x16;



    //_x264_param.analyse.intra = X264_ANALYSE_I8x8 | X264_ANALYSE_PSUB16x16;
    //_x264_param.analyse.inter = X264_ANALYSE_I8x8 | X264_ANALYSE_PSUB16x16;
    _x264_param.analyse.intra = X264_ANALYSE_PSUB16x16;
    _x264_param.analyse.inter = X264_ANALYSE_PSUB16x16;
    _x264_param.analyse.i_me_method = 4;
    _x264_param.i_log_level = X264_LOG_WARNING;//X264_LOG_NONE;
    //_x264_param.i_scenecut_threshold = 40;             /*如何积极地插入额外的I帧 */

    if(_x264_h)
    {
        x264_encoder_reconfig(_x264_h,&_x264_param);
        dump_config();
    }

    return 0;
}

