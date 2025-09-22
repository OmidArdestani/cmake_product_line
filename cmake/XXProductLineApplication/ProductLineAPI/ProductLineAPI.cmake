# Define headers and sources for ProductLineAPI
set(PRODUCT_LINE_API_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productlineapi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/GrpcAPI.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productline_api.grpc.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productline_api.pb.h
)

set(PRODUCT_LINE_API_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productlineapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/grpcapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productline_api.grpc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI/productline_api.pb.cc
)

# Optionally, add include directories if needed
# This can be handled when including this file in the main CMakeLists.txt
