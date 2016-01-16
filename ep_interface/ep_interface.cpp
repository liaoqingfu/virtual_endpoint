#include "ep_interface.h"
#include "video_decoder_task.h"
#include "video_render_task.h"
#include "rtp2nalu_h264.h"
#include "video_render.h"
#include "yuv_widget.h"

Ep_Interface::Ep_Interface()
    :_video_render(NULL)
    ,_video_decoder(NULL)
    ,_video_recv(NULL)
{

}

Ep_Interface *Ep_Interface::instance()
{
    static Ep_Interface ep;
    return &ep;
}

Ep_Interface::~Ep_Interface()
{
    delete _video_recv;
    delete _video_decoder;
    delete _video_render;
}

int Ep_Interface::video_decode_init(QWidget *widget)
{
    _video_render = new Video_Render_Task(widget);
    static Video_Decoder::YUV_CB yuv_cb = std::bind(&Video_Render_Task::operator (), _video_render, std::placeholders::_1,
                                                    std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
    _video_decoder = new Video_Decoder_Task(&yuv_cb, Video_Decoder::H264);
    static Rtp_Recv_Handler::Handler_Cb rtp_cb = std::bind(&Video_Decoder_Task::operator (),
                                                           _video_decoder, std::placeholders::_1, std::placeholders::_2);
    _video_recv = new Rtp2Nalu_H264(&rtp_cb);
}

int Ep_Interface::video_decode_start(int port, int ip)
{
    int rt = _video_render->start();
    if(!rt) rt = _video_decoder->start();
    if(!rt) rt = _video_recv->start(port);

    return rt;
}

void Ep_Interface::video_decode_stop()
{
    _video_recv->stop();
    _video_decoder->stop();
    _video_render->stop();
}
