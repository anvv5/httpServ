#include "Setting.hpp"

// clang++ -Wall -Wextra -Werror Setting.cpp

///////////////////////////////////////
//////////////// LEXER ///////////////
/////////////////////////////////////
void	strtrim(std::string &str, std::vector<std::string> &dst)
{
	//skip double spaces/tabs, before.
	int i = 0;
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	str.erase(0, i);

	//skip comments.
	if (str[0] == '#')
		str.clear();

	//skip empty lines.
	int len = str.length();
	if(len == 0)
		return ;

	//skip double spaces/tabs, after.
	i = len - 1;
	while (str[i] == ' ' || str[i] == '\t')
		i--;
	if (i < len)
		str.erase(i + 1, len);

	//trim str, fill dst with lexems.
	std::string					tmp("");
	for (unsigned long i = 0; i < str.size(); i++)
	{
		if (str[i] == '{' || str[i] == '}' || str[i] == ';' ||
			str[i] == '\t' || str[i] == ' ')
		{
			if (!tmp.empty())
			{
				dst.push_back(tmp);
				tmp.clear();
			}
		}
		else
			tmp.push_back(str[i]);
		
		if (str[i] == '{')
			dst.push_back("{");
		else if (str[i] == '}')
			dst.push_back("}");
		else if (str[i] == ';')
			dst.push_back(";");
	}
	if (!tmp.empty())
		dst.push_back(tmp);
}

//read file, fill lex vect, skip empty lines.
int		lexer(std::string const &file, std::vector<std::string>	&lex)
{
	std::ifstream					in(file);
	std::string						buff("");
	std::vector<std::string>		tmp;

	if (in.is_open() == false)
		throw std::runtime_error("can't open config");

	while (getline(in, buff))
	{
		strtrim(buff, tmp);
		if (tmp.size())
		{
			lex.reserve(lex.size() + tmp.size());
			lex.insert(lex.end(), tmp.begin(), tmp.end());
			tmp.clear();
		}
	}
	in.close();
	return 1;
}


///////////////////////////////////////
///////////// VALIDATOR //////////////
/////////////////////////////////////
void	preValidator(std::vector<std::string>	&src)
{
	ssize_t		cnt = 0;
	bool		flg = 0;

	//checking server existing
	std::vector<std::string>::iterator	it = src.begin();
	for (; it != src.end(); it++)
		if ((*it) == "server")
			flg = 1;
	if (!flg)
		throw std::runtime_error("config error: missing server");

	//checking {} count
	it = src.begin();
	flg = 0;
	for (; it != src.end(); it++)
	{
		if ((*it) == "{") {
			flg = 1;
			cnt++;
		}
		else if ((*it) == "}") {
			flg = 1;
			cnt--;
		}
	}
	if (cnt || (!cnt && !flg))
		throw std::runtime_error("config error: missing bracket");

	//checking tokens before {}
	it = src.begin();
	for (; it != src.end(); it++) {
		if ((*it) == "{") 
			if (!(*(it - 1) == "server" || *(it - 2) == "location"))
				throw std::runtime_error("config error: unknown param");

		if ((*it) == "}") 
			if (!(*(it - 1) == ";" || *(it - 1) == "}"))
				throw std::runtime_error("config error: missing \";\"");
	}
}

void	postValidator(Setting &src)
{
	std::vector<SettingServer>::iterator	it = src.getServers().begin();
	for (; it != src.getServers().end(); it++) {
		//checking address/port
		if ((*it).getFields().find("listen") == (*it).getFields().end())
			throw std::runtime_error("config error: missing \"listen\" config");
		
		//checking locations
		if ((*it).getLocations().empty())
			throw std::runtime_error("config error: missing \"location\" config");
	}
}


///////////////////////////////////////
/////////////// PARSER ///////////////
/////////////////////////////////////
std::map<std::string, std::vector<std::string> >	getPaired(std::vector<std::string> &src)
{
	std::map<std::string, std::vector<std::string> >	dst;
	std::vector<std::string>							tmp;
	std::vector<std::string>::iterator	   it = src.begin();

	if (*it == "location")
	{
		tmp.push_back(*(it + 1));
		dst.insert(std::pair<std::string, std::vector<std::string> >(src.front(), tmp));
		src.erase(src.begin(), it + 3);
		return dst;
	}

	while (it != src.end())
	{
		tmp.push_back(*it);
		if (*it == ";" || (it + 1) == src.end())
			break ;
		it++;
	}
	tmp.erase(tmp.begin());
	dst.insert(std::pair<std::string, std::vector<std::string> >(src.front(), tmp));
	src.erase(src.begin(), it + 1);
	return dst;
}

