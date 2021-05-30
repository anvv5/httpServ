//
// Created by Talla Chicken on 5/21/21.
//

#include "SendFile.h"
#include "FileRead.h"

SendFile::SendFile(const ClientSocket& clientSocket, int fileFD, char *buf, size_t bufSize, size_t bufCapacity, bool eof)
        : _fileFD(fileFD),
          _client(clientSocket),
          _buf(buf),
          _bufSize(bufSize),
          _bufCapacity(bufCapacity),
          _eof(eof),
          _sentCnt(0),
          _isDeleted(false)
{}

void SendFile::writeEvent(EventLoop &eventLoop) {
    if (_bufSize == 0)
    {
        _isDeleted = true;
        return ;
    }
    ssize_t cnt = _client.send(_buf + _sentCnt, _bufSize - _sentCnt);  // TODO: обрыв соединения
    if (cnt == -1)
        throw SocketException(strerror(errno));
    if (cnt < (ssize_t)(_bufSize - _sentCnt))
    {
        _sentCnt += cnt;
    }
    else
    {
        if (!_eof)
            eventLoop.addReadSubscriber(new FileRead(_fileFD, _buf, _bufCapacity, _client));
        else
        {
            delete [] _buf;
            close(_fileFD);
        }
        _isDeleted = true;
    }
}

bool SendFile::isDeleted() const {
    return _isDeleted;
}

int SendFile::getFd() const {
    return _client.getFD();
}
