# XXProductLine

## Overview
**XXProductLine** is a modular application designed to handle various product configurations using Qt and transitioning from QMake to CMake. The project includes a dynamic menu system, API examples, and unit tests for each unique product feature. It demonstrates how to efficiently build and test product lines with different configurations using shared libraries and modern development practices, focusing on the software product line (SPL) approach.

---

## Features

- **Product Builder Design:** Implements a builder pattern for dynamically configuring product-specific features and menus.
- **Shared Libraries:** Unique assets are implemented as shared libraries for modular development.
- **Unit Testing:** Integrated unit tests for each unique asset using QtTest.
- **Dual Build System Support:** Compatible with both CMake and QMake, providing flexibility for modern and legacy build systems.
- **API Support:** Modular APIs for handling product-specific configurations.

---

## Prerequisites

- **Qt Framework:** Version 6.7.2 or higher.
- **CMake:** Version 3.16 or higher.
- **GCC/MinGW or MSVC:** Ensure you have a C++17-compliant compiler.
- **gRPC:** Version 1.40 or higher with C++ bindings.
- **Protocol Buffers:** Version 3.21 or higher.

### Installing Dependencies on Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y qt6-base-dev qt6-tools-dev cmake build-essential
sudo apt install -y libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc
```

### Installing Dependencies on Windows

1. Install Qt 6.7.2+ from [Qt website](https://www.qt.io/download)
2. Install CMake from [CMake website](https://cmake.org/download/)
3. Install gRPC using vcpkg:
   ```cmd
   vcpkg install grpc protobuf
   ```

### Installing Dependencies on macOS

```bash
brew install qt6 cmake grpc protobuf
```

---

## Folder Structure

```plaintext
XXProductLine/
├── XXProductLineApplication/  # Main application source code
│   ├── main.cpp               # Entry point of the application
│   ├── mainwindow.cpp         # Main window implementation
│   └── mainwindow.h           # Main window header
│   └── ProductLineAPI/        # API classes for different products
│   └── ProductBuilder/        # Builder classes for different products
│       ├── Product1Builder
│       ├── Product2Builder
│       ├── ...
├── XXProductLineTests/      # Unit tests for unique assets
│   ├── tst_uniqueasset1
│   ├── tst_uniqueasset2
│   └── ...
├── SharedAssets/            # [Static Library]
├── UniqueAsset1/            # [Shared Library] Implementation of UniqueAsset1
├── UniqueAsset2/            # [Shared Library] Implementation of UniqueAsset2
├── ...
```

---

## API and Unit Tests

## API Support

The project includes modular APIs to facilitate the addition and configuration of product-specific features. These APIs leverage **gRPC** for high-performance communication, utilizing **Protocol Buffers** for efficient data serialization. This design ensures seamless integration of new product builders and assets into the main application while maintaining flexibility, type safety, and extensibility.

## gRPC API Example

### C++ Client Example

```cpp
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include "productline_api.grpc.pb.h"

class ProductLineClient {
public:
    ProductLineClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(productline::ProductLineService::NewStub(channel)) {}

    // Calls a function on a specific instance
    std::string CallFunction(uint64_t instanceId, const std::string& functionName, const std::string& paramsJson) {
        productline::CallFunctionRequest request;
        productline::CallFunctionResponse response;
        grpc::ClientContext context;

        request.set_instance_id(instanceId);
        request.set_function_name(functionName);
        request.set_params_json(paramsJson);

        grpc::Status status = stub_->CallFunction(&context, request, &response);

        if (status.ok()) {
            return "Result: " + response.result_json();
        } else {
            return "RPC failed: " + status.error_message();
        }
    }

