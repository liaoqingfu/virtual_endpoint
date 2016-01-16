#include <sys/mman.h>
#include <fcntl.h>

#include <cerrno>

#include <cstring>
#include <queue>
#include <sstream>
#include <stdexcept>

#include "v4l2device.h"

#define THROW( d ) \
{ \
    std::ostringstream oss; \
    oss << "[" << __FILE__ << ":" << __LINE__ << "]"; \
    oss << " " << d; \
    throw std::runtime_error( oss.str() ); \
    } \


Video_Capture::Video_Capture():_fd(-1)
{

}

Video_Capture::~Video_Capture()
{
    release();

}

bool Video_Capture::init( const std::string& device, const IO aio )
{
    if(_fd != -1)
        return true;
    _fd = v4l2_open( device.c_str(), O_RDWR | O_NONBLOCK, 0);
    if( _fd == - 1 )
        THROW( "can't open " << device << ": " << strerror(errno) );

    // make sure this is a capture device
    v4l2_capability cap;
    xioctl( _fd, VIDIOC_QUERYCAP, &cap );
    if( !(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) )
        THROW("not a video capture device!");

    _supported = io_methods();
    if( _supported.empty() ) THROW("no supported IO methods!");

    bool found_requested = false;
    for( size_t i = 0; i < _supported.size(); ++i )
    {
        if( _supported[i] == aio )
        {
            found_requested = true;
            break;
        }
    }

    // use requested IO if supported, otherwise "fastest"
    _io = ( found_requested ? aio : _supported.back() );

    _capturing = false;
    _is_locked = false;

    return true;
}

void Video_Capture::release()
{
    if(_fd != -1)
    {
        unlock_frame();
        stop_capture();
        v4l2_close( _fd );
        _fd = -1;
    }
    
}

void Video_Capture::start_capture()
{
    if( _capturing ) THROW("already capturing!");
    _capturing = true;

    // grab current frame format
    v4l2_pix_format fmt = get_format();

    // from the v4l2 docs: "Buggy driver paranoia."
    unsigned int min = fmt.width * 2;
    if (fmt.bytesperline < min)
        fmt.bytesperline = min;
    min = fmt.bytesperline * fmt.height;
    if (fmt.sizeimage < min)
        fmt.sizeimage = min;

    const unsigned int bufCount = 4;

    if( _io == READ )
    {
        // allocate buffer
        _buffers.resize( 1 );
        _buffers[ 0 ]._length = fmt.sizeimage;
        _buffers[ 0 ]._start = new char[ fmt.sizeimage ];
    }
    else
    {
        // request buffers
        v4l2_requestbuffers req;
        memset( &req, 0, sizeof(req) );
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = ( _io == MMAP ? V4L2_MEMORY_MMAP : V4L2_MEMORY_USERPTR );
        req.count = bufCount;
        xioctl( _fd, VIDIOC_REQBUFS, &req );

        if( _io == USERPTR )
        {
            // allocate buffers
            _buffers.resize( req.count );
            for( size_t i = 0; i < _buffers.size(); ++i )
            {
                _buffers[ i ]._length = fmt.sizeimage;
                _buffers[ i ]._start = new char[ fmt.sizeimage ];
            }
        }
        else
        {
            // mmap buffers
            _buffers.resize( req.count );
            for( size_t i = 0; i < _buffers.size(); ++i )
            {
                v4l2_buffer buf;
                memset( &buf, 0, sizeof(buf) );
                buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory  = V4L2_MEMORY_MMAP;
                buf.index   = i;
                xioctl( _fd, VIDIOC_QUERYBUF, &buf );

                _buffers[i]._length = buf.length;
                _buffers[i]._start = (char*)v4l2_mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buf.m.offset);
                if( _buffers[i]._start == MAP_FAILED )
                    THROW("mmap() failed!");
            }
        }

        // queue buffers
        for( size_t i = 0; i < _buffers.size(); ++i )
        {
            v4l2_buffer buf;
            memset( &buf, 0, sizeof(buf) );
            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.index       = i;
            buf.memory = ( _io == MMAP ? V4L2_MEMORY_MMAP : V4L2_MEMORY_USERPTR );
            if( _io == USERPTR )
            {
                buf.m.userptr   = (unsigned long)_buffers[i]._start;
                buf.length      = _buffers[i]._length;
            }

            xioctl( _fd, VIDIOC_QBUF, &buf );
        }

        // _start streaming
        try
        {
            v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            xioctl( _fd, VIDIOC_STREAMON, &type );
        }
        catch( ... )
        {

        }
    }
}

