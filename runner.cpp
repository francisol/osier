//
// Created by 谢仲涛 on 2020/4/6.
//

#include "runner.h"
#include "yaml-cpp/yaml.h"
#include <filesystem>
#include "server_config.h"
#include <fstream>
#include "mustache.hpp"
#include "process.h"
using namespace std;
using namespace kainjow::mustache;
void * runner_run(void *data){
    runner* r=(runner*)data;
    r->run();
}
runner::runner(interface_controller &controller,Task _task):controller(controller),task(std::move(_task)) {

}

void runner::run() {
    while (true) {
        pthread_mutex_lock(&mutex);
        try {
            if (!((values.empty() && first_run()) || next_run())) {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }catch (const std::exception&e){
            controller.err_done(task.id,e.what());
            pthread_mutex_unlock(&mutex);
            return;
        }

        string cmd = cmd_tmpl->render(values);
        filesystem::path result_output{task.base_dir};
        result_output.append(output_tmpl->render(short_values));
        if(filesystem::exists(result_output)){
            pthread_mutex_unlock(&mutex);
            continue;
        }
        pthread_mutex_unlock(&mutex);
        run_command(cmd,result_output);
    }
    controller.cb_done(task.id);

}

bool runner::first_run() {
    for (auto item:arg_scripts) {
        string value,short_value;
        if(item->next(value,short_value)) {
            return false;
        }
        values.insert({item->name(),value});
        short_values.insert({item->name(),short_value});
    }
    return true;
}

bool runner::next_run() {
    int index=0;
    for (auto item:arg_scripts) {
        string value,short_value;
        if(item->next(value,short_value)){
            index += 1;
            continue;
        }
        values[item->name()]=value;
        short_values[item->name()]=short_value;
        break;
    }
    if(arg_scripts.size()==index){
        return false;
    }
    for (int i = 0; i < index; ++i) {
        string value,short_value;
        arg_script* item=arg_scripts[i];
        item->reset();
        if(item->next(value,short_value)){
            continue;
        }
        values[item->name()]=value;
        short_values[item->name()]=short_value;
    }
    return true;
}

bool runner::run_command(string &command, filesystem::path &output) {
    char buff[2048];
    const char * out=output.c_str();
    sprintf(buff,"cd %s \n mkdir -p %s \n %s > %s.tmp 2>&1 \nmv %s.tmp %s \n exit \n",
            task.base_dir.c_str(),
            output.parent_path().c_str(),
            command.c_str(),
            out,
            out,
            out
            );
    process proc;
    AppConfig &appConfig=getAppConfig();
    if(appConfig.switch_user){
        char * username=task.username.data();
        char * v[ ] ={"su","-",username, nullptr};
        proc.exec("su",v);
    } else{
        char * v[ ] ={"bash","-", nullptr};
        proc.exec("bash",v);
    }
    proc.write(buff,strlen(buff));
    while (proc.gets(buff,1024)){
        puts(buff);
    }
    return true;
}

runner::~runner() {
    auto item =arg_scripts.begin();
    while (item!=arg_scripts.end()){
        delete (*item);
        arg_scripts.erase(item);
    }

}

void runner::init() {
    filesystem::path path{task.base_dir};
    path.append("task.yaml");
    YAML::Node config = YAML::LoadFile(path);
    command = config["command"].as<string>();
    YAML::Node args = config["args"];
    for (YAML::const_iterator it = args.begin(); it != args.end(); ++it) {
        YAML::Node node = it->as<YAML::Node>();
        if (!node["type"]) {
            continue;
        }
        arg_script *argScript = new arg_script();
        argScript->init(node["type"].as<string>().c_str());
        argScript->add_glob_var("base_dir",task.base_dir.c_str());
        argScript->load(node);
        arg_scripts.push_back(argScript);
    }
    output_tmpl = new mustache(config["output"].as<string>());
    cmd_tmpl = new mustache(command);
    pthread_mutex_init(&mutex,NULL);
}
