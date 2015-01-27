// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#ifndef HEATEQUATIONPARABOLICPDESOLVERSYSTEMPARALLELMPI_HPP
#define HEATEQUATIONPARABOLICPDESOLVERSYSTEMPARALLELMPI_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/pde/operation/hash/OperationParabolicPDESolverSystemDirichlet.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace parallel {

    /**
     * This class implements the ParabolicPDESolverSystem for the
     * Heat Equation parallelized with MPI.
     */
    class HeatEquationParabolicPDESolverSystemParallelMPI : public SGPP::pde::OperationParabolicPDESolverSystemDirichlet {
      private:
        /// the heat coefficient
        double a;
        /// the Laplace Operation (Stiffness Matrix), on boundary grid
        SGPP::base::OperationMatrix* OpLaplaceBound;
        /// the LTwoDotProduct Operation (Mass Matrix), on boundary grid
        SGPP::base::OperationMatrix* OpMassBound;
        /// the Laplace Operation (Stiffness Matrix), on inner grid
        SGPP::base::OperationMatrix* OpLaplaceInner;
        /// the LTwoDotProduct Operation (Mass Matrix), on inner grid
        SGPP::base::OperationMatrix* OpMassInner;

        void applyMassMatrixComplete(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        void applyLOperatorComplete(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        void applyMassMatrixInner(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        void applyLOperatorInner(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

      public:
        /**
         * Std-Constructor
         *
         * @param SparseGrid reference to the sparse grid
         * @param alpha the sparse grid's coefficients
         * @param a the heat coefficient
         * @param TimestepSize the size of one timestep used in the ODE Solver
         * @param OperationMode specifies in which solver this matrix is used, valid values are: ExEul for explicit Euler,
         *                ImEul for implicit Euler, CrNic for Crank Nicolson solver
         */
        HeatEquationParabolicPDESolverSystemParallelMPI(SGPP::base::Grid& SparseGrid, SGPP::base::DataVector& alpha, double a, double TimestepSize, std::string OperationMode = "ExEul");

        /**
         * Std-Destructor
         */
        virtual ~HeatEquationParabolicPDESolverSystemParallelMPI();

        void finishTimestep();

        void coarsenAndRefine(bool isLastTimestep = false);

        void startTimestep();

        virtual void mult(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        virtual SGPP::base::DataVector* generateRHS();
    };

  }
}

#endif /* HEATEQUATIONPARABOLICPDESOLVERSYSTEMPARALLELMPI_HPP */