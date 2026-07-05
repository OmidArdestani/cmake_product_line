# libs/pl_proto — generated gRPC/protobuf code for all product-line contracts.
#
# All .proto files under <root>/proto are compiled here unconditionally:
# codegen is cheap and product selection stays a link-time decision.
# Consumers link `pl_proto` and get grpc++/protobuf transitively.
cmake_minimum_required(VERSION 3.16)
project(pl_proto LANGUAGES CXX)

set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(PROTO_FILES
    common.proto
    pl_core.proto
    unique_asset1.proto
    unique_asset2.proto
    unique_asset3.proto
    unique_asset4.proto
)

set(GENERATED_SOURCES)
foreach(PROTO_FILE IN LISTS PROTO_FILES)
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(GEN
        ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc
        ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
        ${PROTO_GEN_DIR}/${PROTO_NAME}.grpc.pb.cc
        ${PROTO_GEN_DIR}/${PROTO_NAME}.grpc.pb.h
    )
    add_custom_command(
        OUTPUT ${GEN}
        COMMAND $<TARGET_FILE:protobuf::protoc>
        ARGS
            --proto_path=${PROTO_SRC_DIR}
            --cpp_out=${PROTO_GEN_DIR}
            --grpc_out=${PROTO_GEN_DIR}
            --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
            ${PROTO_SRC_DIR}/${PROTO_FILE}
        DEPENDS ${PROTO_SRC_DIR}/${PROTO_FILE} protobuf::protoc grpc_cpp_plugin
        COMMENT "Generating protobuf/gRPC sources for ${PROTO_FILE}"
        VERBATIM
    )
    list(APPEND GENERATED_SOURCES ${GEN})
endforeach()

# SHARED is required: asset libraries are SHARED and each links pl_proto.
# A static pl_proto would embed a copy of gRPC/absl globals into every
# dylib, causing duplicate-registration aborts at process start.
add_library(pl_proto SHARED ${GENERATED_SOURCES})

target_include_directories(pl_proto PUBLIC ${PROTO_GEN_DIR})

target_link_libraries(pl_proto PUBLIC
    grpc++
    libprotobuf
)

set_target_properties(pl_proto PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
