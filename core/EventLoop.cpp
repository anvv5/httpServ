//
// Created by Talla Chicken on 5/21/21.
//


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
#include "EventLoop.h"
#include <unistd.h>
#include <sys/socket.h>

bool is_ready_now(timeval* val)
{
    timeval now;
    gettimeofday(&now, NULL);
    if (timercmp(val, &now, <))
        return true;
    return false;
}

bool less_timeval(const TimeoutSubscriber* left, const TimeoutSubscriber* right)
{
    return timercmp(left->getWhen(), right->getWhen(), <);
}


EventLoop::EventLoop()
        :
        writeSubscribers(),
        readSubscribers(),
        timeoutSubscribers(less_timeval),
        read_fds(),
        write_fds(),
        error_fds(),
        max_fd(0),
        ready_fd(-1)
{
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&error_fds);
}
void EventLoop::addWriteSubscriber(WriteSubscriber* writeSubscriber)
{
    writeSubscribers.push_back(writeSubscriber);
    FD_SET(writeSubscriber->getFd(), &write_fds);
    max_fd = std::max(max_fd, writeSubscriber->getFd());
}
void EventLoop::addReadSubscriber(ReadSubscriber* readSubscriber)
{
    readSubscribers.push_back(readSubscriber);
    FD_SET(readSubscriber->getFd(), &read_fds);
    max_fd = std::max(max_fd, readSubscriber->getFd());
}

void EventLoop::addTimeoutSubscriber(TimeoutSubscriber* timeoutSubscriber)
{
    timeoutSubscribers.insert(timeoutSubscriber);
}

ssize_t EventLoop::read(void* buf, size_t len)
{
    assert(_readyReadFD != -1);
    ssize_t cnt = ::read(_readyReadFD, buf, len);
    _readyReadFD = -1;
    return cnt;
}

ssize_t EventLoop::write(const void* buf, size_t len)
{
    assert(_readyWriteFD != -1);
    ssize_t cnt = ::write(_readyWriteFD, buf, len);
    _readyWriteFD = -1;
    return cnt;
}
ssize_t EventLoop::recv(void* buf, size_t len, int flags)
{
    assert(_readyReadFD != -1);
    ssize_t cnt = ::recv(_readyReadFD, buf, len, flags);
    _readyReadFD = -1;
    return cnt;
}

ssize_t EventLoop::send(const void* buf, size_t len, int flags)
{
    assert(_readyWriteFD != -1);
    ssize_t cnt = ::send(_readyWriteFD, buf, len, flags);
    _readyWriteFD = -1;
    return cnt;
}

void EventLoop::runLoop()
{
    fd_set ready_read;
    fd_set ready_write;
    fd_set ready_error;
    while (! writeSubscribers.empty()
           || ! readSubscribers.empty()
           || ! timeoutSubscribers.empty())
    {
        std::cout << "LOOP: write: " << writeSubscribers.size() \
            << " read: " << readSubscribers.size() \
            << " error: " << errorSubscribers.size() << std::endl;
        FD_COPY(&read_fds, &ready_read);
        FD_COPY(&write_fds, &ready_write);
        FD_COPY(&error_fds, &ready_error);
        timeval timeout = MAX_TIMEOUT;
        if (! timeoutSubscribers.empty())
        {
            timeval* min_timeout = (*timeoutSubscribers.begin())->getWhen();
            if (timercmp(min_timeout, &timeout, <))
                timeout = *min_timeout;
        }
        if (select(max_fd + 1, &ready_read, &ready_write, &ready_error, &timeout) == -1)
            throw SocketException(strerror(errno));

        for (std::list<WriteSubscriber*>::iterator it = writeSubscribers.begin(); it != writeSubscribers.end(); ++it)
        {
            int fd = (*it)->getFd();
            if (FD_ISSET(fd, &ready_write))
            {
                (*it)->writeEvent(*this);
                if ((*it)->isDeleted())
                {
                    FD_CLR(fd, &write_fds);
                    delete *it;                     // TODO: отдельный цикл удаления, так как объект может поменять состояние в лююбое время
                    it = writeSubscribers.erase(it);
                    continue;
                }
            }
            ++it;
        }
        for (std::list<ReadSubscriber*>::iterator it = readSubscribers.begin(); it != readSubscribers.end(); ++it)
        {
            if (FD_ISSET((*it)->getFd(), &ready_error) && ! (*it)->isDeleted())
                (*it)->exceptionEvent(*this, SocketException("Out-band data"));
            if (FD_ISSET((*it)->getFd(), &ready_read) && ! (*it)->isDeleted())
                (*it)->readEvent(*this);
        }
        for (std::set<TimeoutSubscriber*>::iterator it = timeoutSubscribers.begin(); it != timeoutSubscribers.end();)
        {
            if (is_ready_now((*it)->getWhen()))
            {
                (*it)->eventTimeout(*this);
                if ((*it)->isDeleted())
                {
                    delete *it;
                    timeoutSubscribers.erase(it++);
                    continue;
                }
            }
            else
                break;
            ++it;
        }
    }
}

const timeval EventLoop::MAX_TIMEOUT = {1,0}; // 1 second