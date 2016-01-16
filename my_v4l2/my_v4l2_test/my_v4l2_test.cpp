#include <iostream>
#include "my_v4l2_test.h"
#include <vector>
#include <unistd.h>

my_v4l2_Test::my_v4l2_Test()
{
}

int my_v4l2_Test::init_test_video0()
{
    Video_Capture temp;
    int ret = access( "/dev/video0", F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    bool r = temp.init( "/dev/video0" );
    if(true == r){
        return 0;
    }
    return -1;
}

int my_v4l2_Test::init_test_video1()
{
    Video_Capture temp;
    int ret = access( "/dev/video1", F_OK);
    if(ret != 0){
        std::cout<<"device video1 not exist!"<<std::endl;
        return !ret;
    }
    bool r = temp.init( "/dev/video0" );
    if(true == r){
        return 0;
    }
    return -1;
//    try{
//        temp.init( "/dev/video1");
//    }
//    catch(...){
//        return 0;
//    }
}

int my_v4l2_Test::v4l2_enum()
{
    int ret = access( "/dev/video0", F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    Video_Capture cap;
    try
    {
        cap.init ( "/dev/video0" );
    }
    catch( std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::vector< v4l2_fmtdesc > fmts = cap.get_formats();

    for( size_t i = 0; i < fmts.size(); ++i )
    {
        const v4l2_fmtdesc& fmt = fmts[i];

        std::string flags = "(";
        if( fmt.flags & V4L2_FMT_FLAG_COMPRESSED )
            flags += "C";
        if( fmt.flags & V4L2_FMT_FLAG_EMULATED )
            flags += "E";
        else
            flags += "N";
        flags += ")";

        std::cout << fmt.index << ": " << fmt.description << " " << flags << std::endl;

        std::vector< v4l2_frmsizeenum > sizes = cap.get_sizes(fmt);

        for( size_t j = 0; j < sizes.size(); ++j )
        {
            const v4l2_frmsizeenum& size = sizes[j];
            std::cout << "\t" << size << std::endl;

            std::vector< v4l2_frmivalenum > intervals = cap.get_intervals(fmt, size);
            for( size_t k = 0; k < intervals.size(); ++k )
            {
                const v4l2_frmivalenum& interval = intervals[k];
                std::cout <<  "\t\t" << interval << std::endl;
            }
        }
    }
    return 0;
}

int my_v4l2_Test::start_capture_test()
{
    int ret = access( "/dev/video0", F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    std::string device = "/dev/video0";

    Video_Capture dev;
    try
    {
        dev.init ( device );
    }
    catch( std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
//     vector< Video_Capture::IO > ios = dev.supported_io();

     dev.start_capture();
     return 0;
}

int my_v4l2_Test::stop_capture_test()
{
    int ret = access( "/dev/video0", F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    std::string device = "/dev/video0";

    Video_Capture dev;
    try
    {
        dev.init ( device );
    }
    catch( std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
//     vector< Video_Capture::IO > ios = dev.supported_io();

     dev.stop_capture();
     return 0;
}

int my_v4l2_Test::get_format_test()
{
    int ret = access( "/dev/video0", F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    Video_Capture dev;
    dev.init("/dev/video0");
    v4l2_pix_format format = dev.get_format();
    std::cout<<format.width<<std::endl;
    if(format.width){
        return 0;
    }
}

int my_v4l2_Test::get_fps_test()
{
    Video_Capture dev;
    dev.init("/dev/video0");
    int fps = dev.get_fps();
    if(0 < fps){
        return 0;
    }
}

int my_v4l2_Test::set_fps_test(int fps)
{
    Video_Capture dev;

    dev.init("/dev/video0");

    std::cout<<dev.get_fps()<<std::endl;
    bool flag = dev.set_fps(fps);
    std::cout<<fps<<std::endl;
//    sleep(5);
    int temp = dev.get_fps();
    std::cout<<temp<<std::endl;//设置失败
    if(true == flag){
        return 0;
    }
}

int my_v4l2_Test::supported_io_test()
{
    Video_Capture dev;

    dev.init("/dev/video0");
    std::vector< Video_Capture::IO > v;
    std::vector< Video_Capture::IO > s = dev.supported_io();
    std::vector<Video_Capture::IO>::const_iterator it;
    for(it=s.begin(); it!=s.end(); it++){
        std::cout<<*it<<std::endl;
        v.push_back(*it);
    }
    if(v.size() > 0){
        return 0;
    }else{
        return -1;
    }
}

int my_v4l2_Test::selected_io_test()
{
    Video_Capture dev;

    dev.init("/dev/video0");
    Video_Capture::IO temp = dev.selected_io();
    switch(temp){
    case Video_Capture::READ:{
        return 0;
        break;
    }
    case Video_Capture::USERPTR:{
        return 0;
        break;
    }
    case Video_Capture::MMAP:{
        return 0;
        break;
    }
    default:{
        return -1;
        break;
    }
    }
}

 int my_v4l2_Test::lock_frame_test()
 {
     Video_Capture dev;

     dev.init("/dev/video0");

     dev.start_capture();


    const Video_Capture::Buffer& b = dev.lock_frame();

     std::cout<<b._length<<std::endl;
     if(b._length > 0){
         return 0;
     }
 }

int my_v4l2_Test::unlock_frame_test()
{
    Video_Capture dev;

    dev.init("/dev/video0");
    dev.unlock_frame();
//    if()
    return 0;
}

int my_v4l2_Test::set_format_false_test()
{
    Video_Capture dev;

    bool init = dev.init("/dev/video0");
    std::cout<<init<<std::endl;
    if(init == true){
        v4l2_pix_format set;
//        set.width = 100;

        std::cout<<set.width<<std::endl;

        bool ret = dev.set_format(set);
        v4l2_pix_format get = dev.get_format();

        std::cout<<get.width<<std::endl;//设置失败
        if(ret == false){
            std::cout<<"ret"<<ret<<std::endl;
            return 0;
        }
        std::cout<<ret<<std::endl;
        return -1;
    }
    else{
        std::cout<<"init false"<<std::endl;
        return -1;
    }
}

int my_v4l2_Test::get_formats_test()
{
    Video_Capture dev;

    bool init = dev.init("/dev/video0");
    std::cout<<init<<std::endl;

    if(init == true){
        std::vector< v4l2_fmtdesc > a;
        std::vector< v4l2_fmtdesc > b = dev.get_formats();
        std::vector<v4l2_fmtdesc>::const_iterator it;

        if(b.size() <= 0){
            std::cout<<"get_formats error!"<<std::endl;
            return -1;
        }

        for(it=b.begin(); it!=b.end(); it++){
            std::cout<<*it<<std::endl;
            a.push_back(*it);
        }

        if(a.size() > 0){
            std::cout<<a.size()<<std::endl;
            return 0;
        }

    }else{
        std::cout<<"init false"<<std::endl;
        return -1;
    }

}


int my_v4l2_Test::get_sizes_test()
{
    Video_Capture dev;

    bool init = dev.init("/dev/video0");
    std::cout<<init<<std::endl;

//    std::vector< v4l2_frmsizeenum > a = dev.get_sizes()
    std::vector< v4l2_fmtdesc > fmts = dev.get_formats();
//    My_Device_Info info;
    if(fmts.size() > 0){
        for( size_t i = 0; i < fmts.size(); ++i ){
            const v4l2_fmtdesc& fmt = fmts[i];

            std::vector< v4l2_frmsizeenum > sizes = dev.get_sizes( fmt );
            for( size_t j = 0; j < sizes.size(); ++j )
            {
                const v4l2_frmsizeenum& size = sizes[j];
                std::cout<<size.index<<std::endl;
                std::cout<<size.pixel_format<<std::endl;
                std::cout<<size.type<<std::endl;
                std::cout<<size.reserved[0]<<std::endl;
                std::cout<<size.reserved[1]<<std::endl;

    //            std::vector< v4l2_frmivalenum > intervals = dev.get_intervals( fmt, size );
    //            for( size_t k = 0; k < intervals.size(); ++k )
    //            {
    //                const v4l2_frmivalenum& interval = intervals[k];

    //            }
            }
        }
        return 0;
    }
    else{
        std::cout<<"fmts get error!"<<std::endl;
        return -1;
    }
}

int my_v4l2_Test::get_intervals_test()
{
    Video_Capture dev;

    bool init = dev.init("/dev/video0");
    std::cout<<init<<std::endl;

    std::vector< Video_Capture::IO > ios = dev.supported_io();
    for( size_t i = 0; i < ios.size(); ++i )
    {
        std::string name;
        if( ios[i] == Video_Capture::READ )    name = "READ";
        if( ios[i] == Video_Capture::USERPTR ) name = "USERPTR";
        if( ios[i] == Video_Capture::MMAP )    name = "MMAP";
        if( ios[i] == dev.selected_io() )      name += " *";
        std::cerr << name << std::endl;
    }
    std::cerr << std::endl;

    // dump supported formats/sizes/fps
    std::cerr << "Supported formats:" <<std::endl;
    std::vector< v4l2_fmtdesc > fmts = dev.get_formats();
    for( size_t i = 0; i < fmts.size(); ++i )
    {
        const v4l2_fmtdesc& fmt = fmts[i];
        std::cerr << fmt << std::endl;

        std::vector< v4l2_frmsizeenum > sizes = dev.get_sizes( fmt );
        for( size_t j = 0; j < sizes.size(); ++j )
        {
            const v4l2_frmsizeenum& size = sizes[j];
            std::cerr << "\t" << size << ":";

            std::vector< v4l2_frmivalenum > intervals = dev.get_intervals( fmt, size );
            if(intervals.size() > 0){
                for( size_t k = 0; k < intervals.size(); ++k )
                {
                    const v4l2_frmivalenum& interval = intervals[k];
                    std::cerr << " (" << interval << ")";
                }
                std::cerr << std::endl;
                return 0;
            }
            else{
                std::cout<<"get_intervals error"<<std::endl;
                return -1;
            }

        }
    }
}














