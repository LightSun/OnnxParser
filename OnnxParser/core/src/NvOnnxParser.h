#pragma once

#include <string>
#include <vector>

namespace nvinfer1 {

using int32_t = int;
using int64_t = long long;

class Dims64
{
public:
    //! The maximum rank (number of dimensions) supported for a tensor.
    static constexpr int32_t MAX_DIMS{8};

    //! The rank (number of dimensions).
    int32_t nbDims;

    //! The extent of each dimension.
    int64_t d[MAX_DIMS];
};

using Dims = Dims64;

struct Permutation
{
    //!
    //! The elements of the permutation.
    //! The permutation is applied as outputDimensionIndex = permutation.order[inputDimensionIndex], so to
    //! permute from CHW order to HWC order, the required permutation is [1, 2, 0], and to permute
    //! from HWC to CHW, the required permutation is [2, 0, 1].
    //!
    int32_t order[Dims::MAX_DIMS];
};

enum class DataType : int32_t
{
    //! 32-bit floating point format.
    kFLOAT = 0,

    //! IEEE 16-bit floating-point format -- has a 5 bit exponent and 11 bit significand.
    kHALF = 1,

    //! Signed 8-bit integer representing a quantized floating-point value.
    kINT8 = 2,

    //! Signed 32-bit integer format.
    kINT32 = 3,

    //! 8-bit boolean. 0 = false, 1 = true, other values undefined.
    kBOOL = 4,

    //! Unsigned 8-bit integer format.
    //! Cannot be used to represent quantized floating-point values.
    //! Use the IdentityLayer to convert kUINT8 network-level inputs to {kFLOAT, kHALF} prior
    //! to use with other TensorRT layers, or to convert intermediate output
    //! before kUINT8 network-level outputs from {kFLOAT, kHALF} to kUINT8.
    //! kUINT8 conversions are only supported for {kFLOAT, kHALF}.
    //! kUINT8 to {kFLOAT, kHALF} conversion will convert the integer values
    //! to equivalent floating point values.
    //! {kFLOAT, kHALF} to kUINT8 conversion will convert the floating point values
    //! to integer values by truncating towards zero. This conversion has undefined behavior for
    //! floating point values outside the range [0.0F, 256.0F) after truncation.
    //! kUINT8 conversions are not supported for {kINT8, kINT32, kBOOL}.
    kUINT8 = 5,

    //! Signed 8-bit floating point with
    //! 1 sign bit, 4 exponent bits, 3 mantissa bits, and exponent-bias 7.
    kFP8 = 6,

    //! Brain float -- has an 8 bit exponent and 8 bit significand.
    kBF16 = 7,

    //! Signed 64-bit integer type.
    kINT64 = 8,

    //! Signed 4-bit integer type.
    kINT4 = 9,
};

}

namespace nvonnxparser {
enum class ErrorCode : int
{
    kSUCCESS = 0,
    kINTERNAL_ERROR = 1,
    kMEM_ALLOC_FAILED = 2,
    kMODEL_DESERIALIZE_FAILED = 3,
    kINVALID_VALUE = 4,
    kINVALID_GRAPH = 5,
    kINVALID_NODE = 6,
    kUNSUPPORTED_GRAPH = 7,
    kUNSUPPORTED_NODE = 8,
    kUNSUPPORTED_NODE_ATTR = 9,
    kUNSUPPORTED_NODE_INPUT = 10,
    kUNSUPPORTED_NODE_DATATYPE = 11,
    kUNSUPPORTED_NODE_DYNAMIC = 12,
    kUNSUPPORTED_NODE_SHAPE = 13,
    kREFIT_FAILED = 14
};

class IParserError
{
public:
    //!
    //!\brief the error code.
    //!
    virtual ErrorCode code() const = 0;
    //!
    //!\brief description of the error.
    //!
    virtual char const* desc() const = 0;
    //!
    //!\brief source file in which the error occurred.
    //!
    virtual char const* file() const = 0;
    //!
    //!\brief source line at which the error occurred.
    //!
    virtual int line() const = 0;
    //!
    //!\brief source function in which the error occurred.
    //!
    virtual char const* func() const = 0;
    //!
    //!\brief index of the ONNX model node in which the error occurred.
    //!
    virtual int node() const = 0;
    //!
    //!\brief name of the node in which the error occurred.
    //!
    virtual char const* nodeName() const = 0;
    //!
    //!\brief name of the node operation in which the error occurred.
    //!
    virtual char const* nodeOperator() const = 0;
    //!
    //!\brief A list of the local function names, from the top level down, constituting the current
    //!             stack trace in which the error occurred. A top-level node that is not inside any
    //!             local function would return a nullptr.
    //!
    virtual char const* const* localFunctionStack() const = 0;
    //!
    //!\brief The size of the stack of local functions at the point where the error occurred.
    //!             A top-level node that is not inside any local function would correspond to
    //              a stack size of 0.
    //!
    virtual int32_t localFunctionStackSize() const = 0;

protected:
    virtual ~IParserError() {}
};

}
