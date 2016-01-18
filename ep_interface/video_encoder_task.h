#ifndef VIDEO_ENCODER_TASK_H
#define VIDEO_ENCODER_TASK_H

#include <ace/Task_Ex_T.h>
#include <ace/Malloc_T.h>

#include "video_encoder_factory.h"

struct Video_Encoder_Buf
{
    char    buf_0[ 1920*1080*3 ];
    char    buf_1[ 1920*1080*3/4 ];
    char    buf_2[ 1920*1080*3/4 ];
    int     w;
    int     h;
    int linesize[3];
};

class Video_Encoder_Task : public ACE_Task_Ex<ACE_MT_SYNCH, Video_Encoder_Buf>
{
public:
    Video_Encoder_Task(const Encoder_Param& encode_param, Video_Encoder::NALU_CB *cb);
    ~Video_Encoder_Task();

    int start();
    void stop();
    int svc(void);

    int operator ()(int w, int h, char**, int*);

private:
    Video_Encoder*      _video_encoder;
    ACE_Cached_Allocator<Video_Encoder_Buf, ACE_Thread_Mutex>    _allocator;
};

#endif // VIDEO_ENCODER_TASK_H
