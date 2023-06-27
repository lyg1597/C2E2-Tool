/*
 * LinearSet.h
 *
 *  Created on: Jul 28, 2014
 *      Author: parasara
 */

#ifndef LINEARSET_H_
#define LINEARSET_H_

#include <iostream>
#include <cstdlib>
#include <vector>

#include "point.hpp"
#include "initial-set.hpp"
#include "cover-stack.hpp"

using namespace std;

class LinearSet {
private:
	int dimensions;
	int numberOfEquations;
    std::vector<double> matrix;
    std::vector<double> b;

public:
	LinearSet();
	~LinearSet();
	void setDimensions(int dim);
	int getDimensions();
	void setNumEqns(int numeqns);
	int getNumEqns();

    void setMatrix(std::vector<double>& mat);
    std::vector<double>& getMatrix();

    void setB(std::vector<double>& b_in);
    std::vector<double>& getB();

	void setMatrixElement(int row, int col, double value);
	double getMatrixElement(int row, int col);
	void setBElement(int index, double value);
	double getBElement(int index);
	int isInternal(class Point* RefPoint);
	int hasIntersection(class Point* RefPoint, double *deltaArray);
	int hasIntersection(class Point* PtL, class Point* PtU);
	double getMax(int dimID);
	double getMin(int dimID);
	class InitialSet* getCover(double* deltaArray);
	class CoverStack* getCoverStack(std::vector<double>& delta_arr, int mode, 
            int refineTime);
	//class CoverStack* getCoverStack(double *deltaArray, int mode, int refineTime);

};

#endif /* LINEARSET_H_ */
