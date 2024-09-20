#include <iostream>
#include "core/src/OnnxParser.h"

using namespace h7_onnx;

int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);
    String file = "/media/heaven7/Elements_SE/study/work/HxPoc/"
                  "modules/onnx/feature/bst/cls2/Birads_2cls_r50_0.88.onnx";
    OnnxParser parser;
    if(!parser.parseFromFile(file)){
        fprintf(stderr, "parseFromFile >> failed. %s\n", file.data());
        return 1;
    }
    int c = parser.getInputTensorCount();
    for(int i = 0 ; i < c ; ++i){
        auto name = parser.getInputName(i);
        auto dims = parser.getInputDims(i);
        auto dimStr = formatDims(dims);
        printf("input: (name, dim) = (%s, %s)\n", name.data(), dimStr.data());
    }
    return 0;
}
