//
// Created by Talla Chicken on 5/20/21.
//

#ifndef WEBSERV_EXCEPTIONS_H
#define WEBSERV_EXCEPTIONS_H

#include <exception>
#include <string>

class SocketException : public std::exception
{
    std::string _err;
public:
    SocketException(const std::string& str) throw() : std::exception(), _err(str){}
    SocketException(const SocketException& src) throw() : std::exception(), _err(src._err){}
    virtual const char *what() const throw()
    {
        return _err.c_str();
    }
    virtual ~SocketException() throw(){}
};

#endif //WEBSERV_EXCEPTIONS_H
