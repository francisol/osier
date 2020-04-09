//
// Created by 谢仲涛 on 2020/4/7.
//

#include "osier_client.h"
using namespace std;
bool osier_client::create(CreateRequest &request) {
    ClientContext context;
    SimpleResponse response;
    Status status = stub_->create(&context, request, &response);
    if (status.ok()){
        if(response.status()){
            cout<<"create success"<<endl;
        } else{
            cout<< response.msg() <<endl;
        }
    } else{
        cout<<" error:  "<< status.error_message() <<endl;
    }
    return true;
}

bool osier_client::list(ListRequest& request) {
    ClientContext context;
    ListResponse response;
    Status status = stub_->list(&context, request, &response);
    if (!status.ok()) {
        cout << " error:  " << status.error_message() << endl;
        return false;
    }
    if (!response.status()) {
        cout << response.msg() << endl;
        return false;
    }
    auto item= response.data().begin();
    cout<<"id\ttask_name\tusername\tbase_dir\tstatus\tmsg"<<endl;
    while (item!=response.data().end()){

        cout<< item->id()<<"\t"<<item->name()<<"\t"<<item->username()<<"\t"<<item->base_dir()<<"\t"<<TaskMessage_TaskStatus_Name(item->status())<<"\t"<<item->msg()<<endl;
        item++;
    }
    return true;
}

bool osier_client::detail(NameMessage &request) {
    ClientContext context;
    DetailResponse response;
    Status status = stub_->detail(&context, request, &response);
    if (!status.ok()) {
        cout << " error:  " << status.error_message() << endl;
        return false;
    }
    if (!response.status()) {
        cout << response.msg() << endl;
        return false;
    }
    auto  &task=response.data();
    cout << "id          : "<< task.id()<<endl;
    cout << "task_name   : "<< task.name()<<endl;
    cout << "base_dir    : "<< task.base_dir()<<endl;
    cout << "core_num    : "<< task.core_num()<<endl;
    cout << "priority    : "<< task.priority()<<endl;
    cout << "status      : "<< TaskMessage_TaskStatus_Name(task.status())<<endl;
    cout << "created_at  : "<< task.created_at()<<endl;
    cout << "finished_at : "<< task.finished_at()<<endl;
    cout << "msg         : "<< task.msg()<<endl;
    return true;
}

bool osier_client::restart(NameMessage &request) {
    ClientContext context;
    SimpleResponse response;
    Status status = stub_->restart(&context, request, &response);
    if (!status.ok()) {
        cout << " error:  " << status.error_message() << endl;
        return false;
    }
    if (!response.status()) {
        cout << response.msg() << endl;
        return false;
    }
    cout << "restart success" << endl;
    return true;

}

bool osier_client::remove(NameMessage &request) {
    ClientContext context;
    SimpleResponse response;
    Status status = stub_->remove(&context, request, &response);
    if (!status.ok()) {
        cout << " error:  " << status.error_message() << endl;
        return false;
    }
    if (!response.status()) {
        cout << response.msg() << endl;
        return false;
    }
    cout << "remove success" << endl;
    return true;
}

bool osier_client::serverStaus(EmptyMessage &request) {
    ClientContext context;
    ServerStatusResponse response;
    Status status = stub_->serverStatus(&context, request, &response);
    if (!status.ok()) {
        cout << " error:  " << status.error_message() << endl;
        return false;
    }
    cout << "          server status" << endl;
    cout << "core_num     : "<< response.core_num()<<endl;
    cout << "current_num  : "<< response.current_num()<<endl;
    cout << "current_task : "<<response.current_task()<<endl;
    return true;
}


