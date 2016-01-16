#ifndef OGL_WIDGET_H
#define OGL_WIDGET_H

#include <pthread.h>
#include "yuv_event_listener.h"

class OGL_Widget_Impl;
class OGL_Widget : public Yuv_Data_Listener
{
public:
    explicit OGL_Widget(unsigned long parent_win_id);
    bool init();
    void release();

    ~OGL_Widget();

    virtual int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );

private:
	OGL_Widget(const OGL_Widget&);
	OGL_Widget& operator=(const OGL_Widget&);
    OGL_Widget_Impl* _ogl_widget_impl;
};

#endif // OGL_WIDGET_H
