/*
 * LinearSet.h
 *
 *  Created on: Jul 28, 2014
 *      Author: parasara
 */

#ifndef LINEARSET_H_
#define LINEARSET_H_

#include<iostream>
#include<cstdlib>
#include"Point.h"
#include"InitialSet.h"
#include"CoverStack.h"

using namespace std;

class LinearSet {
private:
	int dimensions;
	int numberOfEquations;
	double* matrix;
	double* b;

public:
	LinearSet();
	~LinearSet();
	void setDimensions(int dim);
	int getDimensions();
	void setNumEqns(int numeqns);
	int getNumEqns();
	void setMatrix(double* M);
	double* getMatrix();
	void setB(double* B);
	double* getB();
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
	class CoverStack* getCoverStack(double *deltaArray, int mode, int refineTime);

};

#endif /* LINEARSET_H_ */
