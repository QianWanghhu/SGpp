// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// The Good, i.e. without any modifications
//%include "FullGrid.i"
//%include "combigrid/src/sgpp/base/grid/combination/FullGridSet.hpp"
//%include "FullGridSet.i"

%include "combigrid/src/sgpp/combigrid/utils/combigrid_ultils.hpp"
%ignore combigrid::CombigridLevelVector::operator=; 
%rename(operatorAdd) combigrid::CombigridLevelVector::operator+;
%rename(operatorMultiply) combigrid::CombigridLevelVector::operator*;
%rename(operatorSubtract) combigrid::CombigridLevelVector::operator-;
%rename(operatorAssignment) combigrid::CombigridLevelVector::operator=;
%include "combigrid/src/sgpp/combigrid/utils/CombigridLevelVector.hpp"  
%include "combigrid/src/sgpp/combigrid/basisfunction/CombiBasisFunctionBasis.hpp"
%include "combigrid/src/sgpp/combigrid/basisfunction/CombiLinearBasisFunction.hpp"
%include "combigrid/src/sgpp/combigrid/domain/AbstractStretchingMaker.hpp"
%include "combigrid/src/sgpp/combigrid/domain/CombiDomain1D.hpp" 
%include "combigrid/src/sgpp/combigrid/domain/CombiGridDomain.hpp"
%include "combigrid/src/sgpp/combigrid/domain/CombiAtanSpecialStretching.hpp"
%include "combigrid/src/sgpp/combigrid/domain/CombiTanStretching.hpp"
%include "combigrid/src/sgpp/combigrid/domain/CombiUniformStretching.hpp"   
%include "combigrid/src/sgpp/combigrid/combischeme/CombiSchemeBasis.hpp" 
%include "combigrid/src/sgpp/combigrid/combischeme/CombiTS_CT.hpp"
%include "combigrid/src/sgpp/combigrid/combischeme/CombiS_CT.hpp"
%include "combigrid/src/sgpp/combigrid/combischeme/CombiArbitraryScheme.hpp"
%rename(getFullGridConst) combigrid::CombiGridKernel::getFullGrid(int) const;
%include "combigrid/src/sgpp/combigrid/combigridkernel/CombiGridKernel.hpp"
%rename(getFullGridConst) combigrid::AbstractCombiGrid::getFullGrid(int) const;
%include "combigrid/src/sgpp/combigrid/combigrid/AbstractCombiGrid.hpp"
%include "combigrid/src/sgpp/combigrid/combigrid/SerialCombiGrid.hpp"
%include "combigrid/src/sgpp/combigrid/combigrid/AdaptiveSerialCombiGrid.hpp"
%include "combigrid/src/sgpp/combigrid/combigrid/AdaptiveSerialCombiGridVariableCoefficients.hpp"

//%template(ComplexDouble) complex<double>;
//
%rename(getDomainConst) combigrid::FullGrid::getDomain() const;
%rename(getElementVectorConst) combigrid::FullGrid::getElementVector() const;
%rename(getLevelsConst) combigrid::FullGrid::getLevels() const;
%include "combigrid/src/sgpp/combigrid/fullgrid/CombiFullGrid.hpp"
%template(doubleFullGrid) combigrid::FullGrid<double>;

//%template(FullGridC) combigrid::FullGrid< complex<double> >;
//%template(CombiGridKernelC) combigrid::CombiGridKernel< complex<double> >;
%template(CombiGridKernelD) combigrid::CombiGridKernel< double >;   
//%template(ComplexVector) std::vector< complex<double> >;

//%apply std::string *INPUT { std::string& istr };

%apply unsigned int *OUTPUT { unsigned int& l, unsigned int& i };

//%apply std::vector<std::pair<size_t, double> > *OUTPUT { std::vector<std::pair<size_t, double> >& result };
//%apply std::vector<double> *INPUT { std::vector<double>& point }; 