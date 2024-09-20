if((ONNX_USE_LITE_PROTO AND TARGET protobuf::libprotobuf-lite) OR ((NOT ONNX_USE_LITE_PROTO) AND TARGET protobuf::libprotobuf))
  # Sometimes we need to use protoc compiled for host architecture while linking
  # libprotobuf against target architecture. See https://github.com/caffe2/caffe
  # 2/blob/96f35ad75480b25c1a23d6e9e97bccae9f7a7f9c/cmake/ProtoBuf.cmake#L92-L99
  if(EXISTS "${ONNX_CUSTOM_PROTOC_EXECUTABLE}")
    message(STATUS "Using custom protoc executable")
    set(ONNX_PROTOC_EXECUTABLE ${ONNX_CUSTOM_PROTOC_EXECUTABLE})
  else()
    set(ONNX_PROTOC_EXECUTABLE $<TARGET_FILE:protobuf::protoc>)
  endif()
else()
  # Customized version of find Protobuf. We need to avoid situations mentioned
  # in https://github.com/caffe2/caffe2/blob/b7d983f255ef5496474f1ea188edb5e0ac4
  # 42761/cmake/ProtoBuf.cmake#L82-L92 The following section is stolen from
  # cmake/ProtoBuf.cmake in Caffe2
  find_program(Protobuf_PROTOC_EXECUTABLE
               NAMES protoc
               DOC "The Google Protocol Buffers Compiler")

  # Only if protoc was found, seed the include directories and libraries. We
  # assume that protoc is installed at PREFIX/bin. We use get_filename_component
  # to resolve PREFIX.
  if(Protobuf_PROTOC_EXECUTABLE)
    set(ONNX_PROTOC_EXECUTABLE ${Protobuf_PROTOC_EXECUTABLE})
    get_filename_component(_PROTOBUF_INSTALL_PREFIX
                           ${Protobuf_PROTOC_EXECUTABLE} DIRECTORY)
    get_filename_component(_PROTOBUF_INSTALL_PREFIX
                           ${_PROTOBUF_INSTALL_PREFIX}/.. REALPATH)
    find_library(Protobuf_PROTOC_LIBRARY
                 NAMES protoc
                 PATHS ${_PROTOBUF_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
                 NO_DEFAULT_PATH)
    if(ONNX_USE_LITE_PROTO)
      find_library(Protobuf_LITE_LIBRARY
        NAMES protobuf-lite
        PATHS ${_PROTOBUF_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        NO_DEFAULT_PATH)
    else()
      find_library(Protobuf_LIBRARY
        NAMES protobuf
        PATHS ${_PROTOBUF_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        NO_DEFAULT_PATH)
    endif(ONNX_USE_LITE_PROTO)
    find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h
              PATHS ${_PROTOBUF_INSTALL_PREFIX}/include
              NO_DEFAULT_PATH)
    find_package(Protobuf)
    if (Protobuf_FOUND)
      set(PROTOBUF_DIR "${_PROTOBUF_INSTALL_PREFIX}")
      set(PROTOBUF_INCLUDE_DIR "${_PROTOBUF_INSTALL_PREFIX}/include")
      set(Build_Protobuf OFF)
      if ("${Protobuf_VERSION}" VERSION_GREATER_EQUAL "4.22.0")
        # There are extra dependencies for protobuf.
        find_package(absl REQUIRED)
        find_package(utf8_range REQUIRED)
        message(STATUS "absl_VERSION: ${absl_VERSION}")
        set(protobuf_ABSL_USED_TARGETS
          absl::absl_check
          absl::absl_log
          absl::algorithm
          absl::base
          absl::bind_front
          absl::bits
          absl::btree
          absl::cleanup
          absl::cord
          absl::core_headers
          absl::debugging
          absl::die_if_null
          absl::dynamic_annotations
          absl::flags
          absl::flat_hash_map
          absl::flat_hash_set
          absl::function_ref
          absl::hash
          absl::layout
          absl::log_initialize
          absl::log_severity
          absl::memory
          absl::node_hash_map
          absl::node_hash_set
          absl::optional
          absl::span
          absl::status
          absl::statusor
          absl::strings
          absl::synchronization
          absl::time
          absl::type_traits
          absl::utility
          absl::variant
          utf8_range::utf8_range
          utf8_range::utf8_validity
        )
      endif()
    else()
      set(Build_Protobuf ON)
    endif()
  else()  # Protobuf_PROTOC_EXECUTABLE not found.
    set(Build_Protobuf ON)
  endif()
  if (Build_Protobuf)
    # FetchContent module requires 3.11.
    cmake_minimum_required(VERSION 3.11)
    include(FetchContent)
    message("Loading Dependencies URLs ...")
    # Reference: https://github.com/abseil/abseil-cpp/releases/tag/20230125.3
    set(AbseilURL https://github.com/abseil/abseil-cpp/archive/refs/tags/20230125.3.tar.gz)
    set(AbseilSHA1 e21faa0de5afbbf8ee96398ef0ef812daf416ad8)
    FetchContent_Declare(
      Abseil
      URL ${AbseilURL}
      URL_HASH SHA1=${AbseilSHA1}
    )
    set(ABSL_PROPAGATE_CXX_STD 1)
    set(abseil_BUILD_TESTING 0)
    set(ONNX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    # Use this setting to build thirdparty libs.
    set(BUILD_SHARED_LIBS ${ONNX_USE_PROTOBUF_SHARED_LIBS})
    message(STATUS "Download and build Abseil from ${AbseilURL}")
    FetchContent_Populate(Abseil)
    FetchContent_GetProperties(Abseil)
    # ABSL_ROOT_DIR is required by Protobuf.
    set(ABSL_ROOT_DIR ${abseil_SOURCE_DIR})
    message(STATUS "Abseil source dir:" ${ABSL_ROOT_DIR})
    set(ProtobufURL https://github.com/protocolbuffers/protobuf/releases/download/v22.3/protobuf-22.3.tar.gz)
    set(ProtobufSHA1 310938afea334b98d7cf915b099ec5de5ae3b5c5)
    FetchContent_Declare(
      Protobuf
      URL ${ProtobufURL}
      URL_HASH SHA1=${ProtobufSHA1}
    )
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build protobuf tests" FORCE)
    message(STATUS "Download and build Protobuf from ${ProtobufURL}")
    FetchContent_MakeAvailable(Protobuf Abseil)
    set(ONNX_PROTOC_EXECUTABLE $<TARGET_FILE:protobuf::protoc>)
    set(Protobuf_VERSION "4.22.3")
    # Change back the BUILD_SHARED_LIBS to control the onnx project.
    set(BUILD_SHARED_LIBS ${ONNX_BUILD_SHARED_LIBS})
    set(PROTOBUF_DIR "${protobuf_BINARY_DIR}")
    set(PROTOBUF_INCLUDE_DIR "${protobuf_SOURCE_DIR}/src")
  endif()
  message(STATUS "ONNX_PROTOC_EXECUTABLE: ${ONNX_PROTOC_EXECUTABLE}")
  message(STATUS "Protobuf_VERSION: ${Protobuf_VERSION}")
endif()
