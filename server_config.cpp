#include "server_config.h"

using namespace std;
void exist(std::filesystem::path&path){
    if(!filesystem::exists(path)){
        fprintf(stderr, "No such file or directory: %s \n", path.c_str());
        exit(0);
    }

}
 AppConfig _appConfig;

 void AppConfigInit(int argc, char** argv){
   _appConfig.core_num=128;
    _appConfig.port=1115;
    _appConfig.base_dir="/var/osier";
    _appConfig.lua_package="";
    _appConfig.lua_lib="";
    _appConfig.switch_user= false;
    CLI::App app{"tiny tool developped by C++ based on lua and SQLite","osier-server"};
    app.add_option("--core_num", _appConfig.core_num, "the max thread size of task");
    app.add_option("--base_dir", _appConfig.base_dir, "the program's cwd");
    app.add_option("--lua_lib", _appConfig.lua_lib, "lua scirpt dir default <base_dir>/lua_lib");
    app.add_option("--port", _appConfig.port, "server open port for cli");
    app.add_option("--lua_package", _appConfig.lua_package, "extra lua  search path default <base_dir>/lua_package");
    app.add_flag("--switch-user",_appConfig.switch_user,"run task for diff user");
     try {
         app.parse(argc, argv);
     } catch (const CLI::ParseError &e) {
         exit(app.exit(e));
     }

    exist(_appConfig.base_dir);
    if (_appConfig.lua_package.empty()){
        _appConfig.lua_package=_appConfig.base_dir;
        _appConfig.lua_package.append("lua_package");
    }
    if (_appConfig.lua_lib.empty()){
        _appConfig.lua_lib=_appConfig.base_dir;
        _appConfig.lua_lib.append("lua_lib");
    }
    exist(_appConfig.lua_package);
    exist(_appConfig.lua_lib);
}

 AppConfig& getAppConfig(){ return  _appConfig;}
