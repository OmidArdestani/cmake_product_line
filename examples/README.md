# gRPC Examples

This directory contains example clients for testing the ProductLine gRPC API.

## C++ Client Example

Compile and run:
```bash
g++ -std=c++17 -I../cmake/XXProductLineApplication/ProductLineAPI \
    $(pkg-config --cflags grpc++ protobuf) \
    cpp_client.cpp \
    ../cmake/XXProductLineApplication/ProductLineAPI/productline_api.grpc.pb.cc \
    ../cmake/XXProductLineApplication/ProductLineAPI/productline_api.pb.cc \
    $(pkg-config --libs grpc++ protobuf) -o cpp_client
./cpp_client
```

## Python Client Example

Generate Python gRPC files:
```bash
python3 -m grpc_tools.protoc -I../cmake/XXProductLineApplication/ProductLineAPI \
    --python_out=. --grpc_python_out=. \
    ../cmake/XXProductLineApplication/ProductLineAPI/productline_api.proto
```

Run the client:
```bash
pip3 install grpcio grpcio-tools
python3 python_client.py
```

## Prerequisites

1. Start the XXProductLineApplication
2. Enable the API by clicking "Run" in the application menu
3. The gRPC server will be running on localhost:1025