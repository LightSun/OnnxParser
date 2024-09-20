

##lib
set(nvonnxparser_lib_name SimpleOnnxParser)
file(GLOB_RECURSE SRC_PARSER
    src/*.h
    src/*.hpp
    src/*.cpp
    src/*.c
    )

add_library(${nvonnxparser_lib_name} SHARED
    ${SRC_PARSER}
    )

target_include_directories(${nvonnxparser_lib_name} PUBLIC ${ONNX_INCLUDE_DIRS})
target_link_libraries(${nvonnxparser_lib_name} PUBLIC
    onnx_proto
    ${PROTOBUF_LIBRARY}
    ${LIB_PROTOBUF}
    )
set_target_properties(${nvonnxparser_lib_name} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${TRT_OUT_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${TRT_OUT_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${TRT_OUT_DIR}"
)

## test
file(GLOB_RECURSE SRC_PARSER_DEMO
    main/*.h
    main/*.hpp
    main/*.cpp
    main/*.c
    )
add_executable(test_OnnxParser
    ${SRC_PARSER_DEMO}
    )
target_link_libraries(test_OnnxParser PUBLIC ${nvonnxparser_lib_name})
