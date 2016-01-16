#ifndef YUV_EVENT_LISTENER_H
#define YUV_EVENT_LISTENER_H

struct Yuv_Data_Listener
{
    virtual int on_yuv_data_arrive(int w, int h, char **yuv, int* linesize) = 0;
};

#endif // YUV_EVENT_LISTENER_H
