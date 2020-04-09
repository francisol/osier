//
// Created by 谢仲涛 on 2020/4/7.
//

#include "osier_server.h"

::grpc::Status
osier_server::create(::grpc::ServerContext *context, const ::CreateRequest *request, ::SimpleResponse *response) {
    time_t rawtime;
    char buffer [128]={0};
    struct tm * timeinfo;

    time (&rawtime);

    timeinfo = localtime (&rawtime);
    strftime (buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
    Task task;
    task.username=request->username();
    task.name=request->name();
    task.base_dir=request->base_dir();
    task.core_num=request->core_num();
    task.priority=request->priority();
    task.created_at=buffer;
    task.status=Wait;
    try{
        if(!_db.save(task)){
            response->set_status(false);
        } else{
            _controller.refresh();
            response->set_status(true);
        }
    }catch (const std::exception& e){
        response->set_status(false);
        response->set_msg(e.what());

    }
    return ::grpc::Status::OK;
}

osier_server::osier_server(db &_db, controller &_controller):_db(_db),_controller(_controller) {

}

inline void convert_data(Task& from,::TaskMessage* to){
    to->set_id(from.id);
    to->set_name(from.name);
    to->set_base_dir(from.base_dir);
    to->set_priority(from.priority);
    to->set_username(from.username);
    to->set_core_num(from.core_num);
    to->set_created_at(from.created_at);
    to->set_finished_at(from.finished_at);
    switch (from.status){
        case Wait:to->set_status(TaskMessage_TaskStatus_Wait);break;
        case Doing:to->set_status(TaskMessage_TaskStatus_Doing);break;
        case Done:to->set_status(TaskMessage_TaskStatus_Done);break;
        case Error:to->set_status(TaskMessage_TaskStatus_Error);break;
    }
    to->set_msg(from.msg);
}

::grpc::Status
osier_server::list(::grpc::ServerContext *context, const ::ListRequest *request, ::ListResponse *response) {
    vector<Task> tasks;
   _db.get_all(request->start(),request->end(),tasks);
    response->set_status(true);
   for (auto &item:tasks){
       ::TaskMessage *t = response->add_data();
       convert_data(item,t);
   }
    return ::grpc::Status::OK;
}

::grpc::Status
osier_server::restart(::grpc::ServerContext *context, const ::NameMessage *request, ::SimpleResponse *response) {
    Task task;
   if(! _db.query(request->name().c_str(),task)){
       response->set_status(false);
       response->set_msg("task name don't exist");
       return ::grpc::Status::OK;
   }
   if(task.status==Doing){
       response->set_status(false);
       response->set_msg("task is running");
       return ::grpc::Status::OK;
   }
    (_db.reset(task.id));
        response->set_status(true);
        _controller.refresh();
    return ::grpc::Status::OK;
}

::grpc::Status
osier_server::remove(::grpc::ServerContext *context, const ::NameMessage *request, ::SimpleResponse *response) {
    Task task;
    if(! _db.query(request->name().c_str(),task)){
        response->set_status(false);
        response->set_msg("task name don't exist");
        return ::grpc::Status::OK;
    }
    if(task.status==Doing){
        response->set_status(false);
        response->set_msg("task is running");
        return ::grpc::Status::OK;
    }
    _db.remove(task.id);
    response->set_status(true);
    return ::grpc::Status::OK;
}

::grpc::Status
osier_server::detail(::grpc::ServerContext *context, const ::NameMessage *request, ::DetailResponse *response) {
    Task task;
    if(! _db.query(request->name().c_str(),task)){
        response->set_status(false);
        response->set_msg("task name don't exist");
        return ::grpc::Status::OK;
    }
    response->set_status(true);
    ::TaskMessage *taskMessage=new TaskMessage();
    convert_data(task,taskMessage);
    response->set_allocated_data(taskMessage);
    return ::grpc::Status::OK;

}

::grpc::Status osier_server::serverStatus(::grpc::ServerContext *context, const ::EmptyMessage *request,
                                          ::ServerStatusResponse *response) {
    response->set_core_num(_controller.get_core_num());
    response->set_current_num(_controller.get_current_core());
    response->set_current_task(_controller.get_current_task());
    return ::grpc::Status::OK;
}


