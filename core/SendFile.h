//
// Created by Talla Chicken on 5/21/21.
//

#ifndef WEBSERV_SENDFILE_H
#define WEBSERV_SENDFILE_H

#include "EventLoop.h"
#include "ClientSocket.h"

class SendFile: public WriteSubscriber
{
    int _fileFD;
    ClientSocket _client;
    char *_buf;
    size_t _bufSize;
    size_t _bufCapacity;
    bool _eof;
    size_t _sentCnt;
    bool _isDeleted;
public:
    SendFile(const ClientSocket& clientSocket, int fileFD, char *buf, size_t bufSize, size_t bufCapacity, bool eof);
    virtual void writeEvent(EventLoop &eventLoop);
    virtual bool isDeleted() const;
    virtual int getFd() const;
};


#endif //WEBSERV_SENDFILE_H
