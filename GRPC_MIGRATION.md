# gRPC Migration for XXProductLine

This document describes the migration from WebSocket JSON API to gRPC API.

## Changes Made

### API Protocol Change
- **Before**: WebSocket with JSON messages
- **After**: gRPC with Protocol Buffers

### Key Benefits of gRPC
1. **Type Safety**: Strongly typed interface with Protocol Buffers
2. **Performance**: Binary serialization is faster than JSON
3. **Cross-Language Support**: Generate clients for Python, Java, Go, etc.
4. **HTTP/2**: Better connection multiplexing and flow control
5. **Built-in Security**: TLS support out of the box

### API Interface Comparison

#### WebSocket (Old)
```json
{
  "instanceId": "0",
  "method": "getFeature1", 
  "params": {}
}
```

#### gRPC (New)
```protobuf
message CallFunctionRequest {
    uint64 instance_id = 1;
    string function_name = 2;
    string params_json = 3;
}
```

### Files Modified
- `ProductLineAPI/productlineapi.h` - Updated to use GrpcAPI
- `ProductLineAPI/productlineapi.cpp` - Replaced WebSocketsAPI with GrpcAPI
- `CMakeLists.txt` - Added gRPC dependencies
- `XXProductLineApplication.pro` - Added gRPC dependencies

### Files Added
- `ProductLineAPI/productline_api.proto` - gRPC service definition
- `ProductLineAPI/GrpcAPI.h` - gRPC server implementation
- `ProductLineAPI/grpcapi.cpp` - gRPC server implementation
- Generated protobuf files (.pb.h, .pb.cc, .grpc.pb.h, .grpc.pb.cc)

## Testing the Migration

### 1. Build the Project
```bash
cd cmake/build
cmake ..
make
```

### 2. Run the Application
```bash
./XXProductLineApplication/XXProductLineApplication
```

### 3. Test with C++ Client
```bash
cd /tmp
g++ -std=c++17 -I. $(pkg-config --cflags grpc++ protobuf) \
    grpc_test_client.cpp productline_api.grpc.pb.cc productline_api.pb.cc \
    $(pkg-config --libs grpc++ protobuf) -o grpc_test_client
./grpc_test_client
```

### 4. Test with Python Client
```bash
cd /tmp
python3 python_grpc_client.py
```

## Backward Compatibility

This is a breaking change. WebSocket clients will no longer work and must be updated to use gRPC.

## Future Enhancements

1. Add TLS/SSL support for secure communication
2. Add authentication and authorization
3. Add streaming RPCs for real-time data
4. Add health checking service
5. Add metrics and monitoring