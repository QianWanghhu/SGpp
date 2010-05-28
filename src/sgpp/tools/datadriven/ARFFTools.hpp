/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef ARFFTOOLS_HPP
#define ARFFTOOLS_HPP

#include "data/DataVector.hpp"

namespace sg
{

/**
 * Class that provides functionality to read and write ARFF files
 */
class ARFFTools
{
private:
	/**
	 * stores the attribute info of one instance into a DataVector
	 *
	 * @param instance the string that contains the instance's values
	 * @param destination DataVector into which the instance is stored
	 * @param instanceNo the number of the instance
	 */
	void writeNewElement(std::string& instance, DataVector& destination, size_t instanceNo);

	/**
	 * stores the class info of one instance into a DataVector
	 *
	 * @param instance the string that contains the instance's class
	 * @param destination DataVector into which the instance is stored
	 * @param instanceNo the number of the instance
	 */
	void writeNewClass(std::string& instance, DataVector& destination, size_t instanceNo);

public:
	/**
	 * STD-Constructor
	 */
	ARFFTools();

	/**
	 * STD-Destructor
	 */
	~ARFFTools();

	/**
	 * Determine how many dimensions the dataset contains
	 *
	 * @param tfilename filename of the ARFF file
	 * @return number of dimensions in the dataset
	 */
	size_t getDimension(std::string tfilename);

	/**
	 * Determine how many instances the dataset contains
	 *
	 * @param tfilename filename of the ARFF file
	 * @return number of instances in the dataset
	 */
	size_t getNumberInstances(std::string tfilename);

	/**
	 * reads an ARFF file (except the last attribute) and writes its content into a DataVector object
	 *
	 * @param tfilename the file's filename that should be opened
	 * @param destination reference to a DataVector object into which the data should be stored
	 */
	void readTrainingData(std::string tfilename, DataVector& destination);

	/**
	 * reads an ARFF file (only the last attribute) and writes its content into a DataVector object
	 *
	 * @param tfilename the file's filename that should be opened
	 * @param destination reference to a DataVector object into which the data should be stored
	 */
	void readClasses(std::string tfilename, DataVector& destination);

	/**
	 * writes the content of a DataVector into an ARFF File, used the Sparse Grid's coefficients
	 *
	 * @param tfilename the file's filename that should be written with the data
	 * @param source reference to a DataVector object that contains the data that should be stored
	 */
	//void writeAlpha(std::string tfilename, DataVector& source);

	/**
	 * reads the content of an ARFF File, used the Sparse Grid's coefficients
	 *
	 * @param tfilename the file's filename from with the data should be read
	 * @param destination reference to a DataVector object into which the data should be stored
	 */
	//void readAlpha(std::string tfilename, DataVector& destination);

	/**
	 * reads numeric data stored in a ARFF file.
	 *
	 * @param tfilename the file's filename from with the data should be read
	 * @param destination reference to a DataVector object into which the data should be stored
	 */
	//void readData(std::string tfilename, DataVector& destination);
};

}

#endif /* ARFFTOOLS_HPP */
