/*
 * RepPoint.h
 *
 *  Created on: Feb 15, 2016
 *      Author: Bolun
 */

#ifndef REPPOINT_H_
#define REPPOINT_H_

#include <vector>

#include "point.hpp"

class RepPoint {
private:
	int mode;
	class Point* State;
	double* deltaArray;
	class Point* parentState;
	double* parentDeltaArray;
	int parentMode;
	int dimension;
	int refineTime;

public:
	RepPoint();
	~RepPoint();
	RepPoint(const class RepPoint &obj);
	RepPoint & operator= (const class RepPoint & obj);

	void setState(class Point* P);
	void setDeltaArray(std::vector<double>& value);
	void setMode(int modeval);
	void setParentState(class Point* P);
	void setParentDeltaArray(double* value);
	void setDimension(int d);
	void setRefineTime(int t);
	void setParentMode(int m);

	
	class Point* getState();
	double* getDeltaArray();
	int getMode();
	class Point* getParentState();
	double* getParentDeltaArray();
	int getDimension();
	int getRefineTime();
	int getParentMode();
	
	void print();
};

#endif 
