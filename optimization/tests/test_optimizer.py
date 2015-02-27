# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

import unittest
import pysgpp
import objective_functions
import random
import math

class TestOptimizer(unittest.TestCase):
    def setUp(self):
        """Initialize the test case."""
        # disable status output
        pysgpp.cvar.OptPrinterInstance.setVerbosity(-1)
        # disable multi-threading
        pysgpp.omp_set_num_threads(1)
    
    def testOptimizers(self):
        """Test SGPP::optimization::optimizer."""
        f = objective_functions.TitleFunction()
        f_gradient = objective_functions.TitleFunctionGradient()
        f_hessian = objective_functions.TitleFunctionHessian()
        
        # Test All The Optimizers!
        N = 1000
        optimizers = [pysgpp.OptGradientMethod(f, f_gradient, N),
                      pysgpp.OptNLCG(f, f_gradient, N),
                      pysgpp.OptNewton(f, f_hessian, N),
                      pysgpp.OptNelderMead(f, N),
                      pysgpp.OptRandomSearch(f, N),
                      pysgpp.OptDifferentialEvolution(f, N)]
        
        for optimizer in optimizers:
            xopt = pysgpp.DoubleVector()
            # optimize
            fopt = optimizer.optimize(xopt)
            
            # test xopt and fopt
            self.assertEqual(len(xopt), 2)
            self.assertAlmostEqual(xopt[0], 3.0/16.0 * math.pi, places=2)
            self.assertAlmostEqual(xopt[1], 3.0/14.0 * math.pi, places=2)
            self.assertAlmostEqual(fopt, -2.0, places=2) 