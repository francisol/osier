//
// Created by 谢仲涛 on 2020/4/6.
//
#ifndef OSIER_CONTROLLER_H
#define OSIER_CONTROLLER_H
#include "db.h"
#include <pthread.h>
#include "interface_controller.h"
#include <unordered_map>
#include "runner.h"
#include "server_config.h"
#include "yaml-cpp/yaml.h"
#include <memory>

using namespace std;
class controller : public interface_controller {

public:
    controller(db &_db) : _db(_db) {
        pthread_mutex_init(&mutex, NULL);
        AppConfig &appConfig=getAppConfig();
        core_num = appConfig.core_num;
        current_core=0;
        current_task=0;
    }

    void run();

    bool refresh();
    void cb_done(int id);
    void err_done(int id, const std::string &msg);
    int get_core_num(){ return  core_num;}
    int get_current_core(){ return  current_core;}
    int get_current_task(){ return  current_task;}

private:
    int core_num;
    int current_core;
    int current_task;
    pthread_mutex_t mutex;
    db &_db;
    unordered_map<int, runner*> runners;
    unordered_map<int, int> cores;

};


#endif //OSIER_CONTROLLER_H
