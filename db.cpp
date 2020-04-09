//
// Created by 谢仲涛 on 2020/4/6.
//

#include "db.h"
#define CREATE_TABLE "CREATE TABLE IF NOT EXISTS \"tasks\" (\
\"id\"	INTEGER PRIMARY KEY AUTOINCREMENT,\
\"task_name\"	TEXT UNIQUE,\
\"priority\"	INTEGER DEFAULT 0,\
\"core_num\"	INTEGER DEFAULT 1,\
\"base_dir\"	TEXT,\
\"status\"	INTEGER,\
\"created_at\"	TEXT,\
\"finished_at\"	TEXT,\
\"username\" TEXT,\
\"msg\"	TEXT\
)"
#include<iostream>
using namespace std;
db::db(const char *path) {
    pthread_mutex_init(&mutex,NULL)
;   int rc= sqlite3_open(path,&_db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
        exit(0);
    }
    char *zErrMsg = 0;
    rc=sqlite3_exec(_db,CREATE_TABLE,NULL,0,&zErrMsg);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
        exit(0);
    }
}

db::~db() {
    if(_db){
        sqlite3_close(_db);
        _db= nullptr;
    }
}

bool db::save(Task &task) {
            pthread_mutex_lock(&mutex);
        int rc;
    sqlite3_stmt *stmt = NULL;
    const  char *sql="INSERT INTO tasks (task_name, priority,base_dir,status,core_num,created_at,finished_at,username) values (?1, ?2,?3,?4,?5,?6,?7,?8)";
    rc=sqlite3_prepare_v2( _db,sql,strlen(sql), &stmt, NULL );
    if (rc!=SQLITE_OK){
            pthread_mutex_unlock(&mutex);
        return false;
    }
    sqlite3_bind_text(stmt, 1, task.name.c_str(),task.name.size(),NULL);
    sqlite3_bind_int(stmt, 2, task.priority);
    sqlite3_bind_text(stmt, 3, task.base_dir.c_str(),task.base_dir.size(),NULL);
    sqlite3_bind_int(stmt, 4, task.status);
    sqlite3_bind_int(stmt, 5, task.core_num);
    sqlite3_bind_text(stmt, 6, task.created_at.c_str(),task.created_at.size(),NULL);
    sqlite3_bind_text(stmt, 7, task.finished_at.c_str(),task.finished_at.size(),NULL);
    sqlite3_bind_text(stmt, 8, task.username.c_str(),task.username.size(),NULL);
    rc = sqlite3_step( stmt );
    int count=sqlite3_changes(_db);
    pthread_mutex_unlock(&mutex);
    if (( rc != SQLITE_DONE )&&( rc != SQLITE_ROW )) return false;
    sqlite3_finalize( stmt );
    return count>0;
}


bool db::update_status(int id, int n_status, int o_status, const char * msg) {
        pthread_mutex_lock(&mutex);
        int rc;
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(_db,
                            "UPDATE tasks set status = ?1,msg=?5,finished_at=?2 where id= ?3 AND status= ?4 ", 
                            78, &stmt, NULL);
    if (rc != SQLITE_OK) {
            pthread_mutex_unlock(&mutex);
        return false;
    }
    char buffer [128]={0};
    if (n_status==Done||n_status==Error){
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime (buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
    }

    sqlite3_bind_int(stmt, 1,n_status);
    sqlite3_bind_text(stmt,2,buffer,strlen(buffer),NULL);
    sqlite3_bind_text(stmt,5,msg,strlen(msg),NULL);
    sqlite3_bind_int(stmt, 3,id);
    sqlite3_bind_int(stmt, 4,o_status);
    int count;
    rc = sqlite3_step(stmt);
    count=sqlite3_changes(_db);
    pthread_mutex_unlock(&mutex);
        if ((rc != SQLITE_DONE) && (rc != SQLITE_ROW)) return false;
    sqlite3_finalize(stmt);
    return count>0;
}

bool db::clear() {
        pthread_mutex_lock(&mutex);
        int rc;
    char *zErrMsg = 0;
    const  char *sql="UPDATE tasks set status = 0 where status= 1";
    rc= sqlite3_exec(_db,sql,NULL,NULL,&zErrMsg);
    if (rc!=SQLITE_OK){
        fprintf(stderr, "sqlite error :%s",zErrMsg);
        return false;
    }
    pthread_mutex_unlock(&mutex);
    return true;
}

void db::get_wait_tasks(vector<Task> &tasks) {
    //"SELECT id, task_name, priority, core_num,base_dir,status,created_at,finished_at,username,msg FROM tasks where status==0 limit 1"
            pthread_mutex_lock(&mutex);
    
    int rc;
    sqlite3_stmt *stmt = NULL;
    const char * sql="SELECT id, task_name, priority, core_num,base_dir,status,created_at,finished_at,username,msg FROM tasks where status==?1";
    rc=sqlite3_prepare_v2( _db,sql, strlen(sql), &stmt, NULL );
    sqlite3_bind_int(stmt, 1,Wait);
    while (sqlite3_step(stmt)==SQLITE_ROW){
        Task task;
        read_row(stmt,task);
        tasks.push_back(task);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&mutex);
    if (rc!=SQLITE_OK){
        return ;
    }
}

void db::read_row(sqlite3_stmt *stmt, Task &task) {
    const  char *temp;
    task.id= sqlite3_column_int(stmt,0);
    task.name=(const char *)sqlite3_column_text(stmt,1);
    task.priority=sqlite3_column_int(stmt,2);
    task.core_num=sqlite3_column_int(stmt,3);
    task.base_dir=(const char*)sqlite3_column_text(stmt,4);
    task.status=(TaskStatus)sqlite3_column_int(stmt,5);
    temp=(const char*)sqlite3_column_text(stmt,6);
    if (temp){
        task.created_at=temp;
    }
    temp=(const char*)sqlite3_column_text(stmt,7);
    if (temp){
        task.finished_at=temp;
    }
    temp=(const char*)sqlite3_column_text(stmt,8);
    if (temp){
        task.username=temp;
    }
    temp=(const char*)sqlite3_column_text(stmt,9);
    if (temp){
        task.msg=temp;
    }
}

bool db::get_wait_task(Task &task) {
            pthread_mutex_lock(&mutex);
    
    int rc;
    sqlite3_stmt *stmt = NULL;
    const  char * sql="SELECT id, task_name, priority, core_num,base_dir,status,created_at,finished_at,username,msg FROM tasks where status==?1 limit 1";
    rc=sqlite3_prepare_v2( _db,sql, strlen(sql), &stmt, NULL );
    sqlite3_bind_int(stmt, 1,Wait);
    rc = sqlite3_step(stmt);
    if (rc==SQLITE_ROW){
        read_row(stmt,task);
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&mutex);
        return true;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&mutex);

    return false;
}

