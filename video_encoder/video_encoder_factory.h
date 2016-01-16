#ifndef VIDEO_ENCODER_FACTORY_H
#define VIDEO_ENCODER_FACTORY_H

#include <set>

#include "video_encoder.h"
#include "encoder_types.h"

class Video_Encoder_Factory
{
public:
    static Video_Encoder* create(const Encoder_Param& encode_param, Video_Encoder::NALU_CB *cb);

    static void destroy(Video_Encoder* encoder);

private:
   static  std::set<Video_Encoder *>_encoders;
};

#endif // VIDEO_ENCODER_FACTORY_H
