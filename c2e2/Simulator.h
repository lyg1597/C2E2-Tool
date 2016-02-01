/*
 * Simulator.h
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "Point.h"

class Simulator {
private:
	char* executable;
	double absError;
	double relError;
	double timeStep;
	double globalTime;

public:
	Simulator();
	virtual ~Simulator();
	void setExecutable(char* name);
	char* getExecutable();
	void setAbsError(double value);
	double getAbsError();
	void setRelError(double value);
	double getRelError();
	void setTimeStep(double value);
	double getTimeStep();
	void setGlobalTime(double value);
	double getGlobalTime();
	void Simulate(class Point* Ref);
	void Simulate(class Point* Ref, int mode);
};

#endif /* SIMULATOR_H_ */