void Video_Capture::stop_capture()
{
    if( !_capturing ) return;
    _capturing = false;

    if( _io == READ )
    {
        delete[] _buffers[0]._start;
    }
    else
    {
        // stop streaming
        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl( _fd, VIDIOC_STREAMOFF, &type );

        if( _io == USERPTR )
        {
            // free memory
            for( size_t i = 0; i < _buffers.size(); ++i )
                delete[] _buffers[i]._start;
        }
        else
        {
            // unmap memory
            for( size_t i = 0; i < _buffers.size(); ++i )
                if( -1 == v4l2_munmap(_buffers[i]._start, _buffers[i]._length) )
                    THROW( "munmap() failed!" );
        }
    }

}

// wait for next frame and return it
// timeout in seconds
// someway to indicate timeout?  zero buffer?
const Video_Capture::Buffer& Video_Capture::lock_frame( const float atimeout)
{
    (void)atimeout;
    if( _is_locked ) THROW( "already locked!" );
    _is_locked = true;

    // wait for frame
    while( true )
    {
        fd_set fds;
        FD_ZERO( &fds);
        FD_SET( _fd, &fds );

        timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int r = select( _fd + 1, &fds, NULL, NULL, &tv);
        if( -1 == r && EINTR == errno )
        {
            if( EINTR == errno )
                continue;
            THROW( "select() error" );
        }

        // timeout
        if( 0 == r ) continue;

        // fd readable
        break;
    }

    if( _io == READ )
    {
        if( -1 == v4l2_read( _fd, _buffers[0]._start, _buffers[0]._length) )
        {
            if( errno != EAGAIN && errno != EIO )
                THROW( "read() error" );
        }

        _locked_frame._start = _buffers[0]._start;
        _locked_frame._length = _buffers[0]._length;
    }
    else
    {
        memset( &_locked_buffer, 0, sizeof(_locked_buffer) );
        _locked_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _locked_buffer.memory = ( _io == MMAP ? V4L2_MEMORY_MMAP : V4L2_MEMORY_USERPTR );
//        if( -1 == v4l2_ioctl( _fd, VIDIOC_DQBUF, &_locked_buffer) )
//        {
//            if( errno != EAGAIN && errno != EIO )
//                THROW( "ioctl() error" );
//        }
        xioctl( _fd, VIDIOC_DQBUF, &_locked_buffer);

        size_t i;
        if( _io == USERPTR )
        {
            // only given pointers, find corresponding index
            for( i = 0; i < _buffers.size(); ++i )
            {
                if( _locked_buffer.m.userptr == (unsigned long)_buffers[i]._start &&
                        _locked_buffer.length == _buffers[i]._length )
                {
                    break;
                }
            }
        }
        else
        {
            i = _locked_buffer.index;
        }

        if( i >= _buffers.size() )
            THROW( "buffer index out of range" );

        _locked_frame._start = _buffers[i]._start;
        _locked_frame._length = _locked_buffer.bytesused;
    }

    return _locked_frame;
}

void Video_Capture::unlock_frame()
{
    if( !_is_locked ) return;
    _is_locked = false;

    if( _io == READ ) return;

    xioctl( _fd, VIDIOC_QBUF, &_locked_buffer );
}


v4l2_pix_format Video_Capture::get_format()
{
    v4l2_format fmt;
    memset( &fmt, 0, sizeof(fmt) );
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl( _fd, VIDIOC_G_FMT, &fmt );
    return fmt.fmt.pix;
}

