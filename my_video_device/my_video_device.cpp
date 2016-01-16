#include "my_video_device.h"
#include "my_v4l2/v4l2device.h"


My_Video_Device::My_Video_Device()
{
}

My_Video_Device::~My_Video_Device()
{
}

int My_Video_Device::init(const char* device_name)
{
    string device = device_name;  /* "/dev/video0"; */

    Video_Capture dev;
    try
    {
        dev.init ( device );
    }
    catch( std::exception& e )
    {
        cerr << e.what() << endl;
        return -1;
    }
    // dump supported formats/sizes/fps

    vector< v4l2_fmtdesc > fmts = dev.get_formats();
    My_Device_Info info;

    for( size_t i = 0; i < fmts.size(); ++i )
    {
        const v4l2_fmtdesc& fmt = fmts[i];
        info.pixelformat = fourcc_to_string(fmt.pixelformat);

        if( fmt.flags & V4L2_FMT_FLAG_COMPRESSED )
            info._flag = 'C';
        if( fmt.flags & V4L2_FMT_FLAG_EMULATED )
            info._flag = 'E';
        else
            info._flag = 'N';

        vector< v4l2_frmsizeenum > sizes = dev.get_sizes( fmt );
        for( size_t j = 0; j < sizes.size(); ++j )
        {
            const v4l2_frmsizeenum& size = sizes[j];
            info._width = size.discrete.width;
            info._height = size.discrete.height;

            vector< v4l2_frmivalenum > intervals = dev.get_intervals( fmt, size );
            for( size_t k = 0; k < intervals.size(); ++k )
            {
                const v4l2_frmivalenum& interval = intervals[k];
                info._fps = interval.discrete.denominator;
                _my_device_info.push_back(info);
            }
        }
    }

    return 0;
}
