#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ace/Log_Msg.h>

#include "rtp_recv_handler.h"
#include "protocol_defines.h"

Rtp_Recv_Handler::Rtp_Recv_Handler(Handler_Cb *handler_cb)
    :_sock_fd(-1)
    ,_hander_cb(handler_cb)
{
}

Rtp_Recv_Handler::~Rtp_Recv_Handler()
{
}

int Rtp_Recv_Handler::start(unsigned int recv_port, int recv_ip)
{
    _sock_fd = _udp_socket.init_recvsock(recv_port, recv_ip);
    if( -1 == _sock_fd )
    {
        ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: udp socket init error\n"), __FUNCTION__) );
        return -1;
    }

    activate();

    return 0;
}


void Rtp_Recv_Handler::stop()
{
    ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: !!!!!!!!!!\n"), __FUNCTION__) );
    if( _sock_fd != -1 )
    {
        shutdown(_sock_fd, SHUT_RD);
        close(_sock_fd);
        _sock_fd = -1;
    }

    wait();
}

int Rtp_Recv_Handler::svc()
{
    sockaddr_in src_addr;
    socklen_t src_addr_len;
    int len = -1;
    bool rc = false;
    unsigned char* recv_buf = new unsigned char[RTP_MAX_BUF_SIZE];

    while( true )
    {
        memset(recv_buf, 0, RTP_MAX_BUF_SIZE );
        len = recvfrom(_sock_fd, recv_buf, RTP_MAX_BUF_SIZE, 0, (sockaddr*)&src_addr, &src_addr_len);

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:len = %d!\n"), __PRETTY_FUNCTION__, len));

        if( len < 0 )
        {
            ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: len = %d\n"), __PRETTY_FUNCTION__, len) );

            shutdown(_sock_fd, SHUT_RD);
            close(_sock_fd);
            _sock_fd = -1;
            break;
        }
        rc = rtp_parse(recv_buf, len, recv_buf, len);
        if( rc )
        {
            (*_hander_cb)(recv_buf, len);
        }
    }

    delete recv_buf;

    ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: thread exit!!!!!!!!!!!!!!!!!!!!\n"), __PRETTY_FUNCTION__) );

    return 0;
}

