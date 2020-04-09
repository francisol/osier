//
// Created by 谢仲涛 on 2020/4/7.
//

#ifndef OSIER_OSIER_SERVER_H
#define OSIER_OSIER_SERVER_H

#include "osier_server.grpc.pb.h"
#include "db.h"
#include "controller.h"

class osier_server : public OsierServer::Service{
public:
    osier_server(db &_db,controller &_controller);
    ::grpc::Status create(::grpc::ServerContext* context, const ::CreateRequest* request, ::SimpleResponse* response);
     ::grpc::Status list(::grpc::ServerContext* context, const ::ListRequest* request, ::ListResponse* response);
     ::grpc::Status restart(::grpc::ServerContext* context, const ::NameMessage* request, ::SimpleResponse* response);
     ::grpc::Status remove(::grpc::ServerContext* context, const ::NameMessage* request, ::SimpleResponse* response);
     ::grpc::Status detail(::grpc::ServerContext* context, const ::NameMessage* request, ::DetailResponse* response);
     ::grpc::Status serverStatus(::grpc::ServerContext* context, const ::EmptyMessage* request, ::ServerStatusResponse* response);
private:
    db &_db;
    controller &_controller;
};


#endif //OSIER_OSIER_SERVER_H
