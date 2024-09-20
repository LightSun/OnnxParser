#include <onnx/onnx_pb.h>
#include <unordered_map>
#include <algorithm>
#include "OnnxParser.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "onnxProtoUtils.hpp"


using namespace h7_onnx;

namespace h7_onnx {

class NamedDimension
{
public:
    //nvinfer1::ITensor* tensor;
    int32_t index;

    std::string dimParam;

    NamedDimension(int32_t index_, std::string const& dimParam_)
        :
          //tensor(nullptr),
          index(index_)
        , dimParam(dimParam_)
    {
    }
};
struct TensorItem{
    String name;
    nvinfer1::Dims dims;

    void setDims(const std::vector<int32_t>& ds){
        dims.nbDims = ds.size();
        std::copy(ds.begin(), ds.end(), dims.d);
    }
    std::vector<int> getDimsVec(){
        std::vector<int> vec;
        vec.resize(dims.nbDims);
        memcpy(vec.data(), dims.d, sizeof(int64_t) * dims.nbDims);
        return vec;
    }
};
struct OnnxParserCtx0{
    ::ONNX_NAMESPACE::ModelProto model;
    List<TensorItem> inputItems;
    List<TensorItem> outputItems;
    std::vector<onnx2trt::Status> mErrors;

    std::vector<int> getInputDims(int idx){
        if(idx >= (int)inputItems.size()){
            return {};
        }
        return inputItems[idx].getDimsVec();
    }
    std::vector<int> getOutputDims(int idx){
        if(idx >= (int)outputItems.size()){
            return {};
        }
        return outputItems[idx].getDimsVec();
    }
    onnx2trt::Status importModel(){
        inputItems.clear();
        outputItems.clear();
        ::ONNX_NAMESPACE::GraphProto const& graph = model.graph();
        {
            std::vector<NamedDimension> namedDims;
            for (::ONNX_NAMESPACE::ValueInfoProto const& output : graph.output())
            {
                TensorItem item;
                importImpl(namedDims, item, output);
                outputItems.push_back(item);
            }
        }
        {
            std::vector<NamedDimension> namedDims;
            for (::ONNX_NAMESPACE::ValueInfoProto const& input : graph.input())
            {
                TensorItem item;
                importImpl(namedDims, item, input);
                inputItems.push_back(item);
            }
        }
        return onnx2trt::Status::success();
    }
private:
    void importImpl(std::vector<NamedDimension>& namedDims,TensorItem& item,
                    ::ONNX_NAMESPACE::ValueInfoProto const& input){
        item.name = input.name();
        auto const& onnxDtype = input.type().tensor_type();
        auto& onnxDims = onnxDtype.shape().dim();
        std::vector<int32_t> onnxDimsVec;
        for (auto const& onnxDim : onnxDims)
        {
            // For empty dimensions, the ONNX specification says it's a dynamic dimension
            if (!onnxDim.has_dim_value() && !onnxDim.has_dim_param())
            {
                onnxDimsVec.emplace_back(-1);
            }
            else
            {
                if (!onnxDim.dim_param().empty())
                {
                    namedDims.emplace_back(static_cast<int32_t>(onnxDimsVec.size()), onnxDim.dim_param());
                }
                const int32_t dim = onnxDim.dim_param() == "" ? (onnxDim.dim_value() >= 0 ? onnxDim.dim_value() : -1) : -1;
                onnxDimsVec.emplace_back(dim);
            }
        }
        item.setDims(onnxDimsVec);
    }
};
}

OnnxParser::OnnxParser(){
    m_ctx = new OnnxParserCtx0();
}
OnnxParser::~OnnxParser(){
    if(m_ctx){
        delete m_ctx;
        m_ctx = nullptr;
    }
}
bool OnnxParser::parseFromFile(CString filename){
    std::ifstream onnxFile(filename, std::ios::ate | std::ios::binary);
    if (!onnxFile){
       std::cerr << "Could not open file " << std::string(filename) << std::endl;
       return false;
    }
    auto fileSize = onnxFile.tellg();
    onnxFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    if (!onnxFile.read(buffer.data(), fileSize))
    {
       std::cerr << "Error reading file: " << filename << std::endl;
       return false;
    }
    return parseFromData(buffer.data(), buffer.size());
}

bool OnnxParser::parseFromData(const void* data, size_t len){
    onnx2trt::Status status
        = onnx2trt::deserializeOnnxModel(data, len, &m_ctx->model);
    if (status.is_error()){
       m_ctx->mErrors.push_back(status);
       return false;
    }
    status = m_ctx->importModel();
    if (status.is_error()){
       m_ctx->mErrors.push_back(status);
       return false;
    }
    return true;
}
std::vector<int> OnnxParser::getInputDims(int idx){
    return m_ctx->getInputDims(idx);
}
std::vector<int> OnnxParser::getOutputDims(int idx){
    return m_ctx->getOutputDims(idx);
}
int OnnxParser::getInputTensorCount(){
    return m_ctx->inputItems.size();
}
int OnnxParser::getOutputTensorCount(){
    return m_ctx->outputItems.size();
}
String OnnxParser::getInputName(int idx){
    return m_ctx->inputItems[idx].name;
}
String OnnxParser::getOutputName(int idx){
    return m_ctx->outputItems[idx].name;
}
