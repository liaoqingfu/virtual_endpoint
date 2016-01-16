#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "udp_socket.h"
#include "protocol_defines.h"

UDP_Socket::UDP_Socket()
{

}

int UDP_Socket::init_sendsock ()
{
    int nSendBuf = UDP_SEND_BUF_SIZE;

    unsigned char	ttl = 64;
    unsigned int	loopback = 0;

    int sendfd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if(-1 == sendfd) perror("socket error");

    int rc = setsockopt ( sendfd, SOL_SOCKET, SO_SNDBUF, ( const char* ) & nSendBuf, sizeof ( int ) );
    if(rc != 0) perror("init_sendsock setsockopt");

    rc = setsockopt ( sendfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof ( ttl ) );
    if(rc != 0) perror("init_sendsock setsockopt");

    rc = setsockopt ( sendfd, IPPROTO_IP, IP_MULTICAST_LOOP, ( char* ) &loopback, sizeof ( loopback ) );
    if(rc != 0) perror("init_sendsock setsockopt");

    return sendfd;
}


int UDP_Socket::init_recvsock (unsigned int port, int ip)
{
    int nRecvBuf = UDP_RECV_BUF_SIZE;
    int recv_fd = -1;
    struct sockaddr_in  servaddr;
    int rtn = 0;

    recv_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    rtn = setsockopt ( recv_fd, SOL_SOCKET, SO_RCVBUF, ( const char* ) & nRecvBuf, sizeof ( int ) );
    if ( rtn != 0 ) perror( "init_recvsock setsockopt\n" );

    int opt = 1;
    rtn = setsockopt(recv_fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&opt,sizeof(opt));
    if ( rtn != 0 ) perror( "init_recvsock setsockopt\n" );

    bzero ( &servaddr, sizeof ( servaddr ) );
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons( port );
    servaddr.sin_addr.s_addr = (ip == 0 ? htonl(INADDR_ANY) : ip);

    rtn = ::bind ( recv_fd, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );
    if (rtn != 0)
    {
        perror("bind fd error!\n");
        close(recv_fd);
        return -1;
    }

    return recv_fd;
}

