//
// Created by Talla Chicken on 5/20/21.
//

#ifndef WEBSERV_NONBLOCKFILE_H
#define WEBSERV_NONBLOCKFILE_H

#include <exception>
#include <string>
#include <unistd.h>
#include <fcntl.h>

//class NonBlockFileException: public std::exception
//{
//    std::string _err;
//public:
//    NonBlockFileException(const std::string& str) throw() : std::exception(), _err(str){}
//    NonBlockFileException(const NonBlockFileException& src) throw() : std::exception(), _err(src._err){}
//    virtual const char *what() const throw()
//    {
//        return _err.c_str();
//    }
//    virtual ~NonBlockFileException() throw(){}
//};
//
//class NonBlockFile
//{
//    int _fd;
//public:
//    NonBlockFile(const char *filename, int oflag)
//    {
//        _fd = open(filename, oflag | O_NONBLOCK)
//    }
//};

#endif //WEBSERV_NONBLOCKFILE_H
