#ifndef YUV_WIDGET_IMPL_H
#define YUV_WIDGET_IMPL_H

#include <list>

#include <ace/Message_Queue.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Thread_Mutex.h>

#include "my_msg_block_pool.h"

class YUV2RGB;



class YUV_Widget;
class YUV_Widget_Impl
{
public:
    YUV_Widget_Impl(YUV_Widget *widget);
    ~YUV_Widget_Impl();

    int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );
    void paintEvent(QPaintEvent * p);

private:
	YUV_Widget_Impl(const YUV_Widget_Impl&);
	YUV_Widget_Impl& operator=(const YUV_Widget_Impl&);
    YUV2RGB * _yuv2rgb;
    std::list < QImage * >img_list;
    int _listSize;

    ACE_Message_Block   *_pre_msg_block;

    ACE_Message_Queue<ACE_MT_SYNCH>   _img_queue;
    My_Msg_Block_Pool   _my_msg_block_pool;

    ACE_Thread_Mutex    _destory_mutex;

    YUV_Widget *_widget;

};

#endif // YUV_WIDGET_IMPL_H
