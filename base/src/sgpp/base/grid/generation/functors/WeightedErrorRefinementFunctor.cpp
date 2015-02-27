// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/base/grid/generation/functors/WeightedErrorRefinementFunctor.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {

    WeightedErrorRefinementFunctor::WeightedErrorRefinementFunctor(
      DataVector* alpha, Grid* grid, size_t refinements_num, float_t threshold) :
      alpha(alpha), refinements_num(refinements_num), threshold(threshold), grid(
        grid), trainDataset(
          NULL), classes(NULL) {

    }

    WeightedErrorRefinementFunctor::~WeightedErrorRefinementFunctor() {

    }

    /*
     * Given alpha_j, calculates
     *
     * \sum_{i=0}^{N} abs(phi_j(x_i) * alpha_j * r_i^2)
     *
     * r_i = y_i - \sum{j=0}^{M} phi_j(x_i) * alpha_j
     */
    float_t WeightedErrorRefinementFunctor::operator()(GridStorage* storage,
        size_t seq) {

      if (trainDataset == NULL || classes == NULL || errors == NULL) {
        throw base::application_exception("Training dataset or classes not set");
      }

      const size_t MIN_SUPPORT = 10;

      size_t numData = trainDataset->getNrows();

      SGPP::base::DataVector singleAlpha(alpha->getSize());
      singleAlpha.setAll(0.0);
      singleAlpha.set(seq, alpha->get(seq));

      /* phi_j(x_i) * alpha_j */
      SGPP::base::DataVector val1(numData);
      SGPP::op_factory::createOperationMultipleEval(*grid, *trainDataset)->mult(singleAlpha, val1);

      if (val1.getNumberNonZero() < MIN_SUPPORT) {
        return -std::numeric_limits<float_t>::infinity(); // threshold is 0.0
      }

      float_t error = 0;

      for (size_t i = 0; i < numData; i++) {
        /* abs(phi_j(x_i) * alpha_j * r_i^2) */
        //error += fabs(errors->get(i) * errors->get(i));
        error += fabs(val1.get(i) * errors->get(i) * errors->get(i));
      }

      return error;
    }

    float_t WeightedErrorRefinementFunctor::start() {
      return 0.0;
    }

    size_t WeightedErrorRefinementFunctor::getRefinementsNum() {
      return this->refinements_num;
    }

    float_t WeightedErrorRefinementFunctor::getRefinementThreshold() {
      return this->threshold;
    }

    void WeightedErrorRefinementFunctor::setTrainDataset(
      DataMatrix* trainDataset_) {
      trainDataset = trainDataset_;
    }

    void WeightedErrorRefinementFunctor::setClasses(DataVector* classes_) {
      classes = classes_;
    }

    void WeightedErrorRefinementFunctor::setErrors(DataVector* errors_) {
      errors = errors_;
    }

  }
}