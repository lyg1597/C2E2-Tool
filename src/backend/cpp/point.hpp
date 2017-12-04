/*
 * Point.h
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#ifndef POINT_H_
#define POINT_H_

using namespace std;

class Point {
private:
	int dimension;
	double* coordinates;

public:
	Point();
	Point(int dim);
	Point(int dim, double* coArray);
	Point(const class Point &obj);
	~Point();

	Point & operator= (const class Point & other);
	// bool operator==(Point& lhs);

	void setDimension(int dim);
	void setCoordinates(double* coArray);
	void setCoordinate(int index, double coordinate);

	int getDimension();
	double* getCoordinates();
	double getCoordinate(int index);

	void print();


};

#endif /* POINT_H_ */
