//
// Created by Talla Chicken on 5/21/21.
//

#ifndef WEBSERV_FILEREAD_H
#define WEBSERV_FILEREAD_H

#include "EventLoop.h"
#include "ClientSocket.h"

class FileRead: public ReadSubscriber
{
    int _fd;
    char *buf;
    size_t buf_size;
    const size_t buf_capacity;
    bool _isDeleted;
    ClientSocket _clientSocket;
public:
    FileRead(int fd, char *buf, size_t buf_capacity, const ClientSocket& clientSocket);
    virtual void readEvent(EventLoop &eventLoop);
    virtual bool isDeleted() const;
    virtual int getFd() const;
};


#endif //WEBSERV_FILEREAD_H
