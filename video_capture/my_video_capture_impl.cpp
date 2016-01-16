#include "my_video_capture_impl.h"

#include "my_v4l2/v4l2device.h"

#include <inttypes.h>

#include <string>
#include <cstdint>
using namespace std;

My_Video_Capture_Impl::My_Video_Capture_Impl()
    : _video_capture(NULL)
    , _stop_capture( true )
{
}

My_Video_Capture_Impl::~My_Video_Capture_Impl()
{
    _stop_capture = true;
    if(_video_capture != NULL)
        delete _video_capture;
}

int My_Video_Capture_Impl::init(char flag,
                                int w, int h,
                                int fps,
                                char const *device)
{
    //Todo:##################################################
    _flag = flag;
    _width = w;
    _height = h;
    _fps = fps;
    _stop_capture = false;

    _video_capture = new Video_Capture;
    try
    {
        _video_capture->init ( device );
    }
    catch( std::exception& e )
    {
        cerr << e.what() << endl;
        return -1;
    }
    //todo: set format
    v4l2_pix_format fmt = _video_capture->get_format();

    fmt.width = w;
    fmt.height = h;

    int my_fps = _video_capture->get_fps();
    if( my_fps != _fps )
    {
        fprintf( stderr, "old fps %d set to new fps %d \n", my_fps, _fps );
        _video_capture->set_fps( _fps );
        my_fps = _video_capture->get_fps();
        if( my_fps != _fps )
        {
            fprintf( stderr, "ssssssssssssssssssss set fps to %d failed, now fps is %d \n",
                     _fps, my_fps );
        }
    }
    __u32 fallback = V4L2_PIX_FMT_YUV420;
    fmt.pixelformat = fallback;
    if( !_video_capture->set_format(fmt) )
    {
        delete _video_capture;
        _video_capture = NULL;
        return -1;
    }
    //Todo:##################################################
    {
        v4l2_pix_format fmt = _video_capture->get_format();
        _linesizes[0] = fmt.bytesperline;
        _linesizes[1] = fmt.bytesperline / 2;
        _linesizes[2] = fmt.bytesperline / 2;

        _width = fmt.width;
        _height = fmt.height;

    }

    _video_capture->start_capture();

    return 0;
}

void My_Video_Capture_Impl::release()
{
    _mutex.acquire();
    _video_capture->release();
    _mutex.release();
}

int My_Video_Capture_Impl::set_video_param(char flag, int w, int h,
                                           int fps, char const *device)
{
    _stop_capture = true;

    _mutex.acquire();

    if( _video_capture )
    {
        _video_capture->stop_capture();
        _video_capture->release();
        delete _video_capture;
        _video_capture = NULL;
    }

    int rc = init( flag, w, h, fps, device );

    _mutex.release();
    return rc;
}

bool My_Video_Capture_Impl::set_brightness(int value)
{
    bool rc = _video_capture->set_brightness( value );
    return rc;
}

bool My_Video_Capture_Impl::set_hue(int value)
{
    bool rc = _video_capture->set_hue( value );
    return rc;
}

bool My_Video_Capture_Impl::set_saturation(int value)
{
    bool rc = _video_capture->set_saturation( value );
    return rc;
}

bool My_Video_Capture_Impl::set_contrast(int value)
{
    bool rc = _video_capture->set_contrast( value );
    return rc;
}

int My_Video_Capture_Impl::capture(int(*cb_func)(int w, int h, char**, int*) )
{
    if( _stop_capture )
        return 0;

    _mutex.acquire();

    try
    {
        const Video_Capture::Buffer& b = _video_capture->lock_frame();

        char* ptr = const_cast< char* >( b._start );
        char* planes[3];
        planes[0] = ptr;
        planes[1] = ptr + _height * _linesizes[0];
        planes[2] = ptr + _height * _linesizes[0] + (_height / 2) * _linesizes[1];

        (*cb_func)(_width, _height, planes, _linesizes);

        _video_capture->unlock_frame();
    }
    catch( ... )
    {
    }

    _mutex.release();
    return 0;
}

int My_Video_Capture_Impl::operator()(
        std::function< int (int w, int h, char**, int*) > *cb_func)
{
    if( _stop_capture )
        return 0;

    _mutex.acquire();

    //for(int i = 0; i < 6; ++i)
    {
        try
        {
            const Video_Capture::Buffer& b = _video_capture->lock_frame();

            char* ptr = const_cast< char* >( b._start );
            char* planes[3];
            planes[0] = ptr;
            planes[1] = ptr + _height * _linesizes[0];
            planes[2] = ptr + _height * _linesizes[0] + (_height / 2) * _linesizes[1];

            //if(i == 0)
            (*cb_func)(_width, _height, planes, _linesizes);

            _video_capture->unlock_frame();
        }
        catch( ... )
        {
            _mutex.release();
            return -1;
        }
    }
    _mutex.release();
    return 0;
}