bool db::doing(int id) {
    return update_status(id,Doing,Wait,"");

}

bool db::do_error(int id, const char *msg) {
    return update_status(id,Error,Doing,msg);
}

bool db::finished(int id) {
    return update_status(id,Done,Doing,"");
}

void db::get_all(int from, int to, vector<Task> &tasks) {
            pthread_mutex_lock(&mutex);
    
    int rc;
    sqlite3_stmt *stmt = NULL;
    const char *sql="SELECT id, task_name, priority, core_num,base_dir,status,created_at,finished_at,username,msg FROM tasks limit ?1,?2";
    rc=sqlite3_prepare_v2( _db,sql , strlen(sql), &stmt, NULL );
    sqlite3_bind_int(stmt, 1,from);
    sqlite3_bind_int(stmt, 2,to);
    while (sqlite3_step(stmt)==SQLITE_ROW){
        Task task;
        read_row(stmt,task);
        tasks.push_back(task);
    }
    pthread_mutex_unlock(&mutex);

    sqlite3_finalize(stmt);

}

bool db::query(const char *name, Task &task) {
           pthread_mutex_lock(&mutex);
        int rc;
    sqlite3_stmt *stmt = NULL;
    const char *sql="SELECT id, task_name, priority, core_num,base_dir,status,created_at,finished_at,username,msg FROM tasks where task_name==?1 limit 1";
    rc=sqlite3_prepare_v2( _db,sql, strlen(sql), &stmt, NULL );
    sqlite3_bind_text(stmt, 1,name,strlen(name),NULL);
    if (sqlite3_step(stmt)==SQLITE_ROW){
        read_row(stmt,task);
        pthread_mutex_unlock(&mutex);
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&mutex);

    return false;
}

bool db::reset(int id) {
        pthread_mutex_lock(&mutex);
        int rc;
    sqlite3_stmt *stmt = NULL;
    const char *sql="UPDATE tasks set status = 0,msg='', finished_at='' where id= ?1";
    rc = sqlite3_prepare_v2(_db,sql,strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    sqlite3_bind_int(stmt, 1,id);
    int count;
    rc = sqlite3_step(stmt);
    count=sqlite3_changes(_db);
    pthread_mutex_unlock(&mutex);
        if ((rc != SQLITE_DONE) && (rc != SQLITE_ROW)) return false;
    sqlite3_finalize(stmt);
    return count>0;
}

bool db::remove(int id) {
    pthread_mutex_lock(&mutex);
    int rc;
    sqlite3_stmt *stmt = NULL;
    const char *sql = "DELETE  FROM tasks  where id= ?1 ";
    rc = sqlite3_prepare_v2(_db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    int count;
    rc = sqlite3_step(stmt);
    count = sqlite3_changes(_db);
    pthread_mutex_unlock(&mutex);
    if ((rc != SQLITE_DONE) && (rc != SQLITE_ROW)) return false;
    sqlite3_finalize(stmt);
    return count > 0;
}

