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
#include <ppl.hh>
#include "point.hpp"
#include "annotation.hpp"
#include "initial-set.hpp"
#include "linear-set.hpp"
#include "cover-stack.hpp"
#include "rep-point.hpp"

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
	class Point* getUpperBoundState(size_t index);
	class Point* getLowerBoundState(size_t index);
	int getSize();
	class Point* getUpperBound(int index);
	class Point* getLowerBound(int index);
	void parseInvariantTube(char const* filename, int hasMode);
	class ReachTube* bloatReachTube(double bloatingFactor);
	class ReachTube* bloatReachTube(double* deltaArray, class Annotation* currentAnnotation);
	void parseGuardsTube(char* filename);
	//void printReachTube(char const* filename, int flag);
	void printReachTube(const std::string, int flag);
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
	int getNextSetStack(class CoverStack* ItrStack, class RepPoint* parentRepPoint);
	void clear(int from);
	void addGuards(double *ptLower, vector<pair<int, double *> > guards);
	void addGuards(vector<pair<NNC_Polyhedron, int> > guards);
	void printGuards();

};

#endif /* REACHTUBE_H_ */
