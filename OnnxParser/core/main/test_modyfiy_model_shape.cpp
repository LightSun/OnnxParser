#include <iostream>
#include <string>
#include <fstream>
#include <onnx/onnx_pb.h>  // ONNX protobuf 头文件
#include <onnx/shape_inference/implementation.h>
#include <onnx/checker.h>

//#include <onnx/helper.h>
//#include <onnx/optimizer/optimize.h>
#include "onnx/version_converter/convert.h"
#include "onnxoptimizer/optimize.h"

using namespace ONNX_NAMESPACE;

// 第一个函数：修改模型输入输出大小
void conv_model_input_size(ModelProto& converted_model, int64_t max_batch, int64_t size1) {
    // 修改输入大小
    for (int i = 0; i < converted_model.graph().input_size(); i++) {
        auto* node = converted_model.mutable_graph()->mutable_input(i);
        std::string node_name = node->name();

        if (node_name == "token_ids") {
            auto* shape = node->mutable_type()->mutable_tensor_type()->mutable_shape();
            if (shape->dim_size() >= 2) {
                shape->mutable_dim(0)->set_dim_value(max_batch);
                shape->mutable_dim(1)->set_dim_value(size1);
            }
        } else if (node_name == "attention_masks") {
            auto* shape = node->mutable_type()->mutable_tensor_type()->mutable_shape();
            if (shape->dim_size() >= 2) {
                shape->mutable_dim(0)->set_dim_value(max_batch);
                shape->mutable_dim(1)->set_dim_value(size1);
            }
        } else if (node_name == "labels") {
            auto* shape = node->mutable_type()->mutable_tensor_type()->mutable_shape();
            if (shape->dim_size() >= 2) {
                shape->mutable_dim(0)->set_dim_value(max_batch);
                shape->mutable_dim(1)->set_dim_value(size1);
            }
        }

        // 打印修改后的形状信息
        std::cout << "After changing input size: '";
        auto& tensor_type = node->type().tensor_type();
        if (tensor_type.has_shape()) {
            auto& shape = tensor_type.shape();
            for (int j = 0; j < shape.dim_size(); j++) {
                if (j > 0) std::cout << ", ";
                if (shape.dim(j).has_dim_value()) {
                    std::cout << shape.dim(j).dim_value();
                } else if (shape.dim(j).has_dim_param()) {
                    std::cout << shape.dim(j).dim_param();
                }
            }
        }
        std::cout << "'" << std::endl;
    }

    // 修改输出大小
    for (int i = 0; i < converted_model.graph().output_size(); i++) {
        auto* node = converted_model.mutable_graph()->mutable_output(i);
        std::string node_name = node->name();

        if (node_name == "1530" || node_name == "1531") {
            auto* shape = node->mutable_type()->mutable_tensor_type()->mutable_shape();
            if (shape->dim_size() >= 2) {
                shape->mutable_dim(0)->set_dim_value(max_batch);
                shape->mutable_dim(1)->set_dim_value(size1);
            }
        }
    }
}

// 第二个函数：创建新模块
bool create_new_module(const ModelProto& model, const std::string& dstPath) {
    // 创建新图
    GraphProto new_graph;
    new_graph.set_name("new_graph");

    // 复制所有节点
    for (int i = 0; i < model.graph().node_size(); i++) {
        auto* new_node = new_graph.add_node();
        new_node->CopyFrom(model.graph().node(i));
    }

    // 复制输入
    for (int i = 0; i < model.graph().input_size(); i++) {
        auto* new_input = new_graph.add_input();
        new_input->CopyFrom(model.graph().input(i));
    }

    // 复制输出
    for (int i = 0; i < model.graph().output_size(); i++) {
        auto* new_output = new_graph.add_output();
        new_output->CopyFrom(model.graph().output(i));
    }

    // 复制初始化器
    for (int i = 0; i < model.graph().initializer_size(); i++) {
        auto* new_initializer = new_graph.add_initializer();
        new_initializer->CopyFrom(model.graph().initializer(i));
    }

    // 创建新模型
    ModelProto new_model;
    new_model.mutable_graph()->CopyFrom(new_graph);
    new_model.set_producer_name(model.producer_name());
    new_model.set_ir_version(10);

    // 复制操作集导入
    for (int i = 0; i < model.opset_import_size(); i++) {
        auto* opset = new_model.add_opset_import();
        opset->CopyFrom(model.opset_import(i));
    }

    // 检查是否需要添加默认操作集
    bool has_default_opset = false;
    for (int i = 0; i < new_model.opset_import_size(); i++) {
        if (new_model.opset_import(i).domain().empty()) {
            has_default_opset = true;
            break;
        }
    }

    if (!has_default_opset) {
        auto* opset = new_model.add_opset_import();
        opset->set_domain("");
        opset->set_version(13);
    }

    // 修改输入尺寸
    conv_model_input_size(new_model, 1, 512);

    try {
        // 形状推理
        shape_inference::InferShapes(new_model);

        // 检查模型
        checker::check_model(new_model);

        // 简化模型
        // 注意：ONNX 的简化功能可能有不同版本，这里使用通用方法
        auto model_sim = optimization::OptimizeFixed(new_model,
                                                     optimization::GetAvailablePasses());

        // 保存模型
        std::ofstream output_file(dstPath, std::ios::binary);
        if (!output_file) {
            std::cerr << "Failed to open file for writing: " << dstPath << std::endl;
            return false;
        }

        if (!model_sim.SerializeToOstream(&output_file)) {
            std::cerr << "Failed to write model to file" << std::endl;
            return false;
        }

        output_file.close();
        std::cout << "Simplify success" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error during model processing: " << e.what() << std::endl;
        return false;
        // 如果简化失败，尝试保存原始模型
//        std::ofstream output_file(dstPath, std::ios::binary);
//        if (!output_file) {
//            std::cerr << "Failed to open file for writing: " << dstPath << std::endl;
//            return false;
//        }

//        if (!new_model.SerializeToOstream(&output_file)) {
//            std::cerr << "Failed to write model to file" << std::endl;
//            return false;
//        }

//        output_file.close();
//        std::cout << "Simplify failed, saved original model instead" << std::endl;
//        return true;
    }
}

// test_modify_model_shape: not test ok
void test_modify_model_shape() {
    using String = std::string;
    String dir = "/media/heaven7/Elements_SE/study/work/NLP/onnx";
    String input = dir + "/gdse_ft_20250228_roberta_find.onnx";
    String output = dir + "/gdse_ft_20250228_roberta_find_test.onnx";
    // 加载模型
    ModelProto model;
    std::ifstream input_file(input, std::ios::binary);
    if (!input_file) {
        std::cerr << "Failed to open input model file" << std::endl;
        return;
    }

    if (!model.ParseFromIstream(&input_file)) {
        std::cerr << "Failed to parse model" << std::endl;
        return;
    }
    input_file.close();
    int ird = model.ir_version();
    printf("ir_version: %d\n", ird);
    model = version_conversion::ConvertVersion(model, 13);

    // 创建新模块
    if (!create_new_module(model, output)) {
        std::cerr << "Failed to create new module" << std::endl;
        return;
    }
}
