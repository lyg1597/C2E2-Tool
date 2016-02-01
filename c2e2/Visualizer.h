/*
 * Visualizer.h
 *
 *  Created on: Feb 3, 2013
 *      Author: parasara
 */

#ifndef VISUALIZER_H_
#define VISUALIZER_H_

#include "InitialSet.h"

class Visualizer {
private:
	class InitialSet* TemporaryPoints;
	class InitialSet* VisualizePoints;
	int dim1,dim2;
	char* filename;
public:
	Visualizer();
	virtual ~Visualizer();
	void setTempPoints(class InitialSet* temp);
	class InitialSet* getTempPoints();
	void setVisualizePoints(class InitialSet* temp);
	class InitialSet* getVisualizePoints();
	void setDim1(int value);
	int getDim1();
	void setDim2(int value);
	int getDim2();
	void setFileName(char* string);
	char* getFileName();
	void rehashVisualPoints();
	void plotUnsafe(class Point* UnsafePoint1, class Point* UnsafePoint2, double GT);
	void Plot();

};

#endif /* VISUALIZER_H_ */
