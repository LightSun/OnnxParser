
##
SET(H7_SUPER_DIR /home/heaven7/heaven7/libs/google/protobuf/protobuf-22.3/_install)
SET(absl_DIR ${H7_SUPER_DIR}/lib/cmake/absl)
SET(protobuf_DIR ${H7_SUPER_DIR}/lib/cmake/protobuf)
SET(utf8_range_DIR ${H7_SUPER_DIR}/lib/cmake/utf8_range)

find_package(utf8_range)
find_package(absl)
find_package(protobuf)

#SET(GRPC_DIR /home/heaven7/heaven7/libs/google/grpc/1.5.0_out)
#SET(absl_DIR ${H7_SUPER_DIR}/lib/cmake/absl)
#SET(protobuf_DIR ${H7_SUPER_DIR}/lib/cmake/protobuf)

#find_package(absl)
#find_package(protobuf)
