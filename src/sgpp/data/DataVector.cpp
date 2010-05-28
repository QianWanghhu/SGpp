/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Jörg Blank (blankj@in.tum.de), Alexander Heinecke (Alexander.Heinecke@mytum.de), Dirk Pflueger (Dirk.Pflueger@in.tum.de)

#include "data/DataVector.hpp"
#include <string.h>

#include <sstream>
#include <cmath>

#ifdef USEOMP
#include <omp.h>
#endif

DataVector::DataVector() {

}

DataVector::DataVector(size_t size) :
    size(size), dim(1), unused(0) {
    this->data = new double[size * dim];
}

DataVector::DataVector(size_t size, size_t dim) :
    size(size), dim(dim), unused(0) {
    this->data = new double[size * dim];
}

DataVector::DataVector(DataVector &vec) :
    unused(0) {
    this->size = vec.size;
    this->dim = vec.dim;
    this->data = new double[size * dim];

    memcpy(this->data, vec.data, size * dim * sizeof(double));
}

DataVector::DataVector(const DataVector &vec) :
    unused(0) {
    this->size = vec.size;
    this->dim = vec.dim;
    this->data = new double[size * dim];

    memcpy(this->data, vec.data, size * dim * sizeof(double));
}

DataVector::DataVector(double * input, size_t size, size_t dim) :
    size(size), dim(dim), unused(0) {
    this->data = new double[size * dim];
    memcpy(this->data, input, size * dim * sizeof(double));
}

DataVector::DataVector(DataVectorDefinition& DataVectorDef) {
    this->size = DataVectorDef.size;
    this->dim = DataVectorDef.dim;
    this->unused = DataVectorDef.unused;
    this->data = DataVectorDef.pointerToData;
}

void DataVector::getDataVectorDefinition(DataVectorDefinition& DataVectorDef) {
    DataVectorDef.size = this->size;
    DataVectorDef.dim = this->dim;
    DataVectorDef.unused = this->unused;
    DataVectorDef.pointerToData = this->data;
}

void DataVector::resize(size_t size) {
	// don't do anyhing, if vector is already bigger
    if ((int) size < this->size) {
        return;
    }

    // create new vector
    double* newdata = new double[size * this->dim];
    // copy entries of old vector
    memcpy(newdata, this->data, this->size * this->dim * sizeof(double));
    // set new elements to zero
    for (size_t i = this->size * this->dim; i < size * this->dim; i++) {
        newdata[i] = 0.0;
    }

    delete[] this->data;

    this->data = newdata;
    this->size = size;
}

void DataVector::addSize(int add) {
    // create new vector
    double* newdata = new double[(size + add) * this->dim];
    // copy entries of old vector
    memcpy(newdata, this->data, this->size * this->dim * sizeof(double));

    delete[] this->data;

    this->data = newdata;
    this->unused = add;
}

int DataVector::addValue() {
    if (unused == 0) {
        addSize(size);
    }

    int x = size;

    size++;
    unused--;

    return x;
}

void DataVector::setAll(double value) {
    int n = size * dim;
    //#ifdef USEOMP
    //	#pragma omp parallel for schedule(static)
    //	for(int i = 0; i < n; i++)
    //	{
    //		data[i] = value;
    //	}
    //#else
    for (int i = 0; i < n; i++) {
        data[i] = value;
    }
    //#endif
}

double DataVector::get(int i) const {
    return data[i];
}

void DataVector::set(int i, double value) {
    data[i] = value;
}

void DataVector::getRow(int row, DataVector& vec) {
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < this->dim; i++)
     {
     vec[i] = data[row*dim+i];
     }
     #else*/
    for (int i = 0; i < this->dim; i++) {
        vec[i] = data[row * dim + i];
    }
    //#endif
}

void DataVector::setRow(int row, DataVector& vec) {
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < this->dim; i++)
     {
     data[row*dim+i] = vec[i];
     }
     #else*/
    for (int i = 0; i < this->dim; i++) {
        data[row * dim + i] = vec[i];
    }
    //#endif
}

void DataVector::getColumn(int col, DataVector& vec) {
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int j = 0; j < this->size; j++)
     {
     vec[j] = data[j*dim+col];
     }
     #else*/
    for (int j = 0; j < this->size; j++) {
        vec[j] = data[j * dim + col];
    }
    //#endif
}

void DataVector::setColumn(int col, DataVector& vec) {
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int j = 0; j < this->size; j++)
     {
     data[j*dim+col] = vec[j];
     }
     #else*/
    for (int j = 0; j < this->size; j++) {
        data[j * dim + col] = vec[j];
    }
    //#endif
}

