#include <iostream>
#include "core/src/OnnxParser.h"

using namespace h7_onnx;

extern void test_modify_model_shape();

int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);
#if 0
    test_modify_model_shape();
    return 0;
#endif

    String file = "/media/heaven7/Elements_SE/study/work/MedQA/module/onnx3/"
                  "sec_det/1219_neck_short_jindongmai_det.onnx";
    if(argc > 1){
        file = argv[1];
    }
    //xx --modify_weights 0.2 <in_onnx> <out_onnx>
    if(file == "--modify_weights"){
        if(argc < 5){
            fprintf(stderr, "modify_weights >> format must be like"
                            " '<exe> --modify_weights 0.2 <in_onnx> <out_onnx>'.\n");
            return -1;
        }
        float val = std::stof(argv[2]);
        return h7_onnx::modifyOnnxWeights(argv[3], argv[4], val);
    }

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
    c = parser.getOutputTensorCount();
    for(int i = 0 ; i < c ; ++i){
        auto name = parser.getOutputName(i);
        auto dims = parser.getOutputDims(i);
        auto dimStr = formatDims(dims);
        printf("output: (name, dim) = (%s, %s)\n", name.data(), dimStr.data());
    }
    return 0;
}
