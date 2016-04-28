// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#pragma once

#include <sgpp/base/exception/operation_exception.hpp>
#include <sgpp/base/opencl/OCLOperationConfiguration.hpp>
#include <sgpp/base/opencl/KernelSourceBuilderBase.hpp>

#include <fstream>
#include <sstream>
#include <string>
namespace sgpp {
namespace datadriven {
namespace DensityOCLMultiPlatform {

template<typename real_type>
class SourceBuilderMult: public base::KernelSourceBuilderBase<real_type> {
 private:
  std::shared_ptr<base::OCLDevice> device;
  json::Node &kernelConfiguration;
  size_t dims;
  size_t localWorkgroupSize;
  bool useLocalMemory;
  size_t dataBlockSize;
  size_t transGridBlockSize;
  uint64_t maxDimUnroll;

  std::string getData(std::string dim, size_t dataBlockingIndex) {
    std::stringstream output;
    if (kernelConfiguration["KERNEL_STORE_DATA"].get().compare("array") == 0) {
      output << "data_" << dataBlockingIndex << "[" << dim << "]";
    } else if (kernelConfiguration["KERNEL_STORE_DATA"].get().compare("register") == 0) {
      output << "data_" << dataBlockingIndex << "_" << dim;
    } else if (kernelConfiguration["KERNEL_STORE_DATA"].get().compare("pointer") == 0) {
      output << "ptrData[(" << dataBlockSize << " * globalIdx) + (resultSize * " << dim
             << ") + " << dataBlockingIndex << "]";
    } else {
      std::string error("OCL Error: Illegal value for parameter \"KERNEL_STORE_DATA\"");
      throw new base::operation_exception(error.c_str());
    }
    return output.str();
  }

  std::string save_from_global_to_private(size_t dimensions) {
    std::stringstream output;
    for (auto block = 0; block < dataBlockSize; block++) {
      output << this->indent[0] << "__private int point_indices_block" << block << "["
             << dimensions << "];" << std::endl;
      output << this->indent[0] << "__private int point_level_block" << block << "["
             << dimensions << "];" << std::endl;
      for (size_t i = 0; i < dimensions; i++) {
        output << this->indent[0] << "point_indices_block" << block << "[" << i
               << "] = starting_points[(gridindex * " << dataBlockSize << " + " << block << ") * "
               << dimensions << " * 2 + 2 * " << i << "];" << std::endl;
        output << this->indent[0] << "point_level_block" << block << "[" << i
               << "] = starting_points[(gridindex * " << dataBlockSize << " + " << block << ") * "
               << dimensions << " * 2 + 2 * " << i << " + 1];" << std::endl;
      }
    }
    return output.str();
  }


 public:
  SourceBuilderMult(std::shared_ptr<base::OCLDevice> device, json::Node &kernelConfiguration,
                    size_t dims) :
      device(device), kernelConfiguration(kernelConfiguration), dims(dims), dataBlockSize(1) {
    if (kernelConfiguration.contains("LOCAL_SIZE"))
      localWorkgroupSize = kernelConfiguration["LOCAL_SIZE"].getUInt();
    if (kernelConfiguration.contains("KERNEL_USE_LOCAL_MEMORY"))
      useLocalMemory = kernelConfiguration["KERNEL_USE_LOCAL_MEMORY"].getBool();
    if (kernelConfiguration.contains("KERNEL_DATA_BLOCKING_SIZE"))
      dataBlockSize = kernelConfiguration["KERNEL_DATA_BLOCKING_SIZE"].getUInt();
  }

