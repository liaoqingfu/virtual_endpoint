#ifndef VIDEO_ENCODER_H
#define VIDEO_ENCODER_H

#include <functional>

class Encoder_Param;
class Video_Encoder
{
    friend class Video_Encoder_Factory;
public:
    typedef std::function<int(unsigned char*, int len, int type, unsigned int timestamp)> NALU_CB;

    Video_Encoder():_insert_idr(false){}
    virtual ~Video_Encoder() {}

    virtual bool operator () (uint8_t **yuv_data, int *linesize, int top_or_bottom = 0) = 0;
    void insert_idr(){_insert_idr = true;}

protected:
    virtual bool init(NALU_CB *cb,const Encoder_Param &param) = 0;
    virtual void release() = 0;

protected:
    bool _insert_idr;
};

#endif // VIDEO_ENCODER_H
