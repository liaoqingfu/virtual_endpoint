#ifndef RTP_RECV_HANDLER_H
#define RTP_RECV_HANDLER_H

#include <functional>

#include <ace/Task.h>

#include "udp_socket.h"

class  Rtp_Recv_Handler : public ACE_Task<ACE_NULL_SYNCH>
{
public:
    typedef std::function<bool(unsigned char*, int)>  Handler_Cb;

    Rtp_Recv_Handler(Handler_Cb* handler_cb);
    virtual ~Rtp_Recv_Handler();

    int start(unsigned int recv_port, int recv_ip = 0);
    void stop();

    virtual int svc (void);

    virtual bool rtp_parse(unsigned char* rtp_buf, int rtp_buf_len, unsigned char* out_buf, int& out_buf_len) = 0;

private:
    int             _sock_fd;
    bool            _is_run;
    Handler_Cb*     _hander_cb;
    UDP_Socket      _udp_socket;
};

#endif // RTP_RECV_HANDLER_H
