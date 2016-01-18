#ifndef EP_INTERFACE_H
#define EP_INTERFACE_H

#include <string>

class QWidget;
class Rtp_Recv_Handler;
class Video_Render_Task;
class Video_Decoder_Task;
class Video_Capture_Task;
class Video_Encoder_Task;
class Nalu2Rtp_H264;
class  Ep_Interface
{
public:
    static Ep_Interface* instance();
    ~Ep_Interface();

    int video_decode_init(QWidget* widget);
    int video_decode_start(int port, int ip = 0);
    void video_decode_stop();
//    bool video_set();

    int video_encode_init();
    int video_encode_start(int port, int ip);
    void video_encode_stop();

private:
    Ep_Interface();
    Video_Render_Task*      _video_render;
    Video_Decoder_Task*     _video_decoder;
    Rtp_Recv_Handler*       _video_recv;
    Video_Capture_Task*     _video_capture;
    Video_Encoder_Task*     _video_encoder;
    Nalu2Rtp_H264*          _video_send;
};

#endif // EP_INTERFACE_H
