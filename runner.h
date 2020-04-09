//
// Created by 谢仲涛 on 2020/4/6.
//

#ifndef OSIER_RUNNER_H
#define OSIER_RUNNER_H


#include "interface_controller.h"
#include "db.h"
#include "arg_scrpit.h"
#include <vector>
#include <unordered_map>
#include "mustache.hpp"
#include <filesystem>
using namespace std;
using namespace kainjow::mustache;

void * runner_run(void *data);

class runner {

public:
    runner(interface_controller &controller, Task task);
    void init();
    ~runner();
    void run();

private:
    bool first_run();

    bool next_run();

    bool run_command(string &command, filesystem::path &output);

private:
    interface_controller &controller;
    Task task;
    string command;
    vector<arg_script *> arg_scripts;
    object values;
    object short_values;
    mustache *cmd_tmpl;
    mustache *output_tmpl;
    pthread_mutex_t  mutex{};
};

#endif //OSIER_RUNNER_H
