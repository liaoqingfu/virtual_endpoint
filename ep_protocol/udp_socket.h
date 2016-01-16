#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H


class UDP_Socket
{
public:
    UDP_Socket();

    int init_sendsock ();
    int init_recvsock ( unsigned int port, int ip = 0);
};

#endif // UDP_SOCKET_H
