// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/datadriven/application/ClassificationLearner.hpp>

namespace sgpp {
namespace datadriven {

ClassificationLearner::ClassificationLearner(
    sgpp::base::RegularGridConfiguration gridConfig,
    sgpp::base::AdpativityConfiguration adaptivityConfig,
    sgpp::solver::SLESolverConfiguration solverConfig,
    sgpp::datadriven::RegularizationConfiguration regularizationConfig)
    : gridConfig(gridConfig),
      adaptivityConfig(adaptivityConfig),
      solverConfig(solverConfig),
      regularizationConfig(regularizationConfig) {}

void ClassificationLearner::train(sgpp::base::DataMatrix& trainDataset,
                                  sgpp::base::DataVector& classes) {
  uniqueClasses = std::set<class_t>();
  learners = std::vector<learner_t>();

  // We perform one vs all classification.
  // This means, that we need to create a classificator for each unique class,
  // that is able to predict whether it is a member of that particular class,
  // or not.

  // To do this, we need to find the unique classes first.
  // We use the assumption, that classes are coded with an arbitrary number as the output variable.
  for (size_t i = 0; i < classes.getSize(); ++i) {
    uniqueClasses.insert(classes[i]);
  }
  learners.reserve(uniqueClasses.size());

  // Now we create a learner for each class.
  for (const auto uniqueClass : uniqueClasses) {
    auto newY = generateYOneVsAll(classes, uniqueClass);
    auto learner =
        RegressionLearner(gridConfig, adaptivityConfig, solverConfig, regularizationConfig);
    std::cout << "Created learner for class " << uniqueClass << std::endl;
    learner.train(trainDataset, newY);
    learners.emplace_back(uniqueClass, std::move(learner));
  }
}

size_t ClassificationLearner::getGridSize() const {
    return learners[0].second.getGridSize();
}


sgpp::base::DataVector ClassificationLearner::predict(sgpp::base::DataMatrix& data) {
  const auto nPredictions = learners.size();
  auto predictions = std::vector<std::pair<class_t, sgpp::base::DataVector>>();
  predictions.reserve(nPredictions);

  auto resultClasses = sgpp::base::DataVector(data.getNrows());

  // We need a prediction for each learner. Remember: Each learner can only
  // check wheter the data row belong to its class or not.
  for (auto& learner : learners) {
    const class_t uniqueClass = learner.first;
    auto& regressionLearner = learner.second;
    const auto prediction = regressionLearner.predict(data);
    predictions.emplace_back(uniqueClass, prediction);
  }
  // Find learner with highest output value and its class. This class is (hopefully) the correct
  // one.
  for (size_t i = 0; i < data.getNrows(); ++i) {
    class_t bestClass = 0;
    double bestLearnerResult = 0.0;  // = worst case
    for (const auto& prediction : predictions) {
      const auto learnerResult = prediction.second[i];
      if (learnerResult > bestLearnerResult) {
        bestLearnerResult = learnerResult;
        bestClass = prediction.first;
      }
    }
    resultClasses[i] = bestClass;
  }
  return resultClasses;
}

double ClassificationLearner::getAccuracy(base::DataMatrix& data, const base::DataVector& y) {
  const auto yPrediction = predict(data);
  return getAccuracy(y, yPrediction);
}

sgpp::base::DataVector ClassificationLearner::generateYOneVsAll(const sgpp::base::DataVector& oldY,
                                                                double classValue) {
  const auto nElem = oldY.getSize();
  auto newY = sgpp::base::DataVector(nElem);
  for (size_t i = 0; i < nElem; ++i) {
    if (oldY[i] == classValue) {
      newY[i] = 1;
    } else {
      newY[i] = 0;
    }
  }
  return newY;
}

double ClassificationLearner::getAccuracy(const base::DataVector& y,
                                          const base::DataVector yPrediction) {
  size_t numPredictions = yPrediction.getSize();
  size_t numCorrect = 0;
  for (size_t i = 0; i < numPredictions; ++i) {
    if (y[i] == yPrediction[i]) {
      ++numCorrect;
    }
  }
  return (100.0 * static_cast<double>(numCorrect)) / static_cast<double>(numPredictions);
}

}  // namespace datadriven
}  // namespace sgpp
