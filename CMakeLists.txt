cmake_minimum_required(VERSION 3.15)
project(osier)
set(CMAKE_CXX_STANDARD 17)



find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
add_subdirectory(third_party)
include_directories(third_party/lua)
include_directories(third_party)

add_subdirectory(path)
add_subdirectory(lfs)
set(SERVER_LIBRARIES sqlite lua yaml-cpp lfs path gRPC::grpc++_reflection gRPC::grpc++)
set(CLI_LIBRARIES  yaml-cpp gRPC::grpc++_reflection gRPC::grpc++)
IF (WIN32)
    # nothing to do
ELSEIF (APPLE)

ELSEIF (UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ ${CMAKE_EXE_LINKER_FLAGS}")
    set(SERVER_LIBRARIES "stdc++fs ${SERVER_LIBRARIES}")
    set(CLI_LIBRARIES "stdc++fs ${CLI_LIBRARIES}")
ENDIF ()


file(GLOB PROTO_FILE *.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)
foreach(FIL ${PROTO_FILE})
    message(STATUS ${GRPC_CPP_PLUGIN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(FIL_PATH "${FIL}" PATH)

    list (APPEND gRPC_SRC_GILES ${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc)
    list (APPEND gRPC_HEADER_GILES ${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h)
    list (APPEND protobuf_SRC_GILES ${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc)
    list (APPEND protobuf_HEADER_GILES ${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h)
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h"

            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            -I ${FIL_PATH}
            ${ABS_FIL}
            DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE} gRPC::grpc_cpp_plugin
            COMMENT "Running C++ gRPC compiler on ${FIL}"
            VERBATIM )

endforeach()

set_source_files_properties("${gRPC_SRC_GILES} ${gRPC_HEADER_GILES} ${protobuf_SRC_GILES} ${protobuf_HEADER_GILES}" PROPERTIES GENERATED TRUE)

add_executable(osier-server
        server.cpp
        server_config.cpp
        db.cpp db.h
        lua_extend.c
        process.cpp
        controller.cpp
        runner.cpp
        interface_controller.cpp
        arg_scrpit.cpp
        osier_server.cpp
        ${gRPC_SRC_GILES}
        ${gRPC_HEADER_GILES}
        ${protobuf_SRC_GILES}
        ${protobuf_HEADER_GILES}
        )
target_link_libraries(osier-server  ${SERVER_LIBRARIES})

add_executable(osier-cli cli.cpp
        osier_client.cpp
        ${gRPC_SRC_GILES}
        ${gRPC_HEADER_GILES}
        ${protobuf_SRC_GILES}
        ${protobuf_HEADER_GILES}
        )
target_link_libraries(osier-cli ${CLI_LIBRARIES})