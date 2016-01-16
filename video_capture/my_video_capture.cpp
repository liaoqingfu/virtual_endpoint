#include "my_video_capture.h"

#include "my_video_capture_impl.h"

My_Video_Capture::My_Video_Capture() : _impl(NULL)
{
    _impl = new My_Video_Capture_Impl;
}

My_Video_Capture::~My_Video_Capture()
{
    if( _impl )
    {
        delete _impl;
        _impl = NULL;
    }
}

int My_Video_Capture::init(char flag,
                           int w, int h,
                           int fps,
                           char const *device)
{
    if(_impl)
        return _impl->init(flag, w, h, fps, device);

    return -1;
}

void My_Video_Capture::release()
{
    if( _impl )
        _impl->release();
}

int My_Video_Capture::set_video_param(char flag,
                                int w, int h,
                                int fps,
                                char const *device)
{
    if( _impl )
    {
        return _impl->set_video_param(flag, w, h, fps, device );
    }
    return -1;
}

bool My_Video_Capture::set_brightness(int value)
{
    if( _impl )
        return _impl->set_brightness( value );

    return false;
}

bool My_Video_Capture::set_hue(int value)
{
    if( _impl )
        return _impl->set_hue( value );

    return false;
}

bool My_Video_Capture::set_saturation(int value)
{
    if( _impl )
        return _impl->set_saturation( value );

    return false;
}

bool My_Video_Capture::set_contrast(int value)
{
    if( _impl )
        return _impl->set_contrast( value );

    return false;
}

int My_Video_Capture::capture(int(*cb_func)(int w, int h, char**, int*) )
{
    if( _impl )
        return _impl->capture(cb_func);

    return -1;
}

int My_Video_Capture::operator()( std::function< int (int w, int h, char**, int*) > *cb_func)
{
    if( _impl )
        return (*_impl)(cb_func);

    return -1;
}