bool Video_Capture::set_format( const v4l2_pix_format& afmt )
{
    v4l2_format fmt;
    memset( &fmt, 0, sizeof(fmt) );
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix = afmt;

    while( true )
    {
        try
        {
            xioctl( _fd, VIDIOC_S_FMT, &fmt );
        }
        catch( std::exception& e )
        {
            //if( errno == EBUSY ) continue;
            if( errno == EINVAL ) return false;
            throw e;
        }
        break;
    }

    return true;
}


int Video_Capture::get_fps()
{
    v4l2_streamparm param;
    memset( &param, 0, sizeof(param) );
    param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl( _fd, VIDIOC_G_PARM, &param );
    if( param.parm.capture.timeperframe.numerator != 0 )
        return param.parm.capture.timeperframe.denominator /
                param.parm.capture.timeperframe.numerator;
    return 0;
}

bool Video_Capture::set_fps( int fps )
{
    v4l2_streamparm param;
    memset( &param, 0, sizeof(param) );
    param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.parm.capture.timeperframe.denominator = fps;
    param.parm.capture.timeperframe.numerator = 1;
    xioctl( _fd, VIDIOC_S_PARM, &param );
    return true;
}

v4l2_queryctrl Video_Capture::brightness_properties()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_queryctrl query;
    query.id = V4L2_CID_BRIGHTNESS;
    query.type = V4L2_CTRL_TYPE_INTEGER;
    xioctl(_fd, VIDIOC_QUERYCTRL, &query);

    return query;
}

v4l2_queryctrl Video_Capture::hue_properties()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_queryctrl query;
    query.id = V4L2_CID_HUE;
    query.type = V4L2_CTRL_TYPE_INTEGER;
    xioctl(_fd, VIDIOC_QUERYCTRL, &query);

    return query;
}

v4l2_queryctrl Video_Capture::saturation_properties()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_queryctrl query;
    query.id = V4L2_CID_SATURATION;
    query.type = V4L2_CTRL_TYPE_INTEGER;
    xioctl(_fd, VIDIOC_QUERYCTRL, &query);

    return query;
}

v4l2_queryctrl Video_Capture::contrast_properties()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_queryctrl query;
    query.id = V4L2_CID_CONTRAST;
    query.type = V4L2_CTRL_TYPE_INTEGER;
    xioctl(_fd, VIDIOC_QUERYCTRL, &query);

    return query;
}

int Video_Capture::get_brightness()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_BRIGHTNESS;
    xioctl(_fd, VIDIOC_G_CTRL, &ctl);

    return ctl.value;
}

int Video_Capture::get_hue()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_HUE;
    xioctl(_fd, VIDIOC_G_CTRL, &ctl);

    return ctl.value;
}

int Video_Capture::get_saturation()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_SATURATION;
    xioctl(_fd, VIDIOC_G_CTRL, &ctl);

    return ctl.value;
}

int Video_Capture::get_contrast()
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_CONTRAST;
    xioctl(_fd, VIDIOC_G_CTRL, &ctl);

    return ctl.value;
}

bool Video_Capture::set_brightness(int value)
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_BRIGHTNESS;
    ctl.value = value;

    xioctl(_fd, VIDIOC_S_CTRL, &ctl);

    return true;
}

bool Video_Capture::set_hue(int value)
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_HUE;
    ctl.value = value;

    xioctl(_fd, VIDIOC_S_CTRL, &ctl);

    return true;
}

bool Video_Capture::set_saturation(int value)
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_SATURATION;
    ctl.value = value;

    xioctl(_fd, VIDIOC_S_CTRL, &ctl);

    return true;
}

bool Video_Capture::set_contrast(int value)
{
    if(-1 == _fd)
        THROW( "device is not open" );

    v4l2_control ctl;
    ctl.id = V4L2_CID_CONTRAST;
    ctl.value = value;

    xioctl(_fd, VIDIOC_S_CTRL, &ctl);

    return true;
}


