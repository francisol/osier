//
// Created by 谢仲涛 on 2020/4/6.
//
#include <string>
#include "CLI11.hpp"

#include <filesystem>
#ifndef OSIER_CONFIG_H
#define OSIER_CONFIG_H

using namespace std;
typedef struct  {
    int core_num{};
    filesystem::path base_dir;
    filesystem::path lua_lib;
    filesystem::path lua_package;
    int port{} ;
    bool switch_user{};
}AppConfig;
void exist(std::filesystem::path&path);

void AppConfigInit(int argc, char** argv);

 AppConfig& getAppConfig();
#endif //OSIER_CONFIG_H
