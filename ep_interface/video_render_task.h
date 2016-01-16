#ifndef VIDEO_RENDER_TASK_H
#define VIDEO_RENDER_TASK_H


#include <ace/Task_Ex_T.h>
#include <ace/Malloc_T.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Dgram.h>
#include <ace/Singleton.h>

struct YUV
{
    unsigned char yuv_buf_0[1920 * 1080 * 3];
    unsigned char yuv_buf_1[1920 * 1080 * 3 / 4];
    unsigned char yuv_buf_2[1920 * 1080 * 3 / 4];
    int     width;
    int     height;
    int     linesize[3];
};

class QWidget;
class YUV_Widget;
class YUV_GL_Widget;
class Video_Render;
class Video_Render_Task : public  ACE_Task_Ex<ACE_MT_SYNCH, YUV>
{
public:
    Video_Render_Task(QWidget *widget);
    ~Video_Render_Task();

    int start();
    void stop();

    int svc();

    bool operator ()(unsigned char**, int w, int h, int* linesize);

private:
    YUV_Widget*         _yuv_widget;
    YUV_GL_Widget*      _yuv_gl_widget;
    Video_Render*       _video_render;
    ACE_Cached_Allocator<YUV, ACE_Thread_Mutex>    _allocator;
};

#endif // VIDEO_RENDER_TASK_H
