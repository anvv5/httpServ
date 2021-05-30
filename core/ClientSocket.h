//
// Created by Talla Chicken on 5/20/21.
//

#ifndef WEBSERV_CLIENTSOCKET_H
#define WEBSERV_CLIENTSOCKET_H

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include "Exceptions.h"
#include <netinet/in.h>


class ClientSocket
{
public:
    ClientSocket(int fd, const sockaddr_in& addr, socklen_t sock_addr_len)
            : _sock_fd(fd), _sock_addr(addr), _sock_addr_len(sock_addr_len)
    {}
    explicit ClientSocket(int server_fd)
            : _sock_fd(-1), _sock_addr(), _sock_addr_len()
    {
        _sock_fd = accept(server_fd, (sockaddr*)&_sock_addr, &_sock_addr_len);
        if (_sock_fd == -1)
            throw SocketException(strerror(errno));
        if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
            throw SocketException(strerror(errno));
    }
    ssize_t send(const void* buf, size_t len) const
    {
        return ::send(_sock_fd, buf, len, 0);
    }
    ssize_t recv(void *buf, ssize_t len) const
    {
        return ::recv(_sock_fd, buf, len, 0);
    }
    void close()
    {
        if (_sock_fd != -1){
            ::close(_sock_fd);
            _sock_fd = -1;
        }
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
    int _sock_fd;
    sockaddr_in _sock_addr;
    socklen_t _sock_addr_len;
};

#endif //WEBSERV_CLIENTSOCKET_H
