#include<cstdint>
#include <cstdio>
extern "C"
{
#include "x264.h"
}
#include "x264_lib_v4.h"

using namespace std;


X264_Lib_V4::x264_encoder_encode_func       X264_Lib_V4::x264_encoder_encode = NULL;
//X264_Lib_V4::x264_clip3_func                X264_Lib_V4::x264_clip3 = NULL;
//X264_Lib_V4::x264_mdate_func                X264_Lib_V4::x264_mdate = NULL;
//X264_Lib_V4::x264_free_func                 X264_Lib_V4::x264_free = NULL;
X264_Lib_V4::x264_param_default_preset_func X264_Lib_V4::x264_param_default_preset = NULL;
X264_Lib_V4::x264_param_apply_profile_func  X264_Lib_V4::x264_param_apply_profile = NULL;
X264_Lib_V4::x264_encoder_open_func         X264_Lib_V4::x264_encoder_open = NULL;          //attention
X264_Lib_V4::x264_picture_alloc_func        X264_Lib_V4::x264_picture_alloc = NULL;
X264_Lib_V4::x264_encoder_parameters_func   X264_Lib_V4::x264_encoder_parameters = NULL;

X264_Lib_V4::x264_picture_clean_func                 X264_Lib_V4::x264_picture_clean = NULL;
X264_Lib_V4::x264_encoder_close_func                 X264_Lib_V4::x264_encoder_close = NULL;
X264_Lib_V4::x264_encoder_reconfig_func X264_Lib_V4::x264_encoder_reconfig =NULL;

bool X264_Lib_V4::init(char const *dll_name)
{
    X264_Lib_V4::x264_encoder_encode       = ::x264_encoder_encode;
   // X264_Lib_V4::x264_clip3                = ::x264_clip3;
    //X264_Lib_V4::x264_mdate                = ::x264_mdate;
    X264_Lib_V4::x264_param_default_preset = ::x264_param_default_preset;
    X264_Lib_V4::x264_param_apply_profile  = ::x264_param_apply_profile;
    X264_Lib_V4::x264_encoder_open         = ::x264_encoder_open;          //attention
    X264_Lib_V4::x264_picture_alloc        = ::x264_picture_alloc;
    X264_Lib_V4::x264_encoder_parameters   = ::x264_encoder_parameters;
   // X264_Lib_V4::x264_free                 = ::x264_free;
    X264_Lib_V4::x264_picture_clean = ::x264_picture_clean;
    X264_Lib_V4::x264_encoder_close = ::x264_encoder_close;
    X264_Lib_V4::x264_encoder_reconfig = ::x264_encoder_reconfig;
    return true;
}
