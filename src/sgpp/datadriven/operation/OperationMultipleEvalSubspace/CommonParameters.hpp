#pragma once

#include <map>

//#include "datadriven/operation/OperationMultipleEvalSubspace/simple/X86Simple.hpp"
//#include "X86Baseline.hpp"
//#include "X86Vectorized.hpp"

namespace sg {
namespace parallel {

// list of kernels
enum ComputeKernelType {
	SIMPLE, COMBINED, ACCELERATOR
};

static std::map<ComputeKernelType, std::string> ComputeKernelTypeMap = { { SIMPLE, "SIMPLE" }, { COMBINED, "COMBINED" }, { ACCELERATOR, "ACCELERATOR" } };

static std::map<std::string, ComputeKernelType> NameComputeKernelTypeMap = { { "SIMPLE", SIMPLE }, { "COMBINED", COMBINED }, { "ACCELERATOR", ACCELERATOR } };

}
}

//TODO: add vector selection here, basis for auto tuning => use only easily parsable structures