void	parser(std::vector<std::string> &src, Setting &dst)
{
	std::vector<std::string>::iterator	it = src.begin();
	SettingServer						tmpServ;
	SettingLocation						tmpLocal;

	int	type = 0;

	while (!src.empty())
	{
		//Setting
		while (type == 0 && !src.empty())
		{
			if (*it == "server" && *(it + 1) == "{")
			{
				src.erase(src.begin(), it + 2);
				type = 1;
			}
			else
				dst.addFields(getPaired(src));
			it = src.begin();
		}

		//SettingServer
		while (type == 1)
		{
			if (*it == "location" && *(it + 2) == "{")
			{
				tmpLocal.addFields(getPaired(src));
				type = 2;
			}
			else if (*it == "}")
			{
				dst.addServers(tmpServ);
				tmpServ.clear();
				src.erase(src.begin(), it + 1);
				type = 0;
			}
			else
				tmpServ.addFields(getPaired(src));
			it = src.begin();
		}

		//SettingLocation
		while (type == 2)
		{
			if (*it == "}")
			{
				tmpServ.addLocations(tmpLocal);
				tmpLocal.clear();
				src.erase(src.begin(), it + 1);
				type = 1;
			}
			else
				tmpLocal.addFields(getPaired(src));
			it = src.begin();
		}
		it = src.begin();
	}
}

void	Setting::parse(const std::string &file)
{
	std::vector<std::string>	lexems;

	lexer(file, lexems);
	preValidator(lexems);
	parser(lexems, *this);
	postValidator(*this);
}


///////////// SETTING /////////////
///////////// METHODS ////////////
void    Setting::addFields(std::map<std::string, std::vector<std::string> > const &src)
{this->_fields.insert(src.begin(), src.end());}

void    Setting::addServers(SettingServer const &src)
{this->_servers.push_back(src);}
 
std::map<std::string, std::vector<std::string> >    &Setting::getFields()
{return this->_fields;}

std::vector<SettingServer>                          &Setting::getServers()
{return this->_servers;}

void    Setting::print()
{
	std::vector<SettingServer>::iterator		itMap = this->getServers().begin();
	printf("+ -------------------------------- +\n");
	printf("|                                  |\n");
	printf("|      GLOBAL CONFIGS(sorted)      |\n");
	printf("|                                  |\n");
	printf("+ -------------------------------- +\n");
	printMap(this->_fields);
	while (itMap != this->getServers().end())
	{
		itMap->print();
		itMap++;
	}
}


///////////// SETTING_SERVER /////////////
//////////////// METHODS ////////////////
void    SettingServer::addFields(std::map<std::string, std::vector<std::string> > const &src)
{this->_fields.insert(src.begin(), src.end());}

void    SettingServer::addLocations(SettingLocation const &src)
{this->_locations.insert(std::pair<std::string, SettingLocation >
(src.getFields().find("location")->second[0], src));}

void    SettingServer::clear()
{this->_fields.clear(); this->_locations.clear();}

std::map<std::string, std::vector<std::string> >    &SettingServer::getFields()
{return this->_fields;}

std::map<std::string, SettingLocation>              &SettingServer::getLocations()
{return this->_locations;}

void    SettingServer::print()
{
	std::map<std::string, SettingLocation>::iterator		itMap = this->getLocations().begin();
	printf("\n+ -------------------------------- +\n");
	printf("|      SERVER_CONFIGS(sorted)      |\n");
	printf("+ -------------------------------- +\n");
	printMap(this->_fields);

	while (itMap != this->getLocations().end())
	{
		std::cout << "\nLOCATION_NAME: \"" << itMap->first << "\"\n";
		itMap->second.print();
		itMap++;
	}
}


///////////// SETTING_LOCATION /////////////
///////////////// METHODS /////////////////
void    SettingLocation::addFields(std::map<std::string, std::vector<std::string> > const &src)
{this->_fields.insert(src.begin(), src.end());}

void    SettingLocation::clear()
{this->_fields.clear();}

std::map<std::string, std::vector<std::string> >    SettingLocation::getFields() const
{return this->_fields;};

void    SettingLocation::print()
{
	printf("LOCATION_CONFIGS(sorted):\n========================\n");
	printMap(this->_fields);
}


///////////////////////////////////////
//////////////// UTILS ///////////////
/////////////////////////////////////
void	printMap(std::map<std::string, std::vector<std::string> > dst)
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

void	printVect(std::vector<std::string> lexems)
{
	std::vector<std::string>::iterator	it = lexems.begin();
	while (it != lexems.end())
	{
		printf("%s ", (*it).c_str());
		it++;
	}
	printf("\n\n");
}


///////////////////////////////////////
//////////////// MAIN ////////////////
/////////////////////////////////////
int		main(int argc, char **argv)
{
    Setting		setting;

	try
    {
		if (argc != 2)
			throw std::runtime_error("missing config");

		setting.parse(argv[1]);
		setting.print();
	}
	catch (std::exception &e)
    {
		std::cout << e.what() << std::endl;
	}
}