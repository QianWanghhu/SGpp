// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/grid/type/SquareRootGrid.hpp>

#include <sgpp/base/grid/generation/SquareRootGridGenerator.hpp>
//#include <sgpp/base/operation/hash/OperationEvalLinearBoundary.hpp>
//#include <sgpp/base/operation/hash/OperationHierarchisationLinearBoundary.hpp>

#include <sgpp/base/basis/linear/boundary/LinearBoundaryBasis.hpp>


#include <iostream>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {

    SquareRootGrid::SquareRootGrid(std::istream& istr) : Grid(istr) {

    }

    SquareRootGrid::SquareRootGrid(size_t dim) {
      this->storage = new GridStorage(dim);
    }

    SquareRootGrid::SquareRootGrid(BoundingBox& BB) {
      this->storage = new GridStorage(BB);
    }

    SquareRootGrid::~SquareRootGrid() {
    }

    const char* SquareRootGrid::getType() {
      return "squareRoot";
    }

    const SBasis& SquareRootGrid::getBasis(){
		static SLinearBoundaryBase basis;
		return basis;
	}

    Grid* SquareRootGrid::unserialize(std::istream& istr) {
      return new SquareRootGrid(istr);
    }
    /**
     * Creates new GridGenerator
     * This must be changed if we add other storage types
     */
    GridGenerator* SquareRootGrid::createGridGenerator() {
      return new SquareRootGridGenerator(this->storage);
    }
    //OperationHierarchisation* SquareRootGrid::createOperationHierarchisation()
    //{
    //  return new OperationHierarchisationLinearBoundary(this->storage);
    //}
    //OperationEval* SquareRootGrid::createOperationEval()
    //{
    //  return new OperationEvalLinearBoundary(this->storage);
    //}

    //OperationConvert* SquareRootGrid::createOperationConvert()
    //{
    //  throw factory_exception("Unsupported operation");
    //}

  }
}
