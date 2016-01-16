#ifndef X264_LIB_V4_H
#define X264_LIB_V4_H
#include<cstdint>
extern "C"
{
#include "x264.h"
}

class  X264_Lib_V4
{
public:
    typedef int (*x264_encoder_encode_func)( x264_t *, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_in, x264_picture_t *pic_out );
    typedef int (*x264_clip3_func)( int v, int i_min, int i_max);
    typedef int64_t (*x264_mdate_func)(void);
    typedef int (*x264_param_default_preset_func)(x264_param_t *, const char *preset, const char *tune);
    typedef int (*x264_param_apply_profile_func)(x264_param_t *, const char *profile);
    typedef x264_t* (*x264_encoder_open_func)(x264_param_t *);
    typedef int (*x264_picture_alloc_func)(x264_picture_t *pic, int i_csp, int i_width, int i_height);
    typedef void (*x264_encoder_parameters_func)(x264_t *, x264_param_t *);
    typedef void (*x264_free_func)( void * );
    typedef void (*x264_picture_clean_func)(x264_picture_t *);
    typedef void (*x264_encoder_close_func)(x264_t*);
    typedef int   (*x264_encoder_reconfig_func)( x264_t *, x264_param_t * );
    static x264_encoder_encode_func x264_encoder_encode;
   // static x264_clip3_func x264_clip3;
    //static x264_mdate_func x264_mdate;
    //static x264_free_func x264_free;
    static x264_param_default_preset_func x264_param_default_preset;
    static x264_param_apply_profile_func x264_param_apply_profile;
    static x264_encoder_open_func x264_encoder_open;
    static x264_picture_alloc_func x264_picture_alloc;
    static x264_encoder_parameters_func x264_encoder_parameters;


    static x264_picture_clean_func x264_picture_clean;
    static x264_encoder_close_func x264_encoder_close;
    static x264_encoder_reconfig_func x264_encoder_reconfig;
    static bool init(char const *dll_name);
};

#endif // X264_LIB_V4_H
