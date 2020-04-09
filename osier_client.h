//
// Created by 谢仲涛 on 2020/4/7.
//

#ifndef OSIER_CLI_OSIER_CLIENT_H
#define OSIER_CLI_OSIER_CLIENT_H

#include "osier_server.grpc.pb.h"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ChannelInterface;
using namespace std;
class osier_client  {
public:
    osier_client(int port){
        string target_str="127.0.0.1:";
        target_str.append(to_string(port));
        channel= grpc::CreateChannel(
                target_str, grpc::InsecureChannelCredentials());
        stub_ =OsierServer::NewStub(channel);
    }
    bool create(CreateRequest&request);
    bool list(ListRequest& request);
    bool detail(NameMessage& request);
    bool restart(NameMessage& request);
    bool remove(NameMessage& request);
    bool serverStaus(EmptyMessage& request);
private:
    std::unique_ptr<OsierServer::Stub> stub_;
    std::shared_ptr<ChannelInterface> channel;
};


#endif //OSIER_CLI_OSIER_CLIENT_H
