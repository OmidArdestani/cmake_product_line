# XXProductLine

## Overview
**XXProductLine** is a modular application designed to handle various product configurations using Qt and transitioning from QMake to CMake. The project includes a dynamic menu system, API examples, and unit tests for each unique product feature. It demonstrates how to efficiently build and test product lines with different configurations using shared libraries and modern development practices, focusing on the software product line (SPL) approach.

---

## Features

- **Product Builder Design:** Implements a builder pattern for dynamically configuring product-specific features and menus.
- **Shared Libraries:** Unique assets are implemented as shared libraries for modular development.
- **Unit Testing:** GoogleTest test suite covering all assets.
- **CMake-only Build System:** Single CMake tree; QMake support removed.
- **API Support:** Modular WebSocket-based APIs for product-specific configurations.

---

## Prerequisites

- **Qt Framework:** Version 6.7.2 or higher (Qt5 supported).
- **CMake:** Version 3.16 or higher.
- **GCC/MinGW or MSVC:** C++17-compliant compiler.
- **Git:** With submodule support (GoogleTest vendored under `external/gtest`).

---

## Getting Started

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/<org>/cmake_product_line.git
cd cmake_product_line

# Configure for a specific product (default: Product_1)
cmake -B build -DCURRENT_PRODUCT=Product_1

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Launch the application
./build/bin/XXProductLineApplication
```

Available product variants: `Product_1` … `Product_5`

---

## Folder Structure

```plaintext
.
├── app/                        # Qt GUI application (XXProductLineApplication)
│   ├── main.cpp
│   ├── mainwindow.{h,cpp,ui}
│   ├── ProductBuilder/         # Product1Builder … Product5Builder (SPL variant selection)
│   └── ProductLineAPI/         # WebSocket-based runtime API
├── cmake/
│   └── ProductConfig.cmake     # Maps CURRENT_PRODUCT → asset list + compile definitions
├── docs/                       # Extended documentation
├── external/
│   └── gtest/                  # GoogleTest (git submodule)
├── include/                    # Reserved for project-wide public headers
├── libs/
│   ├── pl_core/                # Static library: shared base (IPLAsset, SharedAssets)
│   │   ├── include/pl_core/
│   │   └── src/
│   ├── unique_asset1/          # Shared library: UniqueAsset1
│   │   ├── include/
│   │   └── src/
│   ├── unique_asset2/ … unique_asset4/   # Same structure
└── tests/                      # GoogleTest suite (one file per library)
    ├── test_pl_core.cpp
    ├── test_unique_asset1.cpp … test_unique_asset4.cpp
    └── CMakeLists.txt
```

---

## API and Unit Tests

## API Support

The project includes modular APIs to facilitate the addition and configuration of product-specific features. These APIs leverage **QWebSocket** for real-time communication, utilizing **JSON** as the data exchange format. This design ensures seamless integration of new product builders and assets into the main application while maintaining flexibility and extensibility.

## API Example

```bash
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

    // Start the WebSocket server
    productLineAPI.start();

    // Insert a new instance into the API
    IPLAsset* newAsset = new SomeIPLAssetImplementation(); // Assume SomeIPLAssetImplementation is a concrete implementation of IPLAsset
    productLineAPI.insertInstance(1, newAsset);

    // Call a function on the new instance
    QJsonObject params;
    params["param1"] = "value1";
    QJsonObject result = productLineAPI.callFunction(1, "someFunction", params);
    qDebug() << "Function call result:" << result;

    // Get the main instance
    QJsonObject mainInstance = productLineAPI.getInstance(QJsonObject());
    qDebug() << "Main instance:" << mainInstance;

    // Stop the WebSocket server
    productLineAPI.stop();

    return a.exec();
}
```

### Key Features

- **Real-Time Updates**: Instant synchronization between components and features.
- **Interoperability**: JSON's lightweight and human-readable format simplifies communication across diverse systems.
- **Scalability**: Easily extend APIs to support new product configurations or features.

### Technologies Used
- **QWebSocket**: For real-time communication.
- **JSON**: For data exchange between components and systems.


## Unit Testing

Unit tests cover each library using **GoogleTest** (vendored as a submodule at `external/gtest`).
Tests are located in `tests/` and built automatically when `BUILD_TESTING=ON` (default).

```bash
cmake --build build --target XXProductLineTests
ctest --test-dir build --output-on-failure
```

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

