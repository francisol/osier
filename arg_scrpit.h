//
// Created by 谢仲涛 on 2020/4/6.
//

#ifndef OSIER_ARG_SCRPIT_H
#define OSIER_ARG_SCRPIT_H


#include "lua/lua.hpp"
#include "yaml-cpp/yaml.h"
#include <string>
using namespace std;
class arg_script {
public:
    arg_script();
    void init(const char *name);
    bool load( YAML::Node &node);
    bool next(string&value,string&short_value);
    bool reset();
    string & name(){return _name;}
    bool add_glob_var(const char *key,const char *value);

private:
    lua_State *L=0;
    string _name;
    void push_path(const char * path);

};


#endif //OSIER_ARG_SCRPIT_H
