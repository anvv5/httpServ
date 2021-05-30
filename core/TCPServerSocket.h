//
// Created by Talla Chicken on 5/20/21.
//

#ifndef WEBSERV_TCPSERVERSOCKET_H
#define WEBSERV_TCPSERVERSOCKET_H

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "ClientSocket.h"
#include "Exceptions.h"

class TCPServerSocket
{
public:
    static const int DEFAULT_LISTEN_QUEUE = 5;

    TCPServerSocket(const std::string& address, unsigned short int port)
            : _sock_addr(), _sock_fd(-1)
    {
        if (address == "*")
            _sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        else
            _sock_addr.sin_addr.s_addr = inet_addr(address.c_str());
        if (_sock_addr.sin_addr.s_addr == INADDR_NONE)
            throw std::invalid_argument("Invalid address");
        _sock_addr.sin_family = AF_INET;
        _sock_addr.sin_port = htons(port);
        _sock_addr_len = sizeof(_sock_addr);
        _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock_fd == -1)
            throw SocketException(strerror(errno));
        if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
            throw SocketException(strerror(errno));
        int reuse_true = 1;
        if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true)) == -1)
            throw SocketException(strerror(errno));
        if (bind(_sock_fd, (sockaddr*)&_sock_addr, _sock_addr_len) != 0)
            throw SocketException(strerror(errno));
    }
    ~TCPServerSocket()
    {
        if (_sock_fd != -1)
            close(_sock_fd);
    }
    void listen(int len = DEFAULT_LISTEN_QUEUE) const
    {
        if (::listen(_sock_fd, len) != 0)
            throw SocketException(strerror(errno));
    }
    ClientSocket accept() const
    {
        return ClientSocket(_sock_fd);
    }
    int getFD() const
    {
        return _sock_fd;
    }
    unsigned short int getPort() const
    {
        return ntohs(_sock_addr.sin_port);
    }

private:
    sockaddr_in _sock_addr;
    socklen_t _sock_addr_len;
    int _sock_fd;
};

#endif //WEBSERV_TCPSERVERSOCKET_H