    // Gets the main instance
    std::string GetInstance() {
        productline::GetInstanceRequest request;
        productline::GetInstanceResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetInstance(&context, request, &response);

        if (status.ok()) {
            return "Instance ID: " + std::to_string(response.instance_id());
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

    // Test GetInstance
    std::cout << "Testing GetInstance: " << client.GetInstance() << std::endl;

    // Test CallFunction with test parameters
    std::cout << "Testing CallFunction: " 
              << client.CallFunction(0, "getFeature1", "{}") << std::endl;

    return 0;
}
```

### Starting the gRPC Server

```cpp
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include "productlineapi.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Create the main window
    MainWindow mainWindow;

    // Create the ProductLineAPI instance
    ProductLineAPI productLineAPI(&mainWindow);

    // Start the gRPC server (runs on port 1025 by default)
    productLineAPI.start();

    // Insert a new instance into the API
    IPLAsset* newAsset = new SomeIPLAssetImplementation(); // Assume SomeIPLAssetImplementation is a concrete implementation of IPLAsset
    productLineAPI.insertInstance(1, newAsset);

    // Call a function on the new instance (can also be called via gRPC)
    QJsonObject params;
    params["param1"] = "value1";
    QJsonObject result = productLineAPI.callFunction(1, "someFunction", params);
    qDebug() << "Function call result:" << result;

    // Get the main instance (can also be called via gRPC)
    QJsonObject mainInstance = productLineAPI.getInstance(QJsonObject());
    qDebug() << "Main instance:" << mainInstance;

    // The gRPC server will continue running
    // Stop the gRPC server when done
    // productLineAPI.stop();

    return a.exec();
}
```

### gRPC Service Definition

The gRPC API is defined using Protocol Buffers in `productline_api.proto`:

```protobuf
syntax = "proto3";

package productline;

service ProductLineService {
    rpc CallFunction(CallFunctionRequest) returns (CallFunctionResponse);
    rpc GetInstance(GetInstanceRequest) returns (GetInstanceResponse);
    rpc InsertInstance(InsertInstanceRequest) returns (InsertInstanceResponse);
}

message CallFunctionRequest {
    uint64 instance_id = 1;
    string function_name = 2;
    string params_json = 3;  // JSON-encoded parameters
}

message CallFunctionResponse {
    string result_json = 1;  // JSON-encoded result
    string error = 2;
}

message GetInstanceRequest {
    // Empty for now
}

message GetInstanceResponse {
    uint64 instance_id = 1;
    string error = 2;
}
```

### Python Client Example

```python
import grpc
import json
from productline_api_pb2 import CallFunctionRequest, GetInstanceRequest
from productline_api_pb2_grpc import ProductLineServiceStub

def main():
    # Create a channel to the server
    channel = grpc.insecure_channel('localhost:1025')
    stub = ProductLineServiceStub(channel)
    
    # Test GetInstance
    request = GetInstanceRequest()
    response = stub.GetInstance(request)
    print(f"Instance ID: {response.instance_id}, Error: {response.error}")
    
    # Test CallFunction
    request = CallFunctionRequest()
    request.instance_id = 0
    request.function_name = "getFeature1"
    request.params_json = "{}"
    