std::vector< v4l2_fmtdesc > Video_Capture::get_formats()
{
    std::vector< v4l2_fmtdesc > fmts;

    int curIndex = 0;
    while( true )
    {
        v4l2_fmtdesc fmt;
        fmt.index = curIndex++;
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        try
        {
            xioctl( _fd, VIDIOC_ENUM_FMT, &fmt );
        }
        catch( std::exception& e )
        {
            // hit the end of the enumeration list, exit
            if( errno == EINVAL ) break;
            // todo: ????
            else throw e;
        }

        fmts.push_back( fmt );
    }

    return fmts;
}

std::vector< v4l2_frmsizeenum > Video_Capture::get_sizes( const v4l2_fmtdesc& format )
{
    std::vector< v4l2_frmsizeenum > sizes;

    int curIndex = 0;
    while( true )
    {
        v4l2_frmsizeenum size;
        size.index = curIndex++;
        size.pixel_format = format.pixelformat;

        try
        {
            xioctl( _fd, VIDIOC_ENUM_FRAMESIZES, &size );
        }
        catch( std::exception& e )
        {
            // hit the end of the enumeration list, exit
            if( errno == EINVAL ) break;
            // todo: ????
            else throw e;
        }

        sizes.push_back( size );

        // only discrete has multiple enumerations
        if( size.type != V4L2_FRMSIZE_TYPE_DISCRETE )
            break;
    }

    return sizes;
}

std::vector< v4l2_frmivalenum > Video_Capture::get_intervals( const v4l2_fmtdesc& format, const v4l2_frmsizeenum& size )
{
    std::vector< v4l2_frmivalenum > intervals;

    int curIndex = 0;
    while( true )
    {
        v4l2_frmivalenum interval;
        interval.index = curIndex++;
        interval.pixel_format = format.pixelformat;
        interval.width = size.discrete.width;
        interval.height = size.discrete.height;

        try
        {
            xioctl( _fd, VIDIOC_ENUM_FRAMEINTERVALS, &interval );
        }
        catch( std::exception& e )
        {
            // hit the end of the enumeration list, exit
            if( errno == EINVAL ) break;

            // todo: ????
            else throw e;
        }

        intervals.push_back( interval );
        curIndex++;
    }

    return intervals;
}

std::vector< Video_Capture::IO > Video_Capture::io_methods()
{
    std::vector< IO > supported;

    v4l2_capability cap;
    xioctl( _fd, VIDIOC_QUERYCAP, &cap );

    // test read/write
    if( cap.capabilities & V4L2_CAP_READWRITE )
        supported.push_back( READ );

    if( cap.capabilities & V4L2_CAP_STREAMING )
    {
        v4l2_requestbuffers req;
        // test userptr
        memset( &req, 0, sizeof(req) );
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.count = 1;
        req.memory = V4L2_MEMORY_USERPTR;
        if( 0 == v4l2_ioctl( _fd, VIDIOC_REQBUFS, &req ) )
        {
            supported.push_back( USERPTR );
            req.count = 0;
            // blind ioctl, some drivers get pissy with count = 0
            v4l2_ioctl( _fd, VIDIOC_REQBUFS, &req );
        }

        // test mmap
        memset( &req, 0, sizeof(req) );
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.count = 1;
        req.memory = V4L2_MEMORY_MMAP;
        if( 0 == v4l2_ioctl( _fd, VIDIOC_REQBUFS, &req ) )
        {
            supported.push_back( MMAP );
            req.count = 0;
            // blind ioctl, some drivers get pissy with count = 0
            v4l2_ioctl( _fd, VIDIOC_REQBUFS, &req );
        }
    }

    return supported;
}

