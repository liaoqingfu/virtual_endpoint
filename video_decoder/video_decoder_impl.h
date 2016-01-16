#ifndef VIDEO_DECODER_IMPL_H
#define VIDEO_DECODER_IMPL_H

#include <cstdint>
#include <stddef.h>
using namespace std;

#include "video_decoder.h"

class Video_Decoder_Factroy;

class Video_Decoder_Impl : public Video_Decoder
{
    public:
        Video_Decoder_Impl();
        ~Video_Decoder_Impl();

        bool   operator () (unsigned char *nalu, int nalu_len);

        bool   init(YUV_CB *cb);

        int   release();

        bool (*init_func)(void* impl, YUV_CB*);
        int (*release_func)(void* impl);
        bool (*decode_func)(void *impl, unsigned char *nalu, int nalu_len) ;
        void *impl;
        void *handle;

    protected:
        friend class Video_Decoder_Factroy;
};

#endif // VIDEO_DECODER_H