    response = stub.CallFunction(request)
    print(f"Result: {response.result_json}, Error: {response.error}")

if __name__ == '__main__':
    main()
```

### Key Features

- **Real-Time Updates**: Instant synchronization between components and features.
- **Interoperability**: JSON's lightweight and human-readable format simplifies communication across diverse systems.
- **Scalability**: Easily extend APIs to support new product configurations or features.

### Technologies Used
- **QWebSocket**: For real-time communication.
- **JSON**: For data exchange between components and systems.


## Unit Testing

Unit tests are provided for each unique asset to ensure reliability and correctness. The tests are implemented using **QtTest**, a framework integrated with Qt for unit testing. The test cases are located in the `Tests/` directory. To verify functionality after changes, execute the tests using the `ctest` command.

---

## Software Product Line (SPL) Approach

### What is SPL?
A Software Product Line (SPL) approach focuses on building a family of products that share common features but differ in specific functionalities. This enables companies to reuse core components across multiple products, saving time and resources while maintaining high quality.

### Benefits of SPL in XXProductLine
- **Reduced Development Time:** Reuse of core components accelerates development.
- **Cost Efficiency:** Shared libraries and modular design lower costs.
- **Improved Quality:** Centralized testing ensures consistent reliability across all products.
- **Flexibility:** Easily add or modify features for specific products without affecting others.
- **Scalability:** Supports the growth of the product family with minimal overhead.

---

## Examples of Software Product Lines (SPL)

1. **Automotive Industry:**
   - A car manufacturer develops a family of vehicles (e.g., sedans, SUVs, and electric cars) using shared components like engines, chassis, and infotainment systems. Differences include vehicle-specific features such as the type of powertrain or luxury amenities.

2. **Smartphone Operating Systems:**
   - A mobile OS (e.g., Android) serves as a base for various devices. Manufacturers add custom user interfaces, pre-installed apps, and features to differentiate their products while sharing a common OS core.

3. **Home Automation Systems:**
   - A shared software platform manages smart devices like thermostats, security cameras, and lighting. Product configurations are customized for residential, commercial, or industrial use cases.

4. **E-Commerce Platforms:**
   - A single e-commerce engine powers multiple stores with variations in design, payment gateways, and inventory management, tailored for different industries or regions.

5. **Healthcare Systems:**
   - Hospital management software includes core functionalities like patient record management but customizes modules for specialties such as radiology, oncology, or pediatrics.

---

## Who Will Need Software Product Line?

1. **Product Manufacturers:**
   - Companies building families of products, such as smartphones, vehicles, or consumer electronics.

2. **Software Vendors:**
   - Businesses offering customizable platforms like CRMs, ERPs, or content management systems for different industries.

3. **Healthcare Providers:**
   - Institutions needing specialized software for different departments while maintaining a shared data ecosystem.

4. **Retail and E-Commerce Businesses:**
   - Organizations requiring customized storefronts and backend systems for specific markets or customer segments.

5. **IoT and Smart Device Companies:**
   - Developers of home automation systems, industrial IoT platforms, or smart city infrastructure that share a common software base.

---

## Usage

**XXProductLine** is designed to manage and implement software product lines efficiently. Here are some specific use cases:

1. **Dynamic Menu Configuration:**
   - Customize menus based on product requirements:
     - *Example:* A retail management system with separate menu items for inventory control, customer management, and sales reports.

2. **Modular Design:**
   - Reuse shared libraries across product families:
     - *Example:* A navigation system for different vehicles sharing a common map module but tailored for sedans, SUVs, or trucks.

3. **API Integration:**
   - Add new features dynamically using the product line API:
     - *Example:* An IoT system integrating device-specific controls for thermostats, cameras, or lights while sharing a central controller.

4. **Industry-Specific Adaptations:**
   - Develop products tailored for different industries:
     - *Example:* A hospital management software system with shared patient record functionality but specialized modules for radiology, surgery, or pediatrics.

5. **Testing and Quality Assurance:**
   - Test individual modules independently using the unit testing framework:
     - *Example:* Ensure the reliability of a billing module used across different e-commerce storefronts.

This flexibility and modularity make **XXProductLine** suitable for diverse industries, ensuring cost efficiency and high-quality software solutions.


---

## Build Instructions

### Clone the Repository

```bash
git clone https://github.com/yourusername/XXProductLine.git
cd XXProductLine
```

### Configure with CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="<QtInstallDir>/lib/cmake/Qt6"
```

### Build the Project

```bash
cmake --build .
```

### Run Unit Tests

```bash
ctest
```

---

## Contribution Guidelines

1. Fork the repository.
2. Create a feature branch.
3. Commit your changes with meaningful messages.
4. Submit a pull request.

---

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- **Qt Framework** for providing a robust foundation for GUI and application development.
- **CMake** for making project configuration and builds manageable.
- The open-source community for guidance and tools.

---

