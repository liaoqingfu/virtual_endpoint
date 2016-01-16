#include "video_render/ogl_widget.h"
#include <assert.h>

#include "video_render/yuv_widget.h"
#include "video_render/yuv_gl_widget.h"
#include "video_render.h"
Video_Render::Video_Render(YUV_Widget* widget)
    : _yuv_widget(widget)
    , _yuv_gl_widget( NULL )
    , _ogl_widget(NULL)
{
    assert( widget);
}

Video_Render::Video_Render(YUV_GL_Widget* widget)
    : _yuv_widget( NULL )
    , _yuv_gl_widget( widget )
    , _ogl_widget(NULL)
{
    assert( widget);
}

Video_Render::Video_Render(OGL_Widget* ogl_widget)
    : _yuv_widget( NULL )
    , _yuv_gl_widget( NULL )
    ,_ogl_widget(ogl_widget)
{
    assert( ogl_widget);
}

Video_Render::~Video_Render()
{
}

int Video_Render::on_yuv_data_arrive(int w, int h, char **yuv, int* linesize)
{
    if (_yuv_widget != NULL)
    {
        _yuv_widget->on_yuv_data_arrive(w, h, yuv, linesize);
    }
    else if (_yuv_gl_widget != NULL)
    {
        _yuv_gl_widget->on_yuv_data_arrive(w, h, yuv, linesize);
    }
    else if (_ogl_widget != NULL)
    {
        _ogl_widget->on_yuv_data_arrive(w, h, yuv, linesize);
    }
    return 0;
}

YUV_Widget *Video_Render::get_yuv_widget()
{
    return _yuv_widget;
}

YUV_GL_Widget *Video_Render::get_yuv_gl_widget()
{
    return _yuv_gl_widget;
}

OGL_Widget *Video_Render::get_ogl_widget()
{
    return _ogl_widget;
}

