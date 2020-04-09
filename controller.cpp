//
// Created by 谢仲涛 on 2020/4/6.
//

#include "controller.h"
#include "runner.h"
#include <thread>
using namespace std;
bool controller::refresh() {
    pthread_mutex_lock(&mutex);
    Task task;
    if (!_db.get_wait_task(task)) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    if (task.core_num + current_core > core_num) {
        pthread_mutex_unlock(&mutex);
        return false;
    }
    _db.doing(task.id);
    try {
        runner *r =  new runner(*this, task);
        r->init();
        for (int i = 0; i < task.core_num; ++i) {
            pthread_t pthread;
            pthread_create(&pthread, NULL, runner_run,(void*)r);
        }
        runners.insert({task.id, r});
        cores.insert({task.id, task.core_num});
    } catch (const std::exception &e) {
        pthread_mutex_unlock(&mutex);
        err_done(task.id, e.what());
        throw e;
        return false;
    }
    ++current_task;
    current_core += task.core_num;
    pthread_mutex_unlock(&mutex);
    return true;
}

void controller::cb_done(int id) {
    pthread_mutex_lock(&mutex);
    auto core=  cores.find(id);
    if (core==cores.end()){
        pthread_mutex_unlock(&mutex);
        return;
    }
    --current_core;
   if ( --(core->second)==0){
       auto run= runners.find(id);
       delete run->second;
       runners.erase(run);
       cores.erase(core);
       _db.finished(id);
       --current_task;
   }
    pthread_mutex_unlock(&mutex);

}

void controller::run() {
    bool status= true;
    while (status){
        try {
            status=refresh();
        }catch (const std::exception &e){
            cout<<"error : "<<e.what()<<endl;
            status= true;
        }

    }
}

void controller::err_done(int id, const std::string &msg) {
    pthread_mutex_lock(&mutex);
    _db.do_error(id,msg.c_str());
    auto core=  cores.find(id);
    if (core==cores.end()){
        pthread_mutex_unlock(&mutex);
        return;
    }
    if ( --(core->second)==0){
        runners.erase(id);
        cores.erase(core);
    }
    pthread_mutex_unlock(&mutex);
}
