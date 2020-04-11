//
// Created by 谢仲涛 on 2020/4/6.
//

#include "arg_scrpit.h"
#include "server_config.h"
#include "lua/lua.hpp"
#include "lua_extend.h"
#include <exception>
using namespace std;

bool arg_script::load(YAML::Node &node) {

    this->_name=node["name"].as<string>();
    lua_settop(L, 0);
    int r = lua_getglobal(L, "load");
    if (r != LUA_TFUNCTION) {
        fprintf(stderr, " lua error: load is not function load \n");
        throw runtime_error("lua error: load is not function load ");
        return false;
    }
    if (node["value"]){
        lua_pushstring(L,node["value"].as<string>().c_str());
    } else{
        lua_pushstring(L,"");
    }
    if(node["option"]){
        YAML::Node option= node["option"];
        lua_createtable(L, 0, option.size());
        for(YAML::const_iterator it=option.begin();it!=option.end();++it) {
            lua_pushstring(L, it->first.as<string>().c_str());
            lua_pushstring(L, it->second.as<string>().c_str());
            lua_settable(L, -3);
        }
    } else{
        lua_createtable(L, 0, 0);
    }

    if(lua_pcall(L,2,0,0)!=LUA_OK){
        const char * err = lua_tolstring(L, 1, NULL);
        fprintf(stderr," lua error: %s\n",err);
        string e="lua error:";
        if(err)e.append(err);
        throw runtime_error(e);
        return false;
    }
    return true;
}

void arg_script::push_path(const char *path) {
    lua_getglobal(L, "package");
    {
        lua_getfield(L, -1, "path");
        const char * temp = lua_tolstring(L, -1, NULL);
        filesystem::path p{path};
        p.append("?.lua");
        string new_path=temp;
        new_path.append(";");
        new_path.append(p);
        p=path;
        p.append("?/init.lua");
        new_path.append(";");
        new_path.append(p);
        lua_settop(L, -2);
        lua_pushstring(L, new_path.c_str());
        lua_setfield(L, -2, "path");
        lua_settop(L, 1);
    }
    {
        lua_getfield(L, -1, "cpath");
        const char * temp = lua_tolstring(L, -1, NULL);
        filesystem::path p{path};
        p.append("?.so");
        string new_path=temp;
        new_path.append(";");
        new_path.append(p);
        lua_settop(L, -2);
        lua_pushstring(L, new_path.c_str());
        lua_setfield(L, -2, "path");
        lua_settop(L, 1);
    }
}

bool arg_script::next(string &value, string &short_value) {
    lua_settop(L, 0);
    lua_getglobal(L, "next");
    if(lua_pcall(L,0,3,0)!=LUA_OK){
        const char * err = lua_tolstring(L, 1, NULL);
        fprintf(stderr," lua error: %s\n",err);
        string e="lua error:";
        if(err)e.append(err);
        throw runtime_error(e);
        return false;
    }
    const char* v = lua_tolstring(L, 1, 0);
    const char* sv = lua_tolstring(L, 2, 0);
    bool end = lua_toboolean(L, 3) != 0;
    if (v){
        value=v;
    }
    if(sv){
        short_value=sv;
    }
    return end;
}

bool arg_script::reset() {
    lua_settop(L, 0);
    lua_getglobal(L, "reset");
    if(lua_pcall(L,0,3,0)!=LUA_OK) {
        const char *err = lua_tolstring(L, 1, NULL);
        fprintf(stderr, " lua error: %s\n", err);
        string e="lua error:";
        if(err)e.append(err);
        throw runtime_error(e);
        return false;
    }
    return true;
}

bool arg_script::add_glob_var(const char *key,const char *value) {
    if (L== nullptr){
        return false;
    }
    lua_pushstring(L, value);
    lua_setglobal(L, key);
    return true;
}

arg_script::arg_script() {
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_openExtendlibs(L);

}

void arg_script::init(const char *name) {
    AppConfig &appConfig=getAppConfig();
    push_path(appConfig.lua_package.c_str());
    filesystem::path path{appConfig.lua_lib};
    path.append(name);
    path.concat(".lua");
    if(luaL_loadfile(L,path.c_str())!=LUA_OK){
        fprintf(stderr," lua error: load file error\n");
        throw runtime_error("lua error: load file error");
    }
    if(lua_pcall(L,0,-1,0)!=LUA_OK){
        fprintf(stderr," lua error: load file error\n");
        throw runtime_error("lua error: load file error");
    }
}
