/*
 * Point.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include "Point.h"
#include <cstdlib>

Point::Point() {
	// TODO Auto-generated constructor stub
	dimension = 0;
	coordinates = NULL;
}

Point::~Point() {
	// TODO Auto-generated destructor stub
}

Point::Point(int dim){
	dimension = dim;
	coordinates = new double[dimension]; //(double*)calloc(dimension,sizeof(double));
	int i;
	for(i=0;i<dimension;i++){
		*(coordinates+i)=0;
	}
}

Point::Point(int dim, double* coArray){
	dimension = dim;
	coordinates = new double[dimension]; //(double*)calloc(dimension,sizeof(double));
	int i;
	for(i=0;i<dimension;i++){
		*(coordinates+i) = *(coArray+i);
	}
}

void Point::setDimension(int dim){
	dimension = dim;
	if(coordinates != NULL){
		free(coordinates);
	}
}

void Point::setCoordinates(double* coArray){
	int i;
	for(i=0;i<dimension;i++){
		*(coordinates+i) = *(coArray+i);
	}
}

void Point::setCoordinate(int index, double coordinate){
	if(dimension == 1){
		*coordinates = coordinate;
	}
	*(coordinates+index) = coordinate;
}

int Point::getDimension(){
	return dimension;
}

double* Point::getCoordinates(){
	return coordinates;
}

double Point::getCoordiate(int index){
	return *(coordinates+index);
}
