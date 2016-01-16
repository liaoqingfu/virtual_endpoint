#ifndef YUV_GL_WIDGET_IMPL_H
#define YUV_GL_WIDGET_IMPL_H
//#include <GL/gl.h>
#include <ace/Message_Queue_T.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Thread_Mutex.h>

#include <iostream>

class YUV_GL_Widget;

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

class YUV_GL_Widget_Impl
{
public:
    YUV_GL_Widget_Impl(YUV_GL_Widget* widget);
    virtual ~YUV_GL_Widget_Impl();

    virtual int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );

    void paintGL();
    void draw_GL(YUV_Data* yuv_data);

    bool create_video_program();
    bool create_video_shader();

public:
    GLuint _yuv_shader;
    GLuint _yuv_program;
    GLuint _texture_id[3];          // ID of texture [0]:y [1]:u [2]:v

private:
	YUV_GL_Widget_Impl(const YUV_GL_Widget_Impl&);
	YUV_GL_Widget_Impl& operator=(const YUV_GL_Widget_Impl&);
    ACE_Message_Queue_Ex<YUV_Data, ACE_MT_SYNCH> _yuv_queue;
    ACE_Thread_Mutex _destory_mutex;

    YUV_GL_Widget *_widget;

    int _video_src_w;
    int _video_src_h;

    YUV_Data *_prev_data;

};

#endif // YUV_GL_WIDGET_IMPL_H
