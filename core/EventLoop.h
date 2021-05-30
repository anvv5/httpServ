//
// Created by Talla Chicken on 5/19/21.
//

#ifndef WEBSERV_EVENTLOOP_H
#define WEBSERV_EVENTLOOP_H

#include <list>
#include <string>
#include <ctime>
#include <sys/select.h>
#include <set>
#include <ctime>
#include <sys/time.h>
#include <cerrno>
#include "Exceptions.h"
//#include "ClientSocket.h"
//#include "TCPServerSocket.h"
#include <iostream>

class EventLoop;

class IOSubscriber
{
    const int _fd;
    bool _isDeleted;
public:
    explicit IOSubscriber(const int fd) : _fd(fd), _isDeleted(false) {}
    void deleteThis()
    {
        _isDeleted = true;
    }
    virtual int getFd() const
    {
        return _fd;
    }
    virtual ~IOSubscriber() {};
public:
    virtual void event(EventLoop &eventLoop) = 0;
    virtual void exceptionEvent(EventLoop &eventLoop, const std::exception&) = 0;
    virtual bool isDeleted() const = 0;
};

class TimeoutSubscriber
{
    const timeval _date;
    bool _isDeleted;
public:
    bool isDeleted() const {
        return _isDeleted;
    }
    void deleteThis(){
        _isDeleted = true;
    }
    const timeval& getDate() const {
        return _date;
    }
    virtual ~TimeoutSubscriber() {};
public:
    virtual void eventTimeout(EventLoop &eventLoop) = 0;
    virtual void exceptionEvent(EventLoop &eventLoop, const std::exception&) = 0;
};

class EventLoop
{
    std::list<WriteSubscriber*> writeSubscribers;
    std::list<ReadSubscriber*> readSubscribers;
    std::set<TimeoutSubscriber*, bool(*)(const TimeoutSubscriber*, const TimeoutSubscriber*)> timeoutSubscribers;
    fd_set                        read_fds;
    fd_set                        write_fds;
    fd_set                        error_fds;
    int                             max_fd;
    int                            _readyReadFD;
    int                             _readyWriteFD;
    static const timeval MAX_TIMEOUT; // 1 second

public:
    EventLoop();
    void addWriteSubscriber(WriteSubscriber* writeSubscriber);
    void addReadSubscriber(ReadSubscriber* readSubscriber);
    void addTimeoutSubscriber(TimeoutSubscriber* timeoutSubscriber);
    ssize_t read(void* buf, size_t len);
    ssize_t write(const void* buf, size_t len);
    ssize_t recv(void* buf, size_t len, int flags);
    ssize_t send(const void* buf, size_t len, int flags);
    void runLoop();
};

//
//class FileRead;
//
//class ClientWrite: public WriteSubscriber
//{
//    int fd;
//    char *buf;
//    size_t _size;
//public:
//    void writeEvent(EventLoop &eventLoop)
//    {
//        // writebuf
//        eventLoop.addReadSubscriber(new FileRead())
//    }
//};
//
//class FileRead: public ReadSubscriber
//{
//    int fd;
//    int client_fd;
//    char *buf;
//public:
//    FileRead(int clientfd, const std::string& filename, char *buf) : fd(-1), client_fd(clientfd), buf(buf){}
//    void readEvent(EventLoop &eventLoop)
//    {
//        // fill buf
//        eventLoop.addWriteSubscriber(new ClientWrite(client_fd, buf));
//    }
//};
//
//class ClientRead: public ReadSubscriber
//{
//    int fd;
//public:
//    ClientRead(int fd) : fd(fd){}
//    void readEvent(EventLoop &eventLoop)
//    {
//        (void):
//    }
//};
//
//class ServerAccept: public ReadSubscriber
//{
//
//public:
//    void readEvent(EventLoop &eventLoop)
//    {
//        eventLoop.addReadSubscriber(new ClientRead())
//    }
//};

#endif //WEBSERV_EVENTLOOP_H
