#ifndef OGL_WIDGET_IMPL_H
#define OGL_WIDGET_IMPL_H

#include <GL/glxew.h>

#include <pthread.h>

#include <ace/Message_Queue_T.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Thread_Mutex.h>


namespace OGL_WIDGET_IMPL
{
struct YUV_Data
{
    int width;
    int height;
    char *buffer[3];
    ~YUV_Data()
    {
        for (int i = 0; i < 3; i++)
        {
            if (NULL != buffer[i])
            {
                delete []buffer[i];
                buffer[i] = NULL;
            }
        }
    }
};
}

class OGL_Widget;
class OGL_YUV_Render;
class OGL_Widget_Impl
{
public:
    OGL_Widget_Impl(unsigned long parent_win_id);

    virtual ~OGL_Widget_Impl();
public:
    bool init();
    void release();
    virtual int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );

private:
    bool run_painter();
    static void* painter(void* arg);
    int painter_func();

private:
    int painter_init();
    int paint(OGL_WIDGET_IMPL::YUV_Data* yuv_data);

private:
	OGL_Widget_Impl(const OGL_Widget_Impl&);
	OGL_Widget_Impl& operator=(const OGL_Widget_Impl&);
    unsigned long     _parent_win_id;
    pthread_t         _painter_pid;
    pthread_t         _win_event_pid;
    bool              _is_inited;
    bool              _is_run_painter;
    volatile bool              _is_stoped;

    ACE_Message_Queue_Ex<OGL_WIDGET_IMPL::YUV_Data, ACE_MT_SYNCH> _yuv_queue;
    int _old_width;
    int _old_height;
    Display*            _display;
    unsigned long       _current_win_id;
    GLXContext          _glcontext;

private:
    ACE_Thread_Mutex    _mutex;
    OGL_YUV_Render*     _render;
    XWindowChanges*     _win_change;
    XWindowAttributes*  _win_attr;

};

#endif // OGL_WIDGET_IMPL_H