void DataVector::copyFrom(const DataVector& vec) {
    if (this == &vec) {
        return;
    }

    if (size != vec.size || dim != vec.dim) {
        delete[] data;
        size = vec.size;
        dim = vec.dim;
        this->data = new double[size * dim];
    }
    memcpy(this->data, vec.data, size * dim * sizeof(double));
}

void DataVector::copySmall(const DataVector& vec) {
    if (this == &vec) {
        return;
    }

    if (vec.dim != 1 || dim != 1 || size < vec.size) {
        return;
    }
    memcpy(this->data, vec.data, vec.size * sizeof(double));
}

DataVector& DataVector::operator=(const DataVector &vec) {
    if (this == &vec) {
        return *this;
    }

    if (size != vec.size || dim != vec.dim) {
        delete[] data;
        size = vec.size;
        dim = vec.dim;
        this->data = new double[size * dim];
    }
    memcpy(this->data, vec.data, size * dim * sizeof(double));
    return *this;
}

void DataVector::add(DataVector &vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;

    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < n; i++)
     {
     data[i] += vec.data[i];
     }
     #else*/
    for (int i = 0; i < n; i++) {
        data[i] += vec.data[i];
    }
    //#endif
}

void DataVector::sub(DataVector &vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;

    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < n; i++)
     {
     data[i] -= vec.data[i];
     }
     #else*/
    for (int i = 0; i < n; i++) {
        data[i] -= vec.data[i];
    }
    //#endif
}

void DataVector::add_parallel(DataVector &vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;

#ifdef USEOMP
#ifdef PARALLELDATAVECTOR
#pragma omp parallel for shared(vec) schedule(static)
    for(int i = 0; i < n; i++)
    {
        data[i] += vec.data[i];
    }
#else
    for(int i = 0; i < n; i++)
    {
        data[i] += vec.data[i];
    }
#endif
#else
    for (int i = 0; i < n; i++) {
        data[i] += vec.data[i];
    }
#endif
}

void DataVector::sub_parallel(DataVector &vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;

#ifdef USEOMP
#ifdef PARALLELDATAVECTOR
#pragma omp parallel for shared(vec) schedule(static)
    for(int i = 0; i < n; i++)
    {
        data[i] -= vec.data[i];
    }
#else
    for(int i = 0; i < n; i++)
    {
        data[i] -= vec.data[i];
    }
#endif
#else
    for (int i = 0; i < n; i++) {
        data[i] -= vec.data[i];
    }
#endif
}

void DataVector::componentwise_mult(DataVector& vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;
    for (int i = 0; i < n; i++) {
        data[i] *= vec.data[i];
    }
}

void DataVector::componentwise_div(DataVector& vec) {
    if (size != vec.size || dim != vec.dim) {
        return;
    }
    int n = size * dim;
    for (int i = 0; i < n; i++) {
        data[i] /= vec.data[i];
    }
}

void DataVector::getLine(int row, DataVector& vec) {
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < this->dim; i++)
     {
     vec[i] = data[row*dim+i];
     }
     #else*/
    for (int i = 0; i < this->dim; i++) {
        vec[i] = data[row * dim + i];
    }
    //#endif
}

void DataVector::getLine(int row, std::vector<double>& vec) {
    vec.clear();
    /*#ifdef USEOMP
     #pragma omp parallel for shared(vec) schedule(static)
     for(int i = 0; i < this->dim; i++)
     {
     vec.push_back(data[row*dim+i]);
     }
     #else*/
    for (int i = 0; i < this->dim; i++) {
        vec.push_back(data[row * dim + i]);
    }
    //#endif
}

size_t DataVector::getSize() {
    return size;
}

size_t DataVector::getDim() {
    return dim;
}

size_t DataVector::getTotalSize() {
    return dim * size;
}

double DataVector::dotProduct(DataVector &vec) {
    double sum = 0.0;

    for (int i = 0; i < size; i++) {
        sum += data[i] * vec.data[i];
    }
    return sum;
}

void DataVector::mult(double scalar) {
    int n = size * dim;
    /*#ifdef USEOMP
     #pragma omp parallel for schedule(static)
     for(int i = 0; i < n; i++)
     {
     data[i] *= scalar;
     }
     #else*/
    for (int i = 0; i < n; i++) {
        data[i] *= scalar;
    }
    //#endif
}

void DataVector::sqr() {
    int n = size * dim;
    /*#ifdef USEOMP
     #pragma omp parallel for schedule(static)
     for(int i = 0; i < n; i++)
     {
     data[i] = data[i] * data[i];
     }
     #else*/
    for (int i = 0; i < n; i++) {
        data[i] = data[i] * data[i];
    }
    //#endif
}

