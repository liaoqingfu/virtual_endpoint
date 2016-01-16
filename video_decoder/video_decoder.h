#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <cstdint>
#include <functional>

class Video_Decoder
{
    friend class Video_Decoder_Factory;
public:
    enum Codec_Type
    {
        UNKNOWN = 0,
        H264,
        H265
    };

    typedef std::function<int(uint8_t **data, int, int, int *linesize)> YUV_CB;

    Video_Decoder(){}
    virtual  ~Video_Decoder(){}

    virtual bool   operator () (unsigned char *nalu, int nalu_len )= 0;

protected:
    virtual bool init(YUV_CB *cb, Codec_Type codec_type = H264) = 0;
    virtual void release() = 0;
};

#endif // VIDEO_DECODER_H
