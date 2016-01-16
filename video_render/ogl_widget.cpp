#include <cassert>
#include <unistd.h>
#include "ogl_widget.h"
#include "ogl_widget_impl.h"

OGL_Widget::OGL_Widget(unsigned long parent_win_id):
    _ogl_widget_impl(NULL)
{
    _ogl_widget_impl = new OGL_Widget_Impl(parent_win_id);
}
OGL_Widget::~OGL_Widget()
{
	 if (NULL != _ogl_widget_impl)
   {
   		delete _ogl_widget_impl;
       _ogl_widget_impl = NULL;
}
}


bool OGL_Widget::init()
{
    return _ogl_widget_impl->init();
}


void OGL_Widget::release()
{
   
   if (NULL != _ogl_widget_impl)
   {
   		_ogl_widget_impl->release();
   }
}

int OGL_Widget::on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize )
{
    int fr = 0;
    if (_ogl_widget_impl)
        fr = _ogl_widget_impl->on_yuv_data_arrive(w, h, yuv, linesize);

    return fr;
}