void DataVector::abs() {
    int n = size * dim;
    /*#ifdef USEOMP
     #pragma omp parallel for schedule(static)
     for(int i = 0; i < n; i++)
     {
     data[i] = std::abs(data[i]);
     }
     #else*/
    for (int i = 0; i < n; i++) {
        data[i] = std::abs(data[i]);
    }
    //#endif
}

double DataVector::sum() {
    int n = size * dim;
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        result += data[i];
    }
    return result;
}

void DataVector::partitionClasses(double border) {
    int n = size * dim;
    for (int i = 0; i < n; i++) {
        data[i] = data[i] > border ? 1.0 : -1.0;
    }
}

void DataVector::axpy(double alpha, DataVector& x) {
    if (size != x.size || dim != x.dim) {
        return;
    }
    int n = size * dim;
    double* p_x = x.data;
    double* p_d = data;
    /*#ifdef USEOMP
     #pragma omp parallel for shared(p_d, p_x) schedule(static)
     for(int i = 0; i < n; i++)
     {
     p_d[i] += alpha*p_x[i];
     }
     #else*/
    for (int i = 0; i < n; i++) {
        p_d[i] += alpha * p_x[i];
    }
    //#endif

}

void DataVector::axpy_parallel(double alpha, DataVector& x) {
    if (size != x.size || dim != x.dim) {
        return;
    }
    int n = size * dim;
    double* p_x = x.data;
    double* p_d = data;
#ifdef USEOMP
#ifdef PARALLELDATAVECTOR
#pragma omp parallel for shared(p_d, p_x) schedule(static)
    for(int i = 0; i < n; i++)
    {
        p_d[i] += alpha*p_x[i];
    }
#else
    for(int i = 0; i < n; i++)
    {
        p_d[i] += alpha*p_x[i];
    }
#endif
#else
    for (int i = 0; i < n; i++) {
        p_d[i] += alpha * p_x[i];
    }
#endif
}

void DataVector::normalizeDimension(int d) {
    normalizeDimension(d, 0.0);
}

void DataVector::normalizeDimension(int d, double border) {
    int n = size * dim;
    double min = data[d];
    double max = data[d];
    for (int i = d; i < n; i += dim) {
        if (min > data[i]) {
            min = data[i];
        }
        if (max < data[i]) {
            max = data[i];
        }
    }
    min -= border;
    max += border;
    double delta = max - min;
    for (int i = d; i < n; i += dim) {
        data[i] = (data[i] - min) / delta;
    }
}

void DataVector::toString(std::string& text) {
    std::stringstream str;
    int n = size * dim;

    str << "[";

    for (int i = 0; i < n; i++) {
        if (i != 0) {
            str << ",";
        }
        str << " " << data[i];
    }
    str << " ]";
    text = str.str();
}

std::string DataVector::toString() {
    std::string str;
    toString(str);
    return str;
}

#ifndef LARRABEE
double DataVector::min(int d) {
    int n = size * dim;
    double min = data[d];
    for (int i = d; i < n; i += dim) {
        if (min > data[i]) {
            min = data[i];
        }
    }
    return min;
}

double DataVector::min() {
    int n = size * dim;
    double min = data[0];
    for (int i = 1; i < n; i += 1) {
        if (min > data[i]) {
            min = data[i];
        }
    }
    return min;
}

double DataVector::max(int d) {
    int n = size * dim;
    double max = data[d];
    for (int i = d; i < n; i += dim) {
        if (max < data[i]) {
            max = data[i];
        }
    }
    return max;
}

double DataVector::max() {
    int n = size * dim;
    double max = data[0];
    for (int i = 1; i < n; i += 1) {
        if (max < data[i]) {
            max = data[i];
        }
    }
    return max;
}

void DataVector::minmax(int d, double* min, double* max) {
    int n = size * dim;
    double min_t = data[d];
    double max_t = data[d];
    for (int i = d; i < n; i += dim) {
        if (min_t > data[i]) {
            min_t = data[i];
        }
        if (max_t < data[i]) {
            max_t = data[i];
        }
    }
    (*min) = min_t;
    (*max) = max_t;
}
#endif

double* DataVector::getPointer() {
    return data;
}

DataVector::~DataVector() {
    delete[] data;
}

size_t DataVector::getNumberNonZero()
{
    size_t n = size * dim;
    size_t nonZero = 0;

    for (size_t i = 0; i < n; i++)
    {
        if (fabs(data[i]) > 0.0)
        {
            nonZero++;
        }
    }
    return nonZero;
}
