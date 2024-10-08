#pragma once

#include <string>
#include <vector>

namespace h7_onnx {

using CString = const std::string&;
using String = std::string;

template<typename T>
using List = std::vector<T>;

template<typename T>
using CList = const std::vector<T>&;

typedef struct OnnxParserCtx0 OnnxParserCtx0;

class OnnxParser{
public:
    OnnxParser();
    ~OnnxParser();

    bool parseFromFile(CString onnx);
    bool parseFromData(const void* data, size_t len);
    //
    int getInputTensorCount();
    int getOutputTensorCount();
    String getInputName(int idx);
    String getOutputName(int idx);
    std::vector<int64_t> getInputDims(int idx);
    std::vector<int64_t> getOutputDims(int idx);

private:
    OnnxParserCtx0* m_ctx {nullptr};
};

template<typename T>
static inline String formatDims(const std::vector<T>& vec, CString sep = "x"){
    String str;
    for(size_t i = 0 ; i < vec.size() ; ++i){
        str.append(std::to_string(vec[i]));
        if(i != vec.size() - 1){
            str.append(sep);
        }
    }
    return str;
}

}
