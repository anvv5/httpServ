#include "ClientParser.hpp"

// clang++ -Wall -Wextra -Werror ClientParser.cpp

void    ft_printVect(std::vector<std::string> lexems);

size_t	ClientParser::parseBuffer(const char *buf, size_t buf_size)
{
	std::stringstream					ssL(std::string(buf, buf_size));
	std::string							token;
	std::map<std::string, std::string>	tmp;

	this->_headersLen = ssL.str().find("\n\n");

	//check previos headers read
	if (!_isHeadersDone && _isBodyDone)
	{
		if (this->_headersLen == std::string::npos)
		{
			this->_headersLen = ssL.str().size();
			this->_headersBuf.append(ssL.str());

			if (this->_headersBuf.find("\n\n") == std::string::npos)
				return this->_headersLen;

			// this->_headersLen = this->_headersBuf.find("\n\n");
			// this->_headersBuf.erase(
			// 	this->_headersBuf.size() - ssL.str().size(), this->_headersBuf.back());
		}
		ssL.str(std::string());
		ssL << this->_headersBuf << std::string(buf, buf_size);
		this->_headersBuf.clear();
		this->_isHeadersDone = true;
std::cout << " --> " << ssL.str() << std::endl;
	}

	//check previos body remainder
	if (_waitLen)
	{
		if (_waitLen > (int)ssL.str().size())
		{
			this->_body.append(ssL.str());
			_waitLen -= ssL.str().size();
		}
		else if (_waitLen < (int)ssL.str().size() - 1)
		{
			// std::cout << "--- " << (int)ssL.str().size() << " " << ssL.str() << std::endl;
			throw std::runtime_error("request error: too large body");
		}
		else
		{
			this->_body.append(ssL.str());
			_waitLen = 0;
			this->_isBodyDone = true;
		}
		return 0;
	}

	// parse first line in request
	std::getline(ssL, token, '\n');

	std::stringstream			ssT;
	ssT << token;

	std::getline(ssT, token, ' ');
	this->setMethod(token);

	std::getline(ssT, token, ' ');
	this->setPath(token);

	std::getline(ssT, token, '\n');
	this->setVers(token);

std::cout << " ----> " << ssL.str() << std::endl;
	//parse headers
	while (std::getline(ssL, token, '\n'))
	{
		std::cout << " -------> " << token << std::endl;
		if (token.empty())
		{
			while (ssL)
				ssL >> this->_body;
			break ;
		}
		std::stringstream			ssT;
		ssT << token;
		std::getline(ssT, token, ':');
		tmp.insert(std::pair<std::string, std::string> (
			token, ssT.str().erase(0, ssT.str().find(' ') + 1)));
// std::cout << " --> " << this->_headers.find("Content-Length")->first << std::endl;
// std::cout << " -> " << token << std::endl;

		this->addHeader(tmp);
		tmp.clear();
	}

std::cout << " find-> " << this->_headers.find("Content-Length")->first << std::endl;
	//check body lenght
	if (this->_headers.find("Content-Length") == this->_headers.end())
		throw std::runtime_error("request error: miss content-lenght header");

	this->setContLen(atoi(this->_headers.find("Content-Length")->second.c_str()));
	this->setBodyLen(this->_body.size() - 1);

	if (_contLen != _bodyLen)
	{
		this->setWaitLen(_contLen - _bodyLen);
		if (_waitLen < 0)
			throw std::runtime_error("request error: too large body");
		this->setIsBodyDone(false);
		return (this->_headersLen);
	}
	this->setIsBodyDone(true);
	return (this->_headersLen);
}

bool	ClientParser::isBodyDone() const
{return _isBodyDone;}

bool	ClientParser::isHeadersDone() const
{return _isHeadersDone;}

///////////// CLIENT_PARSER/ /////////////
//////////////// METHODS ////////////////
ClientParser::ClientParser()
	: _method(""), _path(""), _vers(""), _headersBuf(""), _headersLen(0), _body(""),
	_bodyLen(0), _contLen(0), _waitLen(0), _isHeadersDone(false), _isBodyDone(true) {}

void	ClientParser::setMethod(const std::string & src)
{this->_method = src;}

void	ClientParser::setPath(const std::string & src)
{this->_path = src;}

void	ClientParser::setVers(const std::string & src)
{this->_vers = src;}

