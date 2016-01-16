#ifndef EP_INTERFACE_H
#define EP_INTERFACE_H

#include <string>

class QWidget;
class Rtp_Recv_Handler;
class Video_Render_Task;
class Video_Decoder_Task;
class  Ep_Interface
{
public:
    static Ep_Interface* instance();
    ~Ep_Interface();

    int video_decode_init(QWidget* widget);
    int video_decode_start(int port, int ip = 0);
    void video_decode_stop();
//    bool video_set();

private:
    Ep_Interface();
    Video_Render_Task*      _video_render;
    Video_Decoder_Task*     _video_decoder;
    Rtp_Recv_Handler*       _video_recv;
};

#endif // EP_INTERFACE_H
