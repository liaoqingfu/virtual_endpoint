#include "video_render.h"
#include "video_render_task.h"
#include "yuv_widget.h"
#include "yuv_gl_widget.h"


Video_Render_Task::Video_Render_Task(QWidget *widget)
    :_yuv_widget(new YUV_Widget(widget))
    ,_yuv_gl_widget(new YUV_GL_Widget(widget) )
    ,_video_render(new Video_Render(_yuv_gl_widget))
    ,_allocator(8)
{

}

Video_Render_Task::~Video_Render_Task()
{
    delete _video_render;
}

int Video_Render_Task::start()
{
//    _yuv_widget->show();
    _yuv_gl_widget->show();
    if( !_video_render )
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:new Video_Render fail!\n"), __PRETTY_FUNCTION__));
        return -1;
    }

    msg_queue()->activate();
    activate();

    return 0;
}


void Video_Render_Task::stop()
{
//    _yuv_widget->hide();
    _yuv_gl_widget->hide();
    putq(NULL);
    msg_queue()->deactivate();
    wait();
}

int Video_Render_Task::svc()
{
    YUV* yuv = NULL;
    int rc = -1;

    unsigned char* yuv_buf[3];

    while(true)
    {
        rc = getq(yuv);
        if(-1 == rc)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:getq (deactived) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        if( !yuv )
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:active close msg(yuv == NULL) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        yuv_buf[0] = yuv->yuv_buf_0;
        yuv_buf[1] = yuv->yuv_buf_1;
        yuv_buf[2] = yuv->yuv_buf_2;
        _video_render->on_yuv_data_arrive(yuv->width, yuv->height, (char**)yuv_buf, yuv->linesize);
        _allocator.free(yuv);
    }

    ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: thread exit!!!!!!!!!!!!!!!!!!!!\n"), __PRETTY_FUNCTION__) );

    return 0;
}

bool Video_Render_Task::operator ()(unsigned char** yuv_buf, int w, int h, int *linesize)
{
    if(!yuv_buf || !linesize)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:yuv_buf or linesize is NULL!\n"), __PRETTY_FUNCTION__));
        return false;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:linesize[0] = %d, w = %d!\n"), __PRETTY_FUNCTION__, linesize[0], w));

    YUV* yuv = (YUV*)_allocator.malloc();
    if(yuv_buf == NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:_allocator malloc error!\n"), __PRETTY_FUNCTION__));
        return false;
    }

    yuv->width = w;
    yuv->height = h;

    yuv->linesize[0] = linesize[0];
    yuv->linesize[1] = linesize[1];
    yuv->linesize[2] = linesize[2];

    ACE_OS::memcpy(yuv->yuv_buf_0, yuv_buf[0], h * linesize[0] );
    ACE_OS::memcpy(yuv->yuv_buf_1, yuv_buf[1], h * linesize[1] / 2);
    ACE_OS::memcpy(yuv->yuv_buf_2, yuv_buf[2], h * linesize[2] / 2);

    putq(yuv);

    return true;
}

