#ifndef NALU2RTP_HANDLER_H
#define NALU2RTP_HANDLER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <ace/Task_Ex_T.h>
#include <ace/Malloc_T.h>

#include "protocol_defines.h"
#include "udp_socket.h"

struct Nalu_Buf
{
    unsigned char buf[RTP_MAX_BUF_SIZE];
    int len;
    unsigned int timestamp;
};

class Nalu2Rtp_Handler : public ACE_Task_Ex<ACE_MT_SYNCH, Nalu_Buf>
{
public:
    Nalu2Rtp_Handler();
    virtual ~Nalu2Rtp_Handler();

    int start(int ip, int port);
    void stop();
    virtual int svc (void);

    virtual int operator ()(unsigned char*, int len, int , unsigned int timestamp);

    virtual int rtp_sendto(unsigned char*, int);
    virtual int pack_and_send(unsigned char*, int len, unsigned int timestamp) = 0;

protected:
    int     _ssrc;

private:
    int     _send_fd;
    struct sockaddr_in _dst_addr;
    UDP_Socket          _udp_socket;
    ACE_Cached_Allocator<Nalu_Buf, ACE_Thread_Mutex>    _allocator;
};

#endif // NALU2RTP_HANDLER_H
