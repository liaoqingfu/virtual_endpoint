#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include "video_decoder_factory.h"
#include "video_decoder_ffmpeg.h"


std::set<Video_Decoder *> Video_Decoder_Factory::_decoders;

Video_Decoder* Video_Decoder_Factory::create(Video_Decoder::Codec_Type codec_type, Video_Decoder::YUV_CB *cb)
{
    Video_Decoder * video_decoder = new Video_Decoder_FFmpeg();
    if( !video_decoder )  return NULL;

    bool f = video_decoder->init(cb, codec_type);
    if(!f)
    {
        fprintf(stderr,"init video decoder error\n");
        delete video_decoder;
        video_decoder = NULL;
    }
    else
    {
        fprintf(stderr,"init video decoder success\n");
    }

    _decoders.insert(video_decoder);

    return video_decoder;
}


void Video_Decoder_Factory::destroy(Video_Decoder* decoder)
{

    for(std::set<Video_Decoder *>::iterator iter = _decoders.begin(); iter!=_decoders.end(); ++iter)
    {
        if(*iter == decoder)
        {
            fprintf(stderr,"release video decoder \n");
            decoder->release();
            delete decoder;
            break;
        }
    }

}
