// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/exception/factory_exception.hpp>
#include <sgpp/base/opencl/OCLOperationConfiguration.hpp>
#include <sgpp/base/opencl/OCLManager.hpp>
#include <sgpp/globaldef.hpp>
#include <string>

#include "OperationDensityOCLMultiPlatform.hpp"
namespace sgpp {
namespace datadriven {

DensityOCLMultiPlatform::OperationDensityOCL*
createDensityOCLMultiPlatformConfigured(base::Grid& grid, size_t dimension,
                                        double lambda, std::string opencl_conf,
                                        size_t platform_id, size_t device_id) {
  std::shared_ptr<base::OCLManagerMultiPlatform> manager;

  std::cout << "Using configuration file " << opencl_conf << std::endl;
  base::OCLOperationConfiguration *parameters = new base::OCLOperationConfiguration(opencl_conf);
  manager = std::make_shared<base::OCLManagerMultiPlatform>(true);
  parameters->serialize("MyOCLConfDebug.cfg");
  if (parameters->contains("INTERNAL_PRECISION") == false) {
    std::cout << "Warning! No internal precision setting detected."
              << " Using double precision from now on!" << std::endl;
    parameters->addIDAttr("INTERNAL_PRECISION", "double");
  }

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<float>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<float>::augmentDefaultParameters(*parameters);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<double>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<double>::augmentDefaultParameters(*parameters);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  parameters->serialize("MyOCLConf.cfg");

  std::string &firstPlatformName =
      (*parameters)["PLATFORMS"].keys()[0];
  std::string &firstDeviceName =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"].keys()[0];
  json::Node &deviceNode =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"][firstDeviceName];
  json::Node &firstKernelConfig = deviceNode["KERNELS"]["multdensity"];
  json::Node &secondKernelConfig = deviceNode["KERNELS"]["cscheme"];

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<float>(grid, dimension, manager, firstKernelConfig,
                                                secondKernelConfig, static_cast<float>(lambda),
                                                platform_id, device_id);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<double>(grid, dimension, manager, firstKernelConfig,
                                                 secondKernelConfig, lambda,
                                                 platform_id, device_id);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  return NULL;
}
DensityOCLMultiPlatform::OperationDensityOCL*
createDensityOCLMultiPlatformConfigured(int *gridpoints, size_t gridsize, size_t dimension,
                                        double lambda, std::string opencl_conf,
                                        size_t platform_id, size_t device_id) {
  std::shared_ptr<base::OCLManagerMultiPlatform> manager;

  std::cout << "Using configuration file " << opencl_conf << std::endl;
  base::OCLOperationConfiguration *parameters = new base::OCLOperationConfiguration(opencl_conf);
  manager = std::make_shared<base::OCLManagerMultiPlatform>(true);
  parameters->serialize("MyOCLConfDebug.cfg");
  if (parameters->contains("INTERNAL_PRECISION") == false) {
    std::cout << "Warning! No internal precision setting detected."
              << " Using double precision from now on!" << std::endl;
    parameters->addIDAttr("INTERNAL_PRECISION", "double");
  }

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<float>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<float>::augmentDefaultParameters(*parameters);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<double>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<double>::augmentDefaultParameters(*parameters);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  parameters->serialize("MyOCLConf.cfg");

  std::string &firstPlatformName =
      (*parameters)["PLATFORMS"].keys()[0];
  std::string &firstDeviceName =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"].keys()[0];
  json::Node &deviceNode =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"][firstDeviceName];
  json::Node &firstKernelConfig = deviceNode["KERNELS"]["multdensity"];
  json::Node &secondKernelConfig = deviceNode["KERNELS"]["cscheme"];

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<float>(gridpoints, gridsize, dimension, manager,
                                                firstKernelConfig,
                                                secondKernelConfig, static_cast<float>(lambda),
                                                 platform_id, device_id);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<double>(gridpoints, gridsize, dimension, manager,
                                                 firstKernelConfig,
                                                 secondKernelConfig, lambda,
                                                 platform_id, device_id);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  return NULL;
}
DensityOCLMultiPlatform::OperationDensityOCL*
createDensityOCLMultiPlatformConfigured(base::Grid& grid, size_t dimension,
                                        double lambda, std::string opencl_conf) {
  std::shared_ptr<base::OCLManagerMultiPlatform> manager;

  std::cout << "Using configuration file " << opencl_conf << std::endl;
  base::OCLOperationConfiguration *parameters = new base::OCLOperationConfiguration(opencl_conf);
  manager = std::make_shared<base::OCLManagerMultiPlatform>(true);
  parameters->serialize("MyOCLConfDebug.cfg");
  if (parameters->contains("INTERNAL_PRECISION") == false) {
    std::cout << "Warning! No internal precision setting detected."
              << " Using double precision from now on!" << std::endl;
    parameters->addIDAttr("INTERNAL_PRECISION", "double");
  }

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<float>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<float>::augmentDefaultParameters(*parameters);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    DensityOCLMultiPlatform::KernelDensityMult<double>::augmentDefaultParameters(*parameters);
    DensityOCLMultiPlatform::KernelDensityB<double>::augmentDefaultParameters(*parameters);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  parameters->serialize("MyOCLConf.cfg");

  size_t platformid = 0;
  if (parameters->contains("USE_PLATFORM") == true) {
    platformid = (*parameters)["USE_PLATFORM"].getInt();
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"DensityOCL\": "
                << "There is no given information about which opencl platform (ID) to use!"
                << " Add \"USE_PLATFORM\": platform_id to your configuration file "
                << "or use a different factory method." << std::endl;
    throw base::factory_exception(errorString.str().c_str());
  }
  size_t deviceid = 0;
  if (parameters->contains("USE_DEVICE") == true) {
    deviceid = (*parameters)["USE_DEVICE"].getInt();
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"DensityOCL\": "
                << "There is no given information about which opencl device (ID) to use!"
                << " Add \"USE_DEVICE\": device_id to your configuration file "
                << "or use a different factory method." << std::endl;
    throw base::factory_exception(errorString.str().c_str());
  }
  std::string &firstPlatformName =
      (*parameters)["PLATFORMS"].keys()[0];
  std::string &firstDeviceName =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"].keys()[0];
  json::Node &deviceNode =
      (*parameters)["PLATFORMS"][firstPlatformName]["DEVICES"][firstDeviceName];
  json::Node &firstKernelConfig = deviceNode["KERNELS"]["multdensity"];
  json::Node &secondKernelConfig = deviceNode["KERNELS"]["cscheme"];

  if ((*parameters)["INTERNAL_PRECISION"].get().compare("float") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<float>(grid, dimension, manager, firstKernelConfig,
                                                secondKernelConfig, static_cast<float>(lambda),
                                                platformid, deviceid);
  } else if ((*parameters)["INTERNAL_PRECISION"].get().compare("double") == 0) {
    return new datadriven::DensityOCLMultiPlatform::
        OperationDensityOCLMultiPlatform<double>(grid, dimension, manager, firstKernelConfig,
                                                 secondKernelConfig, lambda,
                                                platformid, deviceid);
  } else {
    std::stringstream errorString;
    errorString << "Error creating operation\"OperationDensityOCLMultiPlatform\": "
                << " invalid value for parameter \"INTERNAL_PRECISION\"";
    throw base::factory_exception(errorString.str().c_str());
  }
  return NULL;
}
}  // namespace datadriven
}  // namespace sgpp
