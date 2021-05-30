#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <map>


class ClientParser
{
	std::string							_method;
	std::string							_path;
	std::string							_vers;
	
	std::string							_headersBuf;
	size_t								_headersLen;
	std::map<std::string, std::string>	_headers;

	std::string							_body;
	int									_bodyLen;
	int									_contLen;
	int									_waitLen;

	bool								_isHeadersDone;
	bool								_isBodyDone;

public:
	//return size of written header
	size_t	parseBuffer(char const *buf, size_t buf_size);
	bool	isBodyDone() const;
	bool	isHeadersDone() const;

	ClientParser();
	void	setMethod(const std::string &);
	void	setPath(const std::string &);
	void	setVers(const std::string &);
	void	addHeader(const std::map<std::string, std::string> &);
	void	setBody(const std::string &);
	void	setContLen(size_t);
	void	setBodyLen(size_t);
	void	setWaitLen(size_t);
	void	setIsBodyDone(bool);
	void	setIsHeadersDone(bool);
	std::string	const							&getMethod() const;
	std::string	const							&getPath() const;
	std::string	const							&getVers() const;
	std::map<std::string, std::string> const	&getHeaders() const;
	std::string	const							&getBody() const;
	size_t										getBodyLen() const;
	size_t										getContLen() const;
	size_t										getHeadersLen() const;
};

//		GET / HTTP/1.1
//		Host: alizar.habrahabr.ru

//		GET / HTTP/1.1\r\nHost: alizar.habrahabr.ru\r\n\r\n

// METHODS: GET HEAD POST PUT DELETE CONNECT OPTIONS TRACE PATCH

// HEADERS:

// -------------------<GENERAL>--------------------------


// ◦ Transfer-Encoding <GENERAL>
// Список способов кодирования, которые были применены к сообщению для передачи.
// Transfer-Encoding: chunked

// ◦ Date <GENERAL>
// Дата генерации отклика.
// Date: Tue, 15 Nov 1994 08:12:31 GMT

// --------------------<CLIENT>------------------------

// ◦ Host <CLIENT>
// Доменное имя и порт хоста запрашиваемого ресурса.
// Необходимо для поддержки виртуального хостинга на серверах.
// Host: ru.wikipedia.org

// ◦ Authorization <CLIENT>
// Данные для авторизации.
// Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==

// ◦ Referer <CLIENT>
// URI ресурса, после которого клиент сделал текущий запрос.

// ◦ User-Agent <CLIENT>
// Список названий и версий клиента и его компонентов с комментариями.
// User-Agent: Mozilla/5.0 (X11; Linux i686; rv:2.0.1) Gecko/20100101 Firefox/4.0.1

// ◦ Accept-Charsets <CLIENT>
// Перечень поддерживаемых кодировок для предоставления пользователю.
// Accept-Charset: utf-8

// ◦ Accept-Language <CLIENT>
// Список поддерживаемых естественных языков.
// Accept-Language: ru

// --------------------<SERVER>---------------------------

// ◦ Location <SERVER>
// URI по которому клиенту следует перейти или URI созданного ресурса.

// ◦ Retry-After <SERVER>
// Дата или время в секундах после которого можно повторить запрос.

// ◦ Server <SERVER>
// Список названий и версий веб-сервера и его компонентов с комментариями.
// Для прокси-серверов поле Via.
// Server: Apache/2.2.17 (Win32) PHP/5.3.5

// ◦ WWW-Authenticate <SERVER>
// Параметры аутентификации для выполнения метода к указанному ресурсу.

// ============================<BODY>============================

// ◦ Content-Type <BODY>
// Формат и способ представления сущности.
// Content-Type: text/html;charset=utf-8

// ◦ Content-Language <BODY>
// Один или несколько естественных языков содержимого сущности.
// Content-Language: en, ase, ru

// ◦ Content-Length <BODY>
// Размер содержимого сущности в октетах (которые в русском языке обычно называют байтами).
// Content-Length: 1348


// ◦ Allow <SERVER/BODY>
// Список поддерживаемых методов.
// Посылается сервером вместе со статусами 405 и 501,
// а также в ответе на метод OPTIONS.

// ◦ Content-Location <SERVER/BODY>
// Альтернативное расположение содержимого сущности.

// ◦ Last-Modified <SERVER/BODY>
// Дата последней модификации сущности.






void    ft_printMap(std::map<std::string, std::vector<std::string> > dst);