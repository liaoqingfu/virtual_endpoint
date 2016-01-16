#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "video_encoder_factory.h"
#include "video_capture/my_video_capture.h"

using namespace std;

Video_Encoder* video_encoder_gl = NULL;
int fd = -1;

int nalu_handler(unsigned char* nalu, int  nalu_len, int, unsigned int)
{
    if(-1 == fd)  return -1;

    write(fd, nalu, nalu_len);

    return 0;
}

int capture_handler(int w, int , char** buf, int*)
{
    int linesize[] = { w, w/2,  w/2};
    if( video_encoder_gl )
        (*video_encoder_gl)((uint8_t**)buf, linesize);

    return 0;
}

int main(int argc, char** argv)
{   
    My_Video_Capture video_capture;
    int rc = video_capture.init();
    if(-1 == rc )  return -1;

    Encoder_Param param;
    param.width = 352;
    param.height = 288;
    param.encoder_type = Encoder_Param::H265;
    param.bitrate = 352*288*1.5*25;
    param.fps = 25;
    param.gop_len = 30;

    Video_Encoder::NALU_CB nalu_cb =
            std::bind(nalu_handler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    std::function< int (int w, int h, char**, int*) > capture_cb =
            std::bind(capture_handler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    video_encoder_gl = Video_Encoder_Factory::create(param, &nalu_cb);

    fd = open("xxx.nalu", O_CREAT | O_TRUNC | O_RDWR, 0666);

    int frames = 100;
    while(frames-- > 0)
    {
        video_capture(&capture_cb);
    }

    Video_Encoder_Factory::destroy(video_encoder_gl);
    video_capture.release();
    close(fd);

    return 0;
}
