//
// Created by 谢仲涛 on 2020/4/6.
//

#ifndef OSIER_DB_H
#define OSIER_DB_H

#include "sqlite/sqlite3.h"
#include <string>
#include "vector"
#include <pthread.h>
#include<cstring>
using namespace std;
enum TaskStatus {
    Wait,
    Doing,
    Done,
    Error,
};
struct Task {
    int id;
    string name;
    int priority;
    string base_dir;
    TaskStatus status;
    int core_num;
    string created_at;
    string finished_at;
    string username;
    string msg;
};

class db {
public:
    db(const char* path);
    ~db();

    bool save(Task&task);
    bool query(const  char * name,Task&task);
    bool  clear();
    void get_wait_tasks(vector<Task> &tasks);
    void get_all(int from,int to,vector<Task> &tasks);

    bool get_wait_task(Task &task);
    bool reset(int id);
    bool remove(int id);

    bool doing(int id);
    bool finished(int id);
    bool do_error(int id, const char * msg);
private:
    sqlite3 * _db=nullptr;
    bool update_status(int name ,int n_status,int o_status, const char * msg );
    void read_row(sqlite3_stmt *stmt,Task&task);
    pthread_mutex_t mutex;


};


#endif //OSIER_DB_H
