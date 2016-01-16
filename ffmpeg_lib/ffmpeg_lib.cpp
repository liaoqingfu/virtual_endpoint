//#ifdef USE_DLOPEN
#include <dlfcn.h>
//#endif

#include <cstdio>
#include<assert.h>
using namespace std;

#include "ffmpeg_lib.h"

void* ffmpeg_lib::avcodec_handle = NULL;
void* ffmpeg_lib::avutil_handle = NULL;
void* ffmpeg_lib::swscale_handle = NULL;

ffmpeg_lib::avcodec_register_func ffmpeg_lib::avcodec_register = NULL;

ffmpeg_lib::avcodec_decode_video2_func ffmpeg_lib::avcodec_decode_video2 = NULL;
ffmpeg_lib::avcodec_encode_video_func ffmpeg_lib::avcodec_encode_video= NULL;
//ffmpeg_lib::avpicture_fill_func ffmpeg_lib::avpicture_fill= NULL;
ffmpeg_lib::sws_scale_func ffmpeg_lib::sws_scale= NULL;
ffmpeg_lib::avpicture_layout_func ffmpeg_lib::avpicture_layout= NULL;
ffmpeg_lib::avpicture_get_size_func  ffmpeg_lib::avpicture_get_size= NULL;
ffmpeg_lib::av_init_packet_func ffmpeg_lib::av_init_packet= NULL;

//static avcodec_init_func avcodec_init;
ffmpeg_lib::avcodec_register_all_func ffmpeg_lib::avcodec_register_all= NULL;
ffmpeg_lib::avcodec_close_func ffmpeg_lib::avcodec_close= NULL;
ffmpeg_lib::avcodec_find_decoder_func ffmpeg_lib::avcodec_find_decoder= NULL;
ffmpeg_lib::avcodec_find_encoder_func ffmpeg_lib::avcodec_find_encoder= NULL;

//ffmpeg_lib::avcodec_alloc_context_func ffmpeg_lib::avcodec_alloc_context= NULL;
ffmpeg_lib::avcodec_alloc_context3_func ffmpeg_lib::avcodec_alloc_context3 = NULL;
ffmpeg_lib::avcodec_alloc_frame_func ffmpeg_lib::avcodec_alloc_frame= NULL;


//ffmpeg_lib::avcodec_open_func ffmpeg_lib::avcodec_open= NULL;
ffmpeg_lib::avcodec_open2_func ffmpeg_lib::avcodec_open2 = NULL;

ffmpeg_lib::av_free_func ffmpeg_lib::av_free= NULL;
//static avpicture_alloc_func avpicture_alloc;
ffmpeg_lib::avpicture_free_func ffmpeg_lib::avpicture_free= NULL;

ffmpeg_lib::sws_getContext_func ffmpeg_lib::sws_getContext= NULL;
ffmpeg_lib::sws_freeContext_func ffmpeg_lib::sws_freeContext= NULL;

ffmpeg_lib::avcodec_set_dimensions_func ffmpeg_lib::avcodec_set_dimensions= NULL;

bool ffmpeg_lib::_is_init = false;

bool ffmpeg_lib::init()
{
    if(ffmpeg_lib::_is_init) return true;

#if 0
    //avcodec_init = ::avcodec_init;
    ffmpeg_lib::avcodec_register = ::avcodec_register;
    ffmpeg_lib::avcodec_register_all = ::avcodec_register_all;
    ffmpeg_lib::avcodec_close = ::avcodec_close;
    ffmpeg_lib::avcodec_find_decoder = ::avcodec_find_decoder;
    ffmpeg_lib::avcodec_find_encoder = ::avcodec_find_encoder;

    //ffmpeg_lib::avcodec_alloc_context = ::avcodec_alloc_context;
    ffmpeg_lib::avcodec_alloc_context3 = ::avcodec_alloc_context3;
    ffmpeg_lib::avcodec_alloc_frame = ::avcodec_alloc_frame;

    //ffmpeg_lib::avcodec_open = ::avcodec_open;
    ffmpeg_lib::avcodec_open2 = ::avcodec_open2;
    ffmpeg_lib::avcodec_decode_video2 = ::avcodec_decode_video2;
    ffmpeg_lib::avcodec_encode_video = ::avcodec_encode_video;
    ffmpeg_lib::av_free = ::av_free;
    //avpicture_alloc = ::avpicture_alloc;
    ffmpeg_lib::avpicture_free = ::avpicture_free;
    ffmpeg_lib::av_init_packet = ::av_init_packet;
    //ffmpeg_lib::avpicture_fill = ::avpicture_fill;

    ffmpeg_lib::sws_scale  = ::sws_scale;
    ffmpeg_lib::sws_getContext = ::sws_getContext;
    ffmpeg_lib::sws_freeContext  = ::sws_freeContext;

    ffmpeg_lib::avcodec_set_dimensions = ::avcodec_set_dimensions;
    printf("ffmpeg_lib::init() true!\n");

#else
    char *error = NULL;

#define LOAD_AVCODEC(FUNC) ffmpeg_lib::FUNC = ( ffmpeg_lib::FUNC##_func ) dlsym ( avcodec_handle, #FUNC )
#define LOAD_AVUTIL(FUNC) ffmpeg_lib::FUNC = ( ffmpeg_lib::FUNC##_func ) dlsym ( avutil_handle, #FUNC )
#define LOAD_SWSCALE(FUNC) ffmpeg_lib::FUNC = ( ffmpeg_lib::FUNC##_func ) dlsym ( swscale_handle, #FUNC )

    avcodec_handle = dlopen ( "libavcodec.so", RTLD_LAZY );

    if ( avcodec_handle == NULL )
    {
        char *error = dlerror();
        fprintf ( stderr, "%s/n", error );
        return false;
    }

    avutil_handle = dlopen ( "libavutil.so", RTLD_LAZY );

    if ( avutil_handle == NULL )
    {
        char *error = dlerror();
        fprintf ( stderr, "%s/n", error );
        return false;
    }

    swscale_handle = dlopen ( "libswscale.so", RTLD_LAZY );

    if ( swscale_handle == NULL )
    {
        char *error = dlerror();
        fprintf ( stderr, "%s/n", error );
        return false;
    }

    LOAD_AVCODEC(avcodec_register);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_register_all);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_close);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_find_decoder);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_find_encoder);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_alloc_context3);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_alloc_frame);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_open2);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_decode_video2);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_encode_video);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avcodec_set_dimensions);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (av_init_packet);
    if ( ( error = dlerror() ) == NULL ) LOAD_AVCODEC (avpicture_free);

    if ( ( error = dlerror() ) == NULL ) LOAD_AVUTIL (av_free);

    if ( ( error = dlerror() ) == NULL ) LOAD_SWSCALE (sws_scale);
    if ( ( error = dlerror() ) == NULL ) LOAD_SWSCALE (sws_getContext);
    if ( ( error = dlerror() ) == NULL ) LOAD_SWSCALE (sws_freeContext);


    if ( ( error = dlerror() ) != NULL )
    {
        fprintf ( stderr, "%s\n", error );
        return false;
    }
#endif

    ffmpeg_lib::_is_init = true;
    return true;
}

void ffmpeg_lib::release()
{
    ffmpeg_lib::_is_init = false;

    if(avcodec_handle != NULL)
    {
        dlclose(avcodec_handle);
        avcodec_handle = NULL;
    }

    if(avutil_handle != NULL)
    {
        dlclose(avutil_handle);
        avutil_handle = NULL;
    }

    if(swscale_handle != NULL)
    {
        dlclose(swscale_handle);
        swscale_handle = NULL;
    }
}
