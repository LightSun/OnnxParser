
#SET(GRPC_DIR /home/heaven7/heaven7/libs/google/grpc/1.5.0_out)
SET(H7_SUPER_DIR /home/heaven7/heaven7/libs/google/protobuf/protobuf-22.3/_install)
SET(absl_DIR ${H7_SUPER_DIR}/lib/cmake/absl)
SET(protobuf_DIR ${H7_SUPER_DIR}/lib/cmake/protobuf)
SET(utf8_range_DIR ${H7_SUPER_DIR}/lib/cmake/utf8_range)

#option(ONNX_VERIFY_PROTO3 ON)

#SET(ONNX_CUSTOM_PROTOC_EXECUTABLE ${GRPC_DIR}/bin/protoc)
#include_directories(${GRPC_DIR}/include)
#SET(LIB_PROTOBUF ${GRPC_DIR}/lib/libprotobuf.a)

find_package(utf8_range)
find_package(absl)
find_package(protobuf)
