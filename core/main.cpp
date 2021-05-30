#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cerrno>
#include <iostream>
#include <map>
#include <fcntl.h>
#include <unistd.h>

#include "ClientSocket.h"
#include "TCPServerSocket.h"
#include "EventLoop.h"
#include "FileRead.h"

class ClientWrite: public WriteSubscriber
{
    ClientSocket _sock;
    bool _isDeleted;
public:
    ClientWrite(const ClientSocket& sock)
    : WriteSubscriber(), _sock(sock), _isDeleted(false)
    {}
    void writeEvent(EventLoop &eventLoop) {
        (void)eventLoop;
        _sock.send("HIHI\n", 5);
        _isDeleted = true;
    }

    bool isDeleted() const {
        return _isDeleted;
    }

    int getFd() const {
        return _sock.getFD();
    }
};

class ClientRead : public ReadSubscriber
{
    ClientSocket _sock;
    bool _isDeleted;
public:
    ClientRead(const ClientSocket& sock) : _sock(sock), _isDeleted(false) {}
    void readEvent(EventLoop &eventLoop) {
        char c;
        ssize_t recv_cnt = _sock.recv(&c, 1);
        if (recv_cnt == -1)
            throw SocketException(strerror(errno));
        if (c == 's' || recv_cnt == 0)
        {
            _sock.close();
            _isDeleted = true;
        }
        else
        {
            int fileFD = open("core/main.cpp", O_RDONLY | O_NONBLOCK);
            if (fileFD == -1)
                throw SocketException(strerror(errno));
            size_t bufCapacity = 1024;
            char *buf = new char[bufCapacity];
            eventLoop.addReadSubscriber(new FileRead(fileFD, buf, bufCapacity, _sock));
        }
    }

    bool isDeleted() const {
        return _isDeleted;
    }

    int getFd() const {
        return _sock.getFD();
    }
};

class ServerAccept : public ReadSubscriber, public ErrorSubscriber
{
    TCPServerSocket& _serverSocket;
public:
    ServerAccept(TCPServerSocket& serverSocket) : _serverSocket(serverSocket){}
    void readEvent(EventLoop &eventLoop) {
        eventLoop.addReadSubscriber(new ClientRead(_serverSocket.accept()));
    }

    void eventError(EventLoop &eventLoop) {
        (void)eventLoop;
        std::cout << "AAAAAAAAA" << std::endl;
    }

    bool isDeleted() const {
        return false;
    }

    int getFd() const {
        return _serverSocket.getFD();
    }
};

int main()
{
    EventLoop loop;
    TCPServerSocket serverSocket("*", 1234);
    TCPServerSocket serverSocket2("*", 12345);
    serverSocket.listen();
    serverSocket2.listen();
    loop.addReadSubscriber(new ServerAccept(serverSocket));
    loop.addReadSubscriber(new ServerAccept(serverSocket2));
    loop.addErrorSubscriber(new ServerAccept(serverSocket));
    loop.addErrorSubscriber(new ServerAccept(serverSocket2));
    loop.runLoop();
}



//int main() {
//    TCPServerSocket ss("*", 1234);
//    ss.listen();
//    std::map<int, ClientSocket> clients;
//    fd_set fds;
//    FD_ZERO(&fds);
//    FD_SET(ss.getFD(), &fds);
//    fd_set test_fds;
//    int max_fd = ss.getFD();
//    while (true)
//    {
//        std::cout << "CYCLE" << std::endl;
//        FD_COPY(&fds, &test_fds);
//        int count = select(max_fd + 1, &test_fds, NULL, NULL, NULL);
//        if (count == -1)
//        {
//            perror("select: ");
//            return 1;
//        }
//        if (FD_ISSET(ss.getFD(), &test_fds))
//        {
//            ClientSocket new_client = ss.accept();
//            std::cout << "Hi " << new_client.getPort() << std::endl;
//            clients.insert(std::pair<int, ClientSocket>(new_client.getFD(), new_client));
//            if (max_fd < new_client.getFD())
//                max_fd = new_client.getFD();
//            FD_SET(new_client.getFD(), &fds);
//        }
//        for (std::map<int, ClientSocket>::const_iterator it = clients.begin(); it != clients.end();)
//        {
//            if (FD_ISSET(it->first, &test_fds))
//            {
//                unsigned char buf[9];
//                ssize_t read_cnt = it->second.recv(buf, 8);
//                if (read_cnt == -1)
//                {
//                    perror("read");
//                    return -1;
//                }
//                if (read_cnt == 0)
//                {
//                    std::cout << "bay bay " << it->second.getPort() << std::endl;
//                    FD_CLR(it->first, &fds);
//                    ClientSocket discon_client = it->second;
//                    clients.erase(it++);
//                    discon_client.close();
//                    continue;
//                }
//                buf[read_cnt] = 0;
//                std::cout << it->second.getPort() << ": " << buf << std::endl;
//                if (it->second.send("kuku\n", 5) == -1)
//                    perror("write");
//                if (it->second.send("kuku\n", 5) == -1)
//                    perror("write");
//                if (it->second.send("kuku\n", 5) == -1)
//                    perror("write");
//            }
//            ++it;
//        }
//    }
//}