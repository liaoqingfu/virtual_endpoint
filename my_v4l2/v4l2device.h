#ifndef INCLUDE_V4L2_DEVICE_GENERIC_HPP
#define INCLUDE_V4L2_DEVICE_GENERIC_HPP

#include <string>
#include <vector>
#include <iostream>

#include "v4l/libv4l2.h"
#include "v4l/linux/videodev2.h"

// wraps a V4L2_CAP_VIDEO_CAPTURE fd
class Video_Capture
{
public:
    struct Buffer
    {
        Buffer() : _start(NULL), _length(0) {}
        char* _start;
        size_t _length;
    };

    enum IO { READ, USERPTR, MMAP };

    Video_Capture(  );

    ~Video_Capture();

    bool init( const std::string& device, const IO aio = MMAP );
    void release();

    IO selected_io()
    {
        return _io;
    }

    std::vector< IO > supported_io()
    {
        return _supported;
    }

    void start_capture();

    void stop_capture();

    // wait for next frame and return it
    // timeout in seconds
    // someway to indicate timeout?  zero buffer?
    const Buffer& lock_frame( const float atimeout = -1.0f );

    void unlock_frame();

    v4l2_pix_format get_format();

    bool set_format( const v4l2_pix_format& afmt );

    int get_fps();
    bool set_fps( int fps );

    v4l2_queryctrl brightness_properties();
    v4l2_queryctrl hue_properties();
    v4l2_queryctrl saturation_properties();
    v4l2_queryctrl contrast_properties();

    int get_brightness();
    int get_hue();
    int get_saturation();
    int get_contrast();

    bool set_brightness(int value);
    bool set_hue(int value);
    bool set_saturation(int value);
    bool set_contrast(int value);

    std::vector< v4l2_fmtdesc > get_formats();

    std::vector< v4l2_frmsizeenum > get_sizes( const v4l2_fmtdesc& format );

    std::vector< v4l2_frmivalenum > get_intervals( const v4l2_fmtdesc& format, const v4l2_frmsizeenum& size );

private:
	Video_Capture(const Video_Capture&);
	Video_Capture& operator=(const Video_Capture&);
    std::vector< IO > io_methods();

    static const char* strreq(unsigned long r );

    static void xioctl(int fd, unsigned long request, void* arg );

    int _fd;
    IO _io;
    std::vector< IO > _supported;
    bool _capturing;

    bool _is_locked;
    Buffer _locked_frame;
    v4l2_buffer _locked_buffer;

    std::vector< Buffer > _buffers;
};

__u32 string_to_fourcc( const std::string& fourcc );

// TODO: little-endian (x86) specific?
std::string fourcc_to_string( const __u32 fourcc );

std::ostream& operator<<( std::ostream& os, const v4l2_fmtdesc& desc );

std::ostream& operator<<( std::ostream& os, const v4l2_frmsizeenum& size );

std::ostream& operator<<( std::ostream& os, const v4l2_fract& frac );

std::ostream& operator<<( std::ostream& os, const v4l2_frmivalenum& interval );

#endif