const char* Video_Capture::strreq(unsigned long r )
{
    if( r == VIDIOC_QBUF )  return "VIDIOC_QBUF";
    if( r == VIDIOC_REQBUFS )  return "VIDIOC_REQBUFS";
    if( r == VIDIOC_QUERYBUF )     return "VIDIOC_QUERYBUF";
    if( r == VIDIOC_STREAMON )   return "VIDIOC_STREAMON";
    if( r == VIDIOC_STREAMOFF )     return "VIDIOC_STREAMOFF";
    if( r == VIDIOC_G_FMT )  return "VIDIOC_G_FMT";
    if( r == VIDIOC_S_FMT )  return "VIDIOC_S_FMT";
    if( r == VIDIOC_G_PARM )     return "VIDIOC_G_PARM";
    if( r == VIDIOC_S_PARM )   return "VIDIOC_S_PARM";
    if( r == VIDIOC_ENUM_FMT )     return "VIDIOC_ENUM_FMT";
    if( r == VIDIOC_ENUM_FRAMESIZES )  return "VIDIOC_ENUM_FRAMESIZES";
    if( r == VIDIOC_ENUM_FRAMEINTERVALS )  return "VIDIOC_ENUM_FRAMEINTERVALS";
    if( r == VIDIOC_QUERYCAP )     return "VIDIOC_QUERYCAP";

    return "UNKNOWN";
}

void Video_Capture::xioctl( int fd, unsigned long request, void* arg )
{
    //if (request <= -1)return;

    int ret = 0;
    do
    {
        ret = v4l2_ioctl( fd, request, arg );
    }
    while( ret == -1 && ( (errno == EINTR) || (errno == EAGAIN) ));

    // todo: ????
    if( ret == -1 )
        THROW( "ret: " << ret << " fd: " << fd << "arg: " << arg <<  " Req: " << request << " " << strreq(request) << ", ioctl() error: " << strerror(errno) );
}

__u32 string_to_fourcc( const std::string& fourcc )
{
    return v4l2_fourcc
            (
                fourcc[ 0 ],
            fourcc[ 1 ],
            fourcc[ 2 ],
            fourcc[ 3 ]
            );
}

// TODO: little-endian (x86) specific?
std::string fourcc_to_string( const __u32 fourcc )
{
    std::string ret = "0000";
    ret[0] = static_cast<char>( (fourcc & 0x000000FF) >>  0 );
    ret[1] = static_cast<char>( (fourcc & 0x0000FF00) >>  8 );
    ret[2] = static_cast<char>( (fourcc & 0x00FF0000) >> 16 );
    ret[3] = static_cast<char>( (fourcc & 0xFF000000) >> 24 );
    return ret;
}

std::ostream& operator<<( std::ostream& os, const v4l2_fmtdesc& desc )
{
    os << fourcc_to_string( desc.pixelformat ) << " ";

    std::string flags = "";
    if( desc.flags & V4L2_FMT_FLAG_COMPRESSED )
        flags += "C";
    if( desc.flags & V4L2_FMT_FLAG_EMULATED )
        flags += "E";
    else
        flags += "N";
    os << "(" << flags << ") ";

    os << "[" << desc.description << "]";
    return os;
}

std::ostream& operator<<( std::ostream& os, const v4l2_frmsizeenum& size )
{
    if( size.type == V4L2_FRMSIZE_TYPE_DISCRETE )
    {
        os << size.discrete.width << "x" << size.discrete.height;
    }
    else
    {
        os << size.stepwise.min_width << "-" << size.stepwise.max_width;
        os << "," << size.stepwise.step_width;

        os << "x";

        os << size.stepwise.min_height << "-" << size.stepwise.max_height;
        os << "," << size.stepwise.step_height;
    }
    return os;
}

std::ostream& operator<<( std::ostream& os, const v4l2_fract& frac )
{
    os << frac.numerator << "/" << frac.denominator;
    return os;
}

std::ostream& operator<<( std::ostream& os, const v4l2_frmivalenum& interval )
{
    if( interval.type == V4L2_FRMIVAL_TYPE_DISCRETE )
    {
        os << interval.discrete;
    }
    else
    {
        os << interval.stepwise.min;
        os << "-";
        os << interval.stepwise.max;
        os << ",";
        os << interval.stepwise.step;
    }
    return os;
}
