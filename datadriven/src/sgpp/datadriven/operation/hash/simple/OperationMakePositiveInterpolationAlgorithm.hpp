// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

#include <sgpp/globaldef.hpp>

#include <vector>

namespace sgpp {
namespace datadriven {

class OperationMakePositiveInterpolationAlgorithm {
 public:
  OperationMakePositiveInterpolationAlgorithm();
  virtual ~OperationMakePositiveInterpolationAlgorithm();

  virtual void computeHierarchicalCoefficients(base::Grid& grid, base::DataVector& alpha,
                                               std::vector<size_t>& addedGridPoints,
                                               double tol = -1e-14) = 0;
};

// -------------------------------------------------------------------------------------------
class OperationMakePositiveSetToZero : public OperationMakePositiveInterpolationAlgorithm {
 public:
  explicit OperationMakePositiveSetToZero(double offset = 0);
  virtual ~OperationMakePositiveSetToZero();

  void computeHierarchicalCoefficients(base::Grid& grid, base::DataVector& alpha,
                                       std::vector<size_t>& addedGridPoints,
                                       double tol = -1e-14) override;

 private:
  double offset;
};

// -------------------------------------------------------------------------------------------
class OperationMakePositiveInterpolateExp : public OperationMakePositiveInterpolationAlgorithm {
 public:
  void computeHierarchicalCoefficients(base::Grid& grid, base::DataVector& alpha,
                                       std::vector<size_t>& addedGridPoints,
                                       double tol = -1e-14) override;
};
// -------------------------------------------------------------------------------------------
class OperationMakePositiveInterpolateBoundaryOfSupport
    : public OperationMakePositiveInterpolationAlgorithm {
 public:
  void computeHierarchicalCoefficients(base::Grid& grid, base::DataVector& alpha,
                                       std::vector<size_t>& addedGridPoints,
                                       double tol = -1e-14) override;

 private:
  double computeMinimum(base::Grid& grid, base::DataVector& alpha, base::HashGridPoint& gp);
};

} /* namespace datadriven */
} /* namespace sgpp */
