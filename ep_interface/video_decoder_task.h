#ifndef VIDEO_DECODER_TASK_H
#define VIDEO_DECODER_TASK_H

#include <ace/Task_Ex_T.h>
#include <ace/Malloc_T.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Dgram.h>
#include <ace/Singleton.h>

#include "video_decoder_factory.h"

#define NALU_MAX_LEN  1920 * 1080 * 3 * 3 / 2

struct Decoder_Nalu_Buf
{
    unsigned char buf[NALU_MAX_LEN];
    int len;
};

class Video_Decoder_Task : public  ACE_Task_Ex<ACE_MT_SYNCH, Decoder_Nalu_Buf>
{
public:
    Video_Decoder_Task(Video_Decoder::YUV_CB *cb, Video_Decoder::Codec_Type codec_type);
    ~Video_Decoder_Task();

    int start();
    void stop();

    int svc();

    bool operator ()(unsigned char*, int);

private:
    Video_Decoder::Codec_Type   _codec_type;
    Video_Decoder::YUV_CB * _yuv_cb;
    Video_Decoder*      _video_decoder;
    ACE_Cached_Allocator<Decoder_Nalu_Buf, ACE_Thread_Mutex>    _allocator;
};

#endif // VIDEO_DECODER_TASK_H
