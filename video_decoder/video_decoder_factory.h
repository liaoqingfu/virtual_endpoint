#ifndef VIDEO_DECODER_FACTORY
#define VIDEO_DECODER_FACTORY

#include<set>

#include "video_decoder.h"

class Video_Decoder_Factory
{
public:
        static Video_Decoder* create(Video_Decoder::Codec_Type codec_type, Video_Decoder::YUV_CB *cb);
        static void destroy(Video_Decoder* video_decoder);

private:
       static  std::set<Video_Decoder *>_decoders;
};

#endif