void	ClientParser::addHeader(const std::map<std::string, std::string> & src)
{this->_headers.insert(std::pair<std::string, std::string>
(src.begin()->first, src.begin()->second));}

void	ClientParser::setBody(const std::string & src)
{this->_body = src;}

void	ClientParser::setBodyLen(size_t src)
{this->_bodyLen = src;}

void	ClientParser::setContLen(size_t src)
{this->_contLen = src;}

void	ClientParser::setWaitLen(size_t src)
{this->_waitLen = src;}

void	ClientParser::setIsBodyDone(bool src)
{this->_isBodyDone = src;}

const std::string & ClientParser::getMethod() const {
    return this->_method;
}

std::string	const							&ClientParser::getPath() const
{return this->_path;}

std::string	const							&ClientParser::getVers() const
{return this->_vers;}

std::map<std::string, std::string> const	&ClientParser::getHeaders() const
{return this->_headers;}

std::string	const							&ClientParser::getBody() const
{return this->_body;}

size_t										ClientParser::getBodyLen() const
{return this->_bodyLen;}

size_t										ClientParser::getContLen() const
{return this->_contLen;}

size_t										ClientParser::getHeadersLen() const
{return this->_headersLen;}


///////////////////////////////////////
//////////////// MAIN ////////////////
/////////////////////////////////////
int		main(int argc, char* argv[])
{
    // char const		buf[] = "GET / HTTP/1.1\nHost: alizar.habrahabr.ru\nContent-Length: 5\n\n1234";
    char const		buf[] = "GET / HTTP/1.1\nbfg: aga\n";
	int		len = sizeof(buf);
	char const		buf2[] = "Co: lol\nContent-Length: 7\n\nHostQWe";
	int		len2 = sizeof(buf2);
	ClientParser	cp;

try{
	cp.parseBuffer(buf, len);
	// ft_printMap(cp.getHeaders());
	printf("meth: %s\n", cp.getMethod().c_str());
	printf("path: %s\n", cp.getPath().c_str());
	printf("vers: %s\n", cp.getVers().c_str());

	std::map<std::string,std::string>::const_iterator	it = cp.getHeaders().begin();
	for (;it != cp.getHeaders().end(); it++)
		std::cout << (*it).first << " - " << (*it).second << std::endl;


	printf("BODY: %s\n", cp.getBody().c_str());

	std::cout << "fullHeader: " << cp.isHeadersDone() << std::endl;
	std::cout << "fullBody: " << cp.isBodyDone() << std::endl;
// -----------------------------------------------------
		cp.parseBuffer(buf2, len2);
	// ft_printMap(cp.getHeaders());
	printf("meth: %s\n", cp.getMethod().c_str());
	printf("path: %s\n", cp.getPath().c_str());
	// printf("vers: %s\n", cp.getVers().c_str());
	std::cout << "vers:" << cp.getVers() << std::endl;

	it = cp.getHeaders().begin();
	for (;it != cp.getHeaders().end(); it++)
		std::cout << (*it).first << " - " << (*it).second << std::endl;
// std::cout << cp.getHeaders().find("Host")->first << std::endl;

// std::cout << cp.getHeaders().find("Host")->second << std::endl;
	std::cout << cp.getBody() << std::endl;

	std::cout << "fullHeader: " << cp.isHeadersDone() << std::endl;
	std::cout << "fullBody: " << cp.isBodyDone() << std::endl;

	std::cout << "f1: " << cp.getHeaders().find("bfg")->first << std::endl;

	(void)argc, (void)argv;

}
catch (std::exception& e) {std::cout << e.what() << std::endl;}
    return 0;
}


///////////////////////////////////////
//////////////// UTILS ///////////////
/////////////////////////////////////
void    ft_printMap(std::map<std::string, std::vector<std::string> > dst)
{
	std::map<std::string, std::vector<std::string> >::iterator		itMap = dst.begin();

	while (itMap != dst.end())
	{
   		std::cout << "key: " << itMap->first << "\nval: ";
		std::vector<std::string>::iterator	itVec = itMap->second.begin();
		while (itVec != itMap->second.end())
		{
			std::cout << *itVec << " ";
			itVec++;
		}
		std::cout << "\n\n";
		itMap++;
	}
}

void    ft_printVect(std::vector<std::string> lexems)
{
	std::vector<std::string>::iterator	it = lexems.begin();
	while (it != lexems.end())
	{
		printf("%s ", (*it).c_str());
		it++;
	}
	printf("\n\n");
}
