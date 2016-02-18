/*
 * Point.h
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#ifndef POINT_H_
#define POINT_H_

class Point {
private:
	int dimension;
	double* coordinates;

public:
	Point();
	Point(int dim);
	Point(int dim, double* coArray);
	Point(const class Point &obj);
	Point & operator= (const class Point & other);
	~Point();

	void setDimension(int dim);
	void setCoordinates(double* coArray);
	void setCoordinate(int index, double coordinate);

	int getDimension();
	double* getCoordinates();
	double getCoordiate(int index);


};

#endif /* POINT_H_ */
