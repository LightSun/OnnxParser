#include "OnnxParser.h"

#include <onnx/onnx_pb.h> // ONNX的Protobuf定义
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring> // for memcpy

// 辅助函数：将 ONNX 的 TensorProto 数据类型映射到 C++ 类型大小
static inline size_t getDataTypeSize(int data_type) {
    switch (data_type) {
    case onnx::TensorProto_DataType_FLOAT: return sizeof(float);
    case onnx::TensorProto_DataType_DOUBLE: return sizeof(double);
    case onnx::TensorProto_DataType_UINT32: return sizeof(uint32_t);
    case onnx::TensorProto_DataType_INT32: return sizeof(int32_t);
    case onnx::TensorProto_DataType_INT64: return sizeof(int64_t);
    case onnx::TensorProto_DataType_UINT64: return sizeof(uint64_t);
    case onnx::TensorProto_DataType_UINT8: return sizeof(uint8_t);
    case onnx::TensorProto_DataType_INT8: return sizeof(int8_t);
    case onnx::TensorProto_DataType_UINT16: return sizeof(uint16_t);
    case onnx::TensorProto_DataType_INT16: return sizeof(int16_t);
    case onnx::TensorProto_DataType_BFLOAT16: return sizeof(float) / 2;
    case onnx::TensorProto_DataType_BOOL: return sizeof(bool);
    // ... 根据你的需要添加更多类型
    default: return 1;
    }
}
namespace h7_onnx {

int modifyOnnxWeights(CString srcOnnx, CString dstOnnx,float val){
    // 1. 加载ONNX模型
    onnx::ModelProto model;
    std::fstream input(srcOnnx, std::ios::in | std::ios::binary);
    if (!model.ParseFromIstream(&input)) {
        std::cerr << "Failed to load ONNX model." << std::endl;
        return -1;
    }
    input.close();

    // 获取计算图
    onnx::GraphProto* graph = model.mutable_graph();

    for (int i = 0; i < graph->initializer_size(); ++i) {
        onnx::TensorProto* tensor = graph->mutable_initializer(i);
        std::cout << "tensor name: " << tensor->name() << std::endl;

        // 3. 反序列化权重数据到 vector<float>
        // 获取数据类型和总数据量
        auto data_type = tensor->data_type();
        size_t type_size = getDataTypeSize(data_type);
        if (type_size == 0) {
            std::cout << "[ignored] Unsupported data type." << std::endl;
            continue;
        }
        // 计算元素个数
        int64_t total_elements = 1;
        for (int j = 0; j < tensor->dims_size(); ++j) {
            total_elements *= tensor->dims(j);
        }
        // 从 raw_data 中读取数据
        const std::string& raw_data = tensor->raw_data();
        if (raw_data.size() != total_elements * type_size) {
            std::cout << "[ignored] Raw data size mismatch." << std::endl;
            continue;
        }

        // 假设权重是 float 类型
        std::vector<float> weights(total_elements);
        memcpy(weights.data(), raw_data.data(), raw_data.size());

        // --- 在这里修改权重，例如全部置零 ---
        std::fill(weights.begin(), weights.end(), val);

        // 4. 将修改后的数据序列化回 raw_data
        tensor->clear_raw_data();
        tensor->set_raw_data(weights.data(), weights.size() * sizeof(float));
    }

    // 5. 保存修改后的模型
    std::fstream output(dstOnnx, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!model.SerializeToOstream(&output)) {
        std::cerr << "Failed to save ONNX model." << std::endl;
        return -1;
    }
    output.close();
    std::cout << "Modified model ok" << std::endl;
    return 0;
}
}
