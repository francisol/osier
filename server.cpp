#include <iostream>
#include "CLI11.hpp"
#include "config.h"
#include <filesystem>
#include <dirent.h>
#include <unistd.h>
#include "db.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "process.h"
#include "controller.h"
#include "osier_server.h"
#include "controller.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <vector>
#include <memory>
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using namespace std;
int main(int argc, char** argv) {
    AppConfigInit(argc, argv);
    AppConfig &appConfig = getAppConfig();
    std::filesystem::path db_path{appConfig.base_dir};
    db_path.append("db");
    db db{db_path.c_str()};
    controller col{db};
    db.clear();
    col.run();
    std::string server_address("0.0.0.0:");
    server_address.append(std::to_string(appConfig.port));
    osier_server osierServer{db,col};

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&osierServer);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();

    return 0;
}
