#ifndef ENCODER_TYPES
#define ENCODER_TYPES

struct Encoder_Param
{
    enum Encoder_Type
    {
        UNKNOWN = 0,
        H264,
        H265
    };

    Encoder_Param() :width(0)  ,height(0)  ,gop_len(0) ,bitrate(0) ,qp_min(0) ,qp_max(0)  ,qp_step(0) ,fps(0), encoder_type(UNKNOWN){}
    Encoder_Param& operator=(const Encoder_Param& other)
    {
        width = other.width;
        height = other.height;
        bitrate = other.bitrate;
        gop_len = other.gop_len;
        qp_min = other.qp_min;
        qp_max = other.qp_max;
        qp_step = other.qp_step;
        fps = other.fps;
        encoder_type = other.encoder_type;

        return *this;
    }


    unsigned short width;
    unsigned short height;
    unsigned short gop_len;

    int bitrate;
    int qp_min;
    int qp_max;
    int qp_step;
    int fps;
    Encoder_Type encoder_type;
};

#endif // ENCODER_TYPES

