/*
 * Vector.h
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#ifndef VECTOR_H_
#define VECTOR_H_
#include "Point.h"

class Vector {
	private:
	int dimension;
	double* directions;

public:
	Vector();
	Vector(int dim);
	Vector(int dim, double* coArray);
	virtual ~Vector();

	void setDimension(int dim);
	void setDirection(int dim, double component);
	void setDirections(int dim, double* coArray);

	int getDimension();
	double* getDirections();
	double getDirection(int dim);

	double magnitude();
	double dotProduct(class Vector* refVec);
	void scalarProduct(double value);
	void subtract(class Vector* refVec);

	void addPoint(class Point* sourcePoint, class Point* resultPoint);

	class Vector* getOrthogonal();

	void normalize();
	class Vector** getAllOrthogonal();

};

class Vector* getVector(class Point* ptr1, class Point* pr2);
class Vector** GramSchmidt(class Vector**, int, int);

#endif /* VECTOR_H_ */
