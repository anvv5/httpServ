#ifndef ARCHIVE_SETTING_H
#define ARCHIVE_SETTING_H

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <array>


class SettingLocation
{
    std::map<std::string, std::vector<std::string> >    _fields;

public:
    void    print();
    void    clear();

    void    addFields(std::map<std::string, std::vector<std::string> > const &);
    std::map<std::string, std::vector<std::string> >    getFields() const;
};


class SettingServer
{
    std::map<std::string, std::vector<std::string> >    _fields;
    std::map<std::string, SettingLocation>              _locations;

public:
    void    print();
    void    clear();

    void    addFields(std::map<std::string, std::vector<std::string> > const &);
    void    addLocations(SettingLocation const &);
    std::map<std::string, std::vector<std::string> >    &getFields();
    std::map<std::string, SettingLocation>              &getLocations();
};


class Setting
{
    std::map<std::string, std::vector<std::string> >    _fields;
    std::vector<SettingServer>                          _servers;

public:
    void	parse(std::string const &);
    void    print();

    void    addFields(std::map<std::string, std::vector<std::string> > const &);
    void    addServers(SettingServer const &);
    std::map<std::string, std::vector<std::string> >    &getFields();
    std::vector<SettingServer>                          &getServers();
};

//utils
void    printMap(std::map<std::string, std::vector<std::string> >);
void    printVect(std::vector<std::string>);

#endif //ARCHIVE_SETTING_H

// минимум один сервер
/* на сервер:
//          минимум один путь в каждом сервере
//          порт и адрес сервера обязательно
   на путь:
        или root, или proxy_pass // обязательно
            root:
                cgi
                autoindex
                index
                proxy_pass
                ничего
*/