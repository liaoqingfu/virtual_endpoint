
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include "video_encoder_factory.h"
#include "video_encoder_h264.h"
#include "video_encoder_h265.h"

std::set<Video_Encoder *> Video_Encoder_Factory::_encoders;



Video_Encoder* Video_Encoder_Factory::create(const Encoder_Param& encode_param, Video_Encoder::NALU_CB *cb)
{
    Video_Encoder * video_encoder  = NULL;

    if(Encoder_Param::H264 == encode_param.encoder_type )
    {
        video_encoder = new Video_Encoder_H264();
    }
    else if( Encoder_Param::H265 == encode_param.encoder_type)
    {
        video_encoder = new Video_Encoder_H265();
    }

    if( !video_encoder )  return NULL;

    bool f = video_encoder->init(cb,encode_param);
    if(!f)
    {
        fprintf(stderr,"init video encoder error\n");
        delete video_encoder;
        video_encoder = NULL;
        return NULL;
    }
    else
    {
        fprintf(stderr,"init video encoder success\n");
    }

    _encoders.insert( video_encoder );

    return video_encoder;
}


void Video_Encoder_Factory::destroy(Video_Encoder* encoder)
{
    fprintf(stderr,"release video encoder \n");

    auto iter =_encoders.find(encoder);
    if(iter != _encoders.end() )
    {
        encoder->release();
        delete encoder;
        _encoders.erase(iter);
    }
}

