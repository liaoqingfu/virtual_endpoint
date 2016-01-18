#include "ep_interface.h"
#include "video_decoder_task.h"
#include "video_render_task.h"
#include "video_capture_task.h"
#include "video_encoder_task.h"
#include "nalu2rtp_h264.h"
#include "rtp2nalu_h264.h"
#include "video_render.h"
#include "yuv_widget.h"


Ep_Interface::Ep_Interface()
    :_video_render(NULL)
    ,_video_decoder(NULL)
    ,_video_recv(NULL)
    ,_video_capture(NULL)
    ,_video_encoder(NULL)
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

    return 0;
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

int Ep_Interface::video_encode_init()
{
    Encoder_Param param;
    param.width = 352;
    param.height = 288;
    param.fps = 24;
    param.encoder_type = Encoder_Param::H264;
    param.bitrate = 352 * 288 * 3 * 24;

    _video_send = new Nalu2Rtp_H264();
    static Video_Encoder::NALU_CB nalu_cb = std::bind(&Nalu2Rtp_H264::operator (), _video_send,
                                                      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    _video_encoder = new Video_Encoder_Task(param, &nalu_cb);
    static Video_Capture_Task::VCapture_Cb capture_cb = std::bind(&Video_Encoder_Task::operator (), _video_encoder,
                                                                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    _video_capture = new Video_Capture_Task(&capture_cb);

    return 0;
}

int Ep_Interface::video_encode_start(int port, int ip)
{
    _video_send->start(ip, port);
    _video_encoder->start();
    _video_capture->start();

    return 0;
}

void Ep_Interface::video_encode_stop()
{
    _video_send->stop();
    _video_encoder->stop();
    _video_capture->stop();
}