  std::string generateSource(size_t dimensions, size_t problemsize) {
    std::stringstream sourceStream;

    if (this->floatType().compare("double") == 0) {
      sourceStream << "#pragma OPENCL EXTENSION cl_khr_fp64 : enable" << std::endl
                   << std::endl;
    }

    sourceStream << "__kernel" << std::endl;
    sourceStream << "__attribute__((reqd_work_group_size(" << localWorkgroupSize << ", 1, 1)))"
                 << std::endl;
    sourceStream << "void multdensity(__global const int *starting_points,__global const "
                 << this->floatType() << " *alpha, __global " << this->floatType()
                 << " *result,const " << this->floatType()
                 << " lambda, const int startid, __global " << this->floatType() << " *divisors)"
                 << std::endl;
    sourceStream << "{" << std::endl;
    sourceStream << this->indent[0] << "int gridindex = startid + get_global_id(0);"
                 << std::endl
                 << this->indent[0] << "__private int local_id = get_local_id(0);"
                 << std::endl;
    sourceStream << save_from_global_to_private(dimensions);
    sourceStream << this->indent[0] << "__private int teiler = 0;" << std::endl;
    sourceStream << this->indent[0] << "__private " << this->floatType()
                 << " h = 0.0;" << std::endl;
    sourceStream << this->indent[0] << "__private " << this->floatType()
                 << " umid = 0.0;" << std::endl;
    sourceStream << this->indent[0] << "__private " << this->floatType()
                 << " sum = 0.0;" << std::endl;
    sourceStream << this->indent[0] << "__private int u= 0;" << std::endl;
    for (size_t block = 0; block < dataBlockSize; block++)
      sourceStream << this->indent[0] << this->floatType() << " gesamtint_block" << block
                   <<" = 0.0;" << std::endl;
    if (useLocalMemory) {
      sourceStream << this->indent[0] << "__local " << "int indices_local["
                   << localWorkgroupSize * dimensions << "];" << std::endl
                   << this->indent[0] << "__local " << "int level_local["
                   << localWorkgroupSize * dimensions << "];" << std::endl
                   << this->indent[0] << "__local " << this->floatType() << " alpha_local["
                   << localWorkgroupSize << "];" << std::endl
                   <<  this->indent[0] << "for (int group = 0; group < "
                   << problemsize / localWorkgroupSize << "; group++) {" << std::endl
                   << this->indent[1] << "barrier(CLK_LOCAL_MEM_FENCE);" << std::endl
                   << this->indent[1] << "for (int j = 0; j <     " << dimensions
                   << " ; j++) {" << std::endl
                   << this->indent[2] << "indices_local[local_id * " << dimensions
                   << " + j] = starting_points[group * " << localWorkgroupSize * dimensions * 2
                   << "  + local_id * " <<  dimensions * 2 << " + 2 * j];" << std::endl
                   << this->indent[2] << "level_local[local_id * " << dimensions
                   << " + j] = starting_points[group * " << localWorkgroupSize * dimensions * 2
                   << "  + local_id * " <<  dimensions * 2 << " + 2 * j + 1];" << std::endl
                   << this->indent[1] << "}" << std::endl
                   << this->indent[1] << "alpha_local[local_id] = alpha[group * "
                   << localWorkgroupSize << "  + local_id ];" << std::endl
                   << this->indent[1] << "barrier(CLK_LOCAL_MEM_FENCE);" << std::endl
                   << this->indent[1] << "for (int i = 0 ; i < " << localWorkgroupSize
                   << "; i++) {" << std::endl
                   << this->indent[2] << "__private " << this->floatType();
      // Generate body for each element in the block
      for (size_t block = 0; block < dataBlockSize; block++) {
        sourceStream << " zellenintegral = 1.0;" << std::endl;
        sourceStream << this->indent[2] << "int same_levels = " << dimensions << ";" << std::endl;
        sourceStream << this->indent[2] << "for(private int dim = 0;dim< " << dimensions
                     << ";dim++) {" << std::endl
                     << this->indent[3] <<"h = 1.0 / (1 << level_local[i*" << dimensions
                     << "+dim]);" << std::endl;
        sourceStream << this->indent[3] << "u = (1 << point_level_block" << block
                     << "[dim]);" << std::endl
                     << this->indent[3] << "umid = u*h*(indices_local[i*" << dimensions
                     << " + dim])-point_indices_block" << block << "[dim];" << std::endl;
        sourceStream << this->indent[3] << "umid = fabs(umid);" << std::endl;
        sourceStream << this->indent[3] << "umid = 1.0-umid;" << std::endl;
        sourceStream << this->indent[3] << "umid = fmax(umid,0.0);" << std::endl;
        sourceStream << this->indent[3] << "sum = h*(umid);" << std::endl
                     << this->indent[3] << "h = 1.0 / (1 << point_level_block" << block
                     << "[dim]);" << std::endl;
        sourceStream << this->indent[3] << "u = (1 << level_local[i*" << dimensions << "+dim]);"
                     << std::endl
                     << this->indent[3] << "umid = u*h*(point_indices_block" << block <<
            "[dim])-indices_local[i*"
                     << dimensions << " + dim];" << std::endl;
        sourceStream << this->indent[3] << "umid = fabs(umid);" << std::endl;
        sourceStream << this->indent[3] << "umid = 1.0-umid;" << std::endl;
        sourceStream << this->indent[3] << "umid = fmax(umid,0.0);" << std::endl;
        sourceStream << this->indent[3] << "sum += h*(umid);" << std::endl;
        //sourceStream << this->indent[3] << "sum *= level_local[i* " << dimensions
        //             << "+dim] == point_level_block" << block << "[dim] ? 1.0/3.0 : 1.0;"
        //             << std::endl;
        sourceStream << this->indent[3] << "zellenintegral*=sum;" << std::endl;
        sourceStream << this->indent[3] << "same_levels -= min((int)(abs(level_local[i*" << dimensions
                     << "+dim] - point_level_block" << block
                     << "[dim])),(int)(1));" << std::endl;
        sourceStream << this->indent[2] << "}" << std::endl;
        sourceStream << this->indent[2] << "zellenintegral *= divisors[same_levels];" << std::endl;
        sourceStream << this->indent[2] << "gesamtint_block" << block
                     <<" += zellenintegral*alpha_local[i];" << std::endl;
      }
      sourceStream << this->indent[1] << "}" << std::endl;
      sourceStream << this->indent[0] << "}" << std::endl;
    } else {
      sourceStream << this->indent[0] << "for(__private int i = 0; i < " << problemsize
                   << "; i++) {" << std::endl;

      for (size_t block = 0; block < dataBlockSize; block++) {
        sourceStream << this->indent[1] << this->floatType() << " zellenintegral = 1.0;"
                     << std::endl;
        sourceStream << this->indent[1] << "for(private int dim = 0;dim< " << dimensions
                     << ";dim++) {" << std::endl;
        sourceStream << this->indent[2] << "int index = point_indices_block" << block << "[dim];"
                     << std::endl;
        sourceStream << this->indent[2] << "int level = point_level_block" << block << "[dim];"
                     << std::endl;
        sourceStream << this->indent[2] << "int index2 = starting_points[i* " << dimensions
                     << "*2+2*dim];" << std::endl;
        sourceStream << this->indent[2] << "int level2 = starting_points[i* " << dimensions
                     << "*2+2*dim+1];" << std::endl;
        sourceStream << this->indent[2] << "if(starting_points[gridindex* " << dimensions
                     << "*2+2*dim+1]>starting_points[i* " << dimensions
                     << "*2+2*dim+1]) {" << std::endl;
        sourceStream << this->indent[3] << "index = starting_points[i* "
                     << dimensions << "*2+2*dim];" << std::endl;
        sourceStream << this->indent[3] << "level = starting_points[i* "
                     << dimensions << "*2+2*dim+1];" << std::endl;
        sourceStream << this->indent[3] << "index2 = point_indices_block" << block << "[dim];"
                     << std::endl;
        sourceStream << this->indent[3] << "level2 = point_level_block" << block << "[dim];"
                     << std::endl;
        sourceStream << this->indent[2] << "}" << std::endl;
        sourceStream << this->indent[2] << "int teiler = (1 << level2);" << std::endl;
        sourceStream << this->indent[2] << this->floatType() << " h = 1.0 / teiler;" << std::endl;
        sourceStream << this->indent[2] << "__private " << this->floatType()
                     << " grenze1 = h*(index2-1);" << std::endl;
        sourceStream << this->indent[2] << "__private " << this->floatType()
                     << " grenze2 = h*(index2+1);" << std::endl;
        sourceStream << this->indent[2] << "int u= (1 << level);" << std::endl;
        sourceStream << this->indent[2] << "__private " << this->floatType()
                     << " uright = u*grenze2-index;" << std::endl;
        sourceStream << this->indent[2] << "__private " << this->floatType()
                     << " uleft = u*grenze1-index;" << std::endl;
        sourceStream << this->indent[3] << "uleft = fabs(uleft);" << std::endl;
        sourceStream << this->indent[2] << "uleft = 1-uleft;" << std::endl;
        sourceStream << this->indent[2] << "if(uleft<0)" << std::endl;
        sourceStream << this->indent[3] << "uleft = 0;" << std::endl;
        sourceStream << this->indent[3] << "uright = fabs(uright);" << std::endl;
        sourceStream << this->indent[2] << "uright = 1-uright;" << std::endl;
        sourceStream << this->indent[2] << "if(uright<0)" << std::endl;
        sourceStream << this->indent[3] << "uright = 0;" << std::endl;
        sourceStream << this->indent[2] << "__private " << this->floatType()
                     << " integral = h/2.0*(uleft+uright);" << std::endl;
        sourceStream << this->indent[2] << "if(starting_points[i* " << dimensions
                     << "*2+2*dim+1] == starting_points[gridindex* "
                     << dimensions << "*2+2*dim+1]) {" << std::endl;
        sourceStream <<  this->indent[3] <<"integral = 2.0/3.0*h;" << std::endl;
        sourceStream << this->indent[3] << "if(starting_points[i* " << dimensions
                     << "*2+2*dim] != starting_points[gridindex* "
                     << dimensions << "*2+2*dim])" << std::endl;
        sourceStream << this->indent[4] << "integral = 0.0;" << std::endl;
        sourceStream << this->indent[2] << "}" << std::endl;
        sourceStream << this->indent[3] << "zellenintegral *= integral;" << std::endl;
        sourceStream << this->indent[1] << "}" << std::endl;
        sourceStream << this->indent[1] << "gesamtint_block" << block
                     <<" += zellenintegral*alpha[i];" << std::endl;
      }
      sourceStream << this->indent[0] << "}" << std::endl;
    }
    for (auto block = 0; block < dataBlockSize; ++block) {
      sourceStream << this->indent[0] << "result[get_global_id(0) * "<< dataBlockSize
                   <<" + " << block << "] = gesamtint_block" << block << ";" << std::endl;
      sourceStream << this->indent[0] << "result[get_global_id(0) * "<< dataBlockSize
                   <<" + " << block << "] += alpha[get_global_id(0) * "<< dataBlockSize
                   <<" + " << block << "]*" << "lambda;" << std::endl;
    }

    sourceStream << "}" << std::endl;

    if (kernelConfiguration.contains("WRITE_SOURCE")) {
      if (kernelConfiguration["WRITE_SOURCE"].getBool()) {
        this->writeSource("DensityOCLMultiPlatform_mult.cl", sourceStream.str());
      }
    }
    return sourceStream.str();
  }
};

}  // namespace DensityOCLMultiPlatform
}  // namespace datadriven
}  // namespace sgpp
