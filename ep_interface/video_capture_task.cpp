#include "video_capture_task.h"
#include "my_video_capture.h"

Video_Capture_Task::Video_Capture_Task(VCapture_Cb *cb)
    :_is_run(false)
    ,_handler_cb(cb)
    ,_video_capture(new My_Video_Capture())
{
//    init();
}

Video_Capture_Task::~Video_Capture_Task()
{
    if( _video_capture )
    {
        _video_capture->release();
        delete _video_capture;
        _video_capture = NULL;
    }
}

int Video_Capture_Task::init(char flag, int w, int h, int fps, const char *device)
{
    if( !_handler_cb || !_video_capture)
    {
        ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: NULL\n"), __PRETTY_FUNCTION__) );
        return -1;
    }

    return _video_capture->init(flag, w, h, fps, device);
}

int Video_Capture_Task::start()
{
    _is_run = true;
    _video_capture->init();
    activate();

    return 0;
}

void Video_Capture_Task::stop()
{
    _is_run = false;
    _video_capture->release();
    wait();
}

int Video_Capture_Task::svc()
{
    while( _is_run )
    {
        (*_video_capture)(_handler_cb);
    }

    return 0;
}

