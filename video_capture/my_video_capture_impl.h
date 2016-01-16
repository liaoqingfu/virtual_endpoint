#ifndef MY_VIDEO_CAPTURE_IMPL_H
#define MY_VIDEO_CAPTURE_IMPL_H

#include <functional>

#include <ace/Thread_Mutex.h>

class Video_Capture;
class My_Video_Capture_Impl
{
public:
    My_Video_Capture_Impl();
    ~My_Video_Capture_Impl();

    int init(char flag, int w, int h, int fps, char const *device);
    void release();

    int set_video_param(char flag, int w, int h, int fps, char const *device);

    bool set_brightness(int value);
    bool set_hue(int value);
    bool set_saturation(int value);
    bool set_contrast(int value);

    int capture(int(*cb_func)(int w, int h, char**, int*) );

    int operator()( std::function< int (int w, int h, char**, int*) > *cb_func);

private:
	My_Video_Capture_Impl(const My_Video_Capture_Impl&);
	My_Video_Capture_Impl& operator=(const My_Video_Capture_Impl&);
    Video_Capture *_video_capture;
    int _width;
    int _height;
    int _flag;
    int _fps;

    int _linesizes[4];

    bool _stop_capture;
    ACE_Thread_Mutex _mutex;
};

#endif // VIDEO_CAPTURE_IMPL_H
