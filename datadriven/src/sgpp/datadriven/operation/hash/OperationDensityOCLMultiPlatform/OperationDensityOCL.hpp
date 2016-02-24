// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once
#include <sgpp/base/operation/hash/OperationMatrix.hpp>
#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/operation/hash/OperationMultipleEval.hpp>
#include <sgpp/base/tools/SGppStopwatch.hpp>
#include <sgpp/base/exception/operation_exception.hpp>
#include <sgpp/base/opencl/OCLOperationConfiguration.hpp>
#include <sgpp/base/opencl/OCLManager.hpp>

namespace SGPP {
namespace datadriven {
namespace DensityOCLMultiPlatform {

class OperationDensityOCL: public base::OperationMatrix {
 public:
    OperationDensityOCL()  {
    }
    virtual void mult(base::DataVector& alpha, base::DataVector& result) = 0;
    virtual void generateb(base::DataMatrix &dataset, SGPP::base::DataVector &b) = 0;
};

}  // namespace DensityOCLMultiPlatform
}  // namespace datadriven
}  // namespace SGPP
