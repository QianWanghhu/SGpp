/* Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * SampleProviderModule.cpp
 *
 *  Created on: 17.05.2016
 *      Author: Michael Lettrich
 */

#include "DataSource.hpp"

#include <sgpp/datadriven/datamining/modules/dataSource/DataSourceIterator.hpp>
#include <sgpp/datadriven/datamining/modules/dataSource/DataTransformation.hpp>
#include <sgpp/datadriven/datamining/modules/dataSource/FileSampleProvider.hpp>
#include <sgpp/datadriven/datamining/modules/dataSource/RosenblattTransformation.hpp>
#include <sgpp/datadriven/tools/Dataset.hpp>
#include <sgpp/globaldef.hpp>

#include <iostream>
#include <limits>
#include <memory>

namespace sgpp {
namespace datadriven {

DataSource::DataSource(DataSourceConfig conf, SampleProvider* sp)
    : config(conf), currentIteration(0), sampleProvider(std::unique_ptr<SampleProvider>(sp)) {
  // if a file name was specified, we are reading from a file, so we need to open it.
  if (!this->config.filePath.empty()) {
    dynamic_cast<FileSampleProvider*>(sampleProvider.get())->readFile(this->config.filePath);
  }
}

DataSourceIterator DataSource::begin() { return DataSourceIterator(*this, 0); }

DataSourceIterator DataSource::end() { return DataSourceIterator(*this, config.numBatches); }

Dataset* DataSource::getNextSamples() {
  // only one iteration: we want all samples
  Dataset* dataset;
  if (config.numBatches == 1 && config.batchSize == 0) {
    currentIteration++;
    dataset = sampleProvider->getAllSamples();
  } else {
    currentIteration++;
    dataset = sampleProvider->getNextSamples(config.batchSize);
  }
  // Transform dataset if wanted
  if (!(config.dataTransformation == DataTransformationType::NONE)) {
    DataTransformation* dataTr = new DataTransformation();
    return dataTr->initialize(config.dataTransformation, dataset).doTransformation();
  } else {
    return dataset;
  }
}

const DataSourceConfig& DataSource::getConfig() const { return config; }

size_t DataSource::getCurrentIteration() const { return currentIteration; }

} /* namespace datadriven */
} /* namespace sgpp */
