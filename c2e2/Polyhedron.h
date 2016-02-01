/*
 * Polyhedron.h
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include "Point.h"
#include "Vector.h"

#ifndef POLYHEDRON_H_
#define POLYHEDRON_H_

class Polyhedron {
private:
	int numPoints;
	class Point** SetPtr;

public:
	Polyhedron();
	Polyhedron(int num);
	virtual ~Polyhedron();
	void setSize(int num);
	void setPoints(int num, class Point** SetPtr);
	void setPoint(int index, class Point* pointPtr);

	int getSize();
	class Point** getPoints();
	class Point* getPoint(int index);
};

class Polyhedron* createPolyhedron(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon);

class Polyhedron* createExternalPolyhedron(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon);

class Polyhedron* createNewPoly(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon);

#endif /* POLYHEDRON_H_ */
