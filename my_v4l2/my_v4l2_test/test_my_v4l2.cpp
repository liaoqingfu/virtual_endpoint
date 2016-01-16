#include <iostream>

#include "../v4l2device.h"
#include <string.h>
#include "v4l2device.h"

using namespace std;

//Video_Capture::init( const std::string& device, const IO aio )
int test_video_capture_init()
{
    Video_Capture pVideo;
    //IO io = READ;

    bool r = pVideo.init( "/dev/video0" );
    if(r)
    {
        return 0;
    }

    return -1;
}

int test_video_capture_init_NULL()
{
    Video_Capture pVideo;
    //IO io = READ;

    bool r = pVideo.init( "/dev/video1" );
    if(r)
    {
        return 0;
    }

    return -1;
}

int test_video_capture_release()
{
    Video_Capture pVideo;
    //IO io = READ;

    bool r = pVideo.init( "/dev/video0" );
    if(!r)
        return -1;

    pVideo.release();
    return 0;
}

//void Video_Capture::start_capture()
int test_video_capture_start_capture()
{
    Video_Capture pVideo;
    bool r = pVideo.init( "/dev/video0" );
    if(!r)
        return -1;

    pVideo.start_capture();

    return 0;
}

int test_video_capture_stop_capture()
{
    Video_Capture pVideo;
    bool r = pVideo.init( "/dev/video0" );
    if(!r)
        return -1;

    pVideo.start_capture();

    pVideo.stop_capture();
    return 0;
}

//const Video_Capture::Buffer& Video_Capture::lock_frame( const float atimeout)
int test_video_capture_lock_frame()
{
    string device = "/dev/video0";

    Video_Capture pVideo;
    bool r = pVideo.init(device );
    if(!r)
        return -1;

    pVideo.start_capture();

    const Video_Capture::Buffer& b = pVideo.lock_frame();

    return 0;
}

int test_video_capture_unlock_frame()
{
    string device = "/dev/video0";

    Video_Capture pVideo;
    bool r = pVideo.init(device );
    if(!r)
        return -1;

    pVideo.start_capture();

    const Video_Capture::Buffer& b = pVideo.lock_frame();

    pVideo.unlock_frame();

    return 0;
}

//v4l2_pix_format Video_Capture::get_format()
int test_video_capture_get_format()
{
     v4l2_pix_format fmt ;

     string device = "/dev/video0";

     Video_Capture pVideo;
     bool r = pVideo.init(device);
     if(!r)
         return -1;

    fmt = pVideo.get_format();

    cout << "fmt is : " << fmt.pixelformat << endl;

    return 0;
}

//bool Video_Capture::set_format( const v4l2_pix_format& afmt )
int test_video_capture_set_format_yuv420()
{
    Video_Capture pVideo;
    v4l2_pix_format fmt ;
    __u32 fallback = V4L2_PIX_FMT_YUV420;
    fmt.pixelformat = fallback;

    bool r = pVideo.init("/dev/video0");
    if(!r)
        return -1;

    //cout << __LINE__ << endl;

    if (pVideo.set_format(fmt))
        return 0;
    return -1;
}

int test_video_capture_set_format_yuv422p()
{
    Video_Capture pVideo;
    v4l2_pix_format fmt ;
    __u32 fallback = V4L2_PIX_FMT_YUV422P;//PIX_FMT_YUYV422
    fmt.pixelformat = fallback;

    bool r = pVideo.init("/dev/video0");
    if(!r)
        return -1;

    if (pVideo.set_format(fmt))
        return 0;
    return -1;
}

//int Video_Capture::get_fps()
int test_video_capture_get_fps()
{
    Video_Capture pVideo;

    bool r = pVideo.init("/dev/video0");
    if(!r)
        return -1;

    int rc = pVideo.get_fps();
    if (rc == 0)
        return 0;
    return -1;
}

int main(int argc, char *argv[])
{
    int result = -1;

    if (argc != 3)
    {
        cout << "please input right amount args!" << endl;
        return -1;
    }

    if (0 == strcmp(argv[1], "my_v4l2"))
    {
        if (0 == strcmp(argv[2], "init"))
        {
            result = test_video_capture_init();
        }
        else if (0 == strcmp(argv[2], "init_NULL"))
        {
            result = test_video_capture_init_NULL();
        }
        else if (0 == strcmp(argv[2], "release"))
        {
            result = test_video_capture_release();
        }
        else if (0 == strcmp(argv[2], "start_capture"))
        {
            result = test_video_capture_start_capture();
        }
        else if (0 == strcmp(argv[2], "stop_capture"))
        {
            result = test_video_capture_stop_capture();
        }
        else if (0 == strcmp(argv[2], "lock_frame"))
        {
            result = test_video_capture_lock_frame();
        }
        else if (0 == strcmp(argv[2], "unlock_frame"))
        {
            result = test_video_capture_unlock_frame();
        }
        else if (0 == strcmp(argv[2], "get_format"))
        {
            result = test_video_capture_get_format();
        }
        else if (0 == strcmp(argv[2], "set_format_yuv420"))
        {
            cout << "this is format : set_format_yuv420" << endl;

            result = test_video_capture_set_format_yuv420();
        }
        else if (0 == strcmp(argv[2], "set_format_yuv422p"))
        {
            result = test_video_capture_set_format_yuv422p();
        }
        else if (0 == strcmp(argv[2], "get_fps"))
        {
            result = test_video_capture_get_fps();
        }
    }

    cout << "the result is " << result << endl;

    return result;
}
