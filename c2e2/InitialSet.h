/*
 * InitialSet.h
 *
 *  Created on: Mar 14, 2012
 *      Author: parasara
 */

#ifndef INITIALSET_H_
#define INITIALSET_H_

#include "Point.h"

class InitialSet {
private:
	int mode;
	class Point* State;
	double delta;
	class InitialSet* next;

public:
	InitialSet();
	virtual ~InitialSet();
	void setState(class Point* P);
	void setDelta(double value);
	void setMode(int mode);
	void setNext(class InitialSet* N);
	class Point* getState();
	double getDelta();
	int getMode();
	class InitialSet* getNext();
	void append(class InitialSet* L);
	void add(class Point* P);
	int getLength();
	void print();
};

#endif /* INITIALSET_H_ */
