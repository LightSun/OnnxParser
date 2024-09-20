cmake_minimum_required(VERSION 3.5)

project(OnnxParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(cmake/ENV.cmake)

##

# Find protobuf if it's not a target.
if (NOT TARGET protobuf::libprotobuf)
  FIND_PACKAGE(Protobuf REQUIRED)
endif()

# Set protobuf libraries between full / lite.
if (ONNX_USE_LITE_PROTO)
  add_definitions("-DUSE_LITE_PROTOBUF=1")
  set(PROTOBUF_LIBRARY "protobuf::libprotobuf-lite")
else()
  set(PROTOBUF_LIBRARY "protobuf::libprotobuf")
endif()

# sub
add_subdirectory(third_party/onnx)
add_subdirectory(core/)

#add_executable(OnnxParser main.cpp)
