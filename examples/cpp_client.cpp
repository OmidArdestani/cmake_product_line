#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include "productline_api.grpc.pb.h"

class ProductLineClient {
public:
    ProductLineClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(productline::ProductLineService::NewStub(channel)) {}

    // Calls GetInstance to test basic connectivity
    std::string GetInstance() {
        productline::GetInstanceRequest request;
        productline::GetInstanceResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetInstance(&context, request, &response);

        if (status.ok()) {
            return "Instance ID: " + std::to_string(response.instance_id()) + 
                   ", Error: " + response.error();
        } else {
            return "RPC failed: " + status.error_message();
        }
    }

    // Calls a function with test parameters
    std::string CallFunction(uint64_t instanceId, const std::string& functionName, const std::string& paramsJson) {
        productline::CallFunctionRequest request;
        productline::CallFunctionResponse response;
        grpc::ClientContext context;

        request.set_instance_id(instanceId);
        request.set_function_name(functionName);
        request.set_params_json(paramsJson);

        grpc::Status status = stub_->CallFunction(&context, request, &response);

        if (status.ok()) {
            return "Result: " + response.result_json() + 
                   ", Error: " + response.error();
        } else {
            return "RPC failed: " + status.error_message();
        }
    }

private:
    std::unique_ptr<productline::ProductLineService::Stub> stub_;
};

int main(int argc, char** argv) {
    // Create a channel to the server
    std::string server_address("localhost:1025");
    ProductLineClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    std::cout << "Connecting to gRPC server at " << server_address << std::endl;

    // Test GetInstance
    std::cout << "Testing GetInstance: " << client.GetInstance() << std::endl;

    // Test CallFunction with test parameters
    std::cout << "Testing CallFunction: " 
              << client.CallFunction(0, "getFeature1", "{}") << std::endl;

    return 0;
}