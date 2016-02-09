/*
 * ReachTube.h
 *
 *  Created on: Jul 24, 2014
 *      Author: parasara
 */

#ifndef REACHTUBE_H_
#define REACHTUBE_H_

#include <vector>
#include <iostream>
#include "Point.h"
#include "Annotation.h"
#include "InitialSet.h"
#include "LinearSet.h"
using namespace std;

class ReachTube{
private:
	int dimensions;
	int isReachTube;
	int reachTubeMode;
	std::vector<int> color;
	std::vector<int> mode;
	std::vector<class Point*> upperBound;
	std::vector<class Point*> lowerBound;

public:

	ReachTube();
	~ReachTube();
	void setDimensions(int dim);
	int getDimensions();
	void setMode(int mode);
	int getMode();
	void addUpperBoundState(class Point* upperBound);
	void addLowerBoundState(class Point* lowerBound);
	class Point* getUpperBoundState(int index);
	class Point* getLowerBoundState(int index);
	int getSize();
	class Point* getUpperBound(int index);
	class Point* getLowerBound(int index);
	void parseInvariantTube(char* filename);
	class ReachTube* bloatReachTube(double bloatingFactor);
	class ReachTube* bloatReachTube(double* deltaArray, class Annotation* currentAnnotation);
	void parseGuardsTube(char* filename);
	void printReachTube(char* filename, int flag);
	int getGuardMode(int index);
	void compressGuardSet();
	class InitialSet* samplingGuardSet();
	double getMaxCoordinate(int dimension, int mode);
	double getMinCoordinate(int dimension, int mode);
	vector<double> MaxCoordinate(int dimension);
	vector<double> MinCoordinate(int dimension);
	int checkunsafe(double* forM, double* forB, int numofeq);
	int checkunsafe_rest(double* forM, double* forB, int numofeq, int index);
	class InitialSet* getNextSet();
	int checkIntersection(int mode, class Point* currPoint, double* deltaArray);
	double getMinTime(int mode, class Point* currPoint, double* deltaArray);
	double getMaxTime(int mode, class Point* currPoint, double* deltaArray);


};

#endif /* REACHTUBE_H_ */
