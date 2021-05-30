//
// Created by Talla Chicken on 5/21/21.
//

#include "FileRead.h"
#include "SendFile.h"

FileRead::FileRead(int fd, char *buf, size_t buf_capacity, const ClientSocket &clientSocket)
:
    _fd(fd),
    buf(buf),
    buf_size(0),
    buf_capacity(buf_capacity),
    _isDeleted(false),
    _clientSocket(clientSocket)
{}

void FileRead::readEvent(EventLoop &eventLoop) {
    ssize_t cnt = read(_fd, buf + buf_size, buf_capacity - buf_size);
    if (cnt == -1)
        throw SocketException(strerror(errno));
    if (cnt == 0)
    {
        eventLoop.addWriteSubscriber(new SendFile(_clientSocket, _fd, buf, buf_size, buf_capacity, true));
        _isDeleted = true;
    }
    buf_size += cnt;
    if (buf_size == buf_capacity)
    {
        eventLoop.addWriteSubscriber(new SendFile(_clientSocket, _fd, buf, buf_size, buf_capacity, false));
        _isDeleted = true;
    }
}

bool FileRead::isDeleted() const {
    return _isDeleted;
}

int FileRead::getFd() const {
    return _fd;
}
