#include <iostream>
#include "CLI11.hpp"
#include "osier_client.h"
#include "yaml-cpp/yaml.h"
#include <unistd.h>
using namespace std;
int port=1115;

typedef struct {
    string base_dir;
}CreateCMD;
void create(CreateCMD&createCmd) {
    char buff[1024];
    filesystem::path temp =getcwd(buff,1024);
    temp.append(createCmd.base_dir);
    filesystem::path path;
    for (const auto &item:temp) {
        if (item == "..") {
            path = path.parent_path();
            continue;
        } else if (item == ".") {
            continue;
        }
        path.append(item.string());
    }
    CreateRequest request;
    FILE *fp= popen("whoami","r");
    char  buf[65];
    string username=  fgets(buf,64,fp);
    pclose(fp);
    username.pop_back();
    request.set_base_dir(path);
    request.set_username(username);
    cout<<path<<endl;
    path.append("task.yaml");
    YAML::Node node = YAML::LoadFile(path);
    if (node["name"]) {
        request.set_name(node["name"].as<string>());
    } else {
        cout << "file must has name";
        exit(0);
    }
    if (node["core_num"]) {
        request.set_core_num(node["core_num"].as<int>());
    } else {
        cout << "file must has core_num";
        exit(0);
    }
    if (node["priority"]) {
        request.set_priority(node["priority"].as<int>());
    } else {
        cout << "file must has priority";
        exit(0);
    }
    string target_str = "127.0.0.1:";
    target_str.append(to_string(port));
    osier_client client(port);
    client.create(request);
    exit(0);
}

void _list(int start,int end){
    ListRequest request;
    request.set_start(start);
    request.set_end(end);
    osier_client client(port);
    client.list(request);
    exit(0);
}

void restart(const string& name){
    NameMessage request;
    request.set_name(name);
    osier_client client(port);
    client.restart(request);
    exit(0);
}

void status(const string& name){
    NameMessage request;
    request.set_name(name);
    osier_client client(port);
    client.detail(request);
    exit(0);
}

void remove(const string& name){
    NameMessage request;
    request.set_name(name);
    osier_client client(port);
    client.remove(request);
    exit(0);
}

void serverStatsu(){
    EmptyMessage request;
    osier_client client(port);
    client.serverStaus(request);
    exit(0);
}



int main(int argc, char** argv) {


    string name;
    int start = 0;
    int end = 10;
    CLI::App app{"App description", "ss"};
    CreateCMD createCMD;
    app.add_option("--port", port, "xxx", true);
    CLI::App *createApp = app.add_subcommand("create", "ccccc")
            ->ignore_case()
            ->callback([&createCMD]() {
                create(createCMD);
            });
    createApp->add_option("--base_dir", createCMD.base_dir, "base_dir");

    CLI::App *listApp = app.add_subcommand("list", "show tasks")->callback([start, end]() {
        _list(start, end);
    });
    listApp->add_option("--start", start, "start", true);
    listApp->add_option("--end", end, "end", true);

    CLI::App *restartApp = app.add_subcommand("restart", "restart task")->callback([&name]() {
        restart(name);
    });
    restartApp->add_option("--name",name,"task name")->required();


    CLI::App *statusApp = app.add_subcommand("status", "show task status")->callback([&name]() {
        status(name);
    });
    statusApp->add_option("--name",name,"task name")->required();


    CLI::App *deletesApp = app.add_subcommand("delete", "delete task")->callback([&name]() {
        remove(name);
    });
    deletesApp->add_option("--name",name,"task name")->required();


    CLI::App *serversApp = app.add_subcommand("server", "show server info")->callback([]() {
        serverStatsu();
    });

    CLI11_PARSE(app, argc, argv);
    printf("test");
    return 0;
}
