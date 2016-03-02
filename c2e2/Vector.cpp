/*
 * Vector.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include "Vector.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;

Vector::Vector() {
	// TODO Auto-generated constructor stub
dimension = 0;
directions = NULL;
}

Vector::Vector(int dim){
	dimension = dim;
	directions = new double[dimension]; // (double*)calloc(dimension,sizeof(double));
	int i;
	for(i=0;i<dimension;i++){
		*(directions+i) = 0;
	}
}

Vector::Vector(int dim, double* coArray){
	dimension = dim;
	directions = (double*)calloc(dimension,sizeof(double));
	int i;
	for(i=0;i<dimension;i++){
		*(directions+i) = *(coArray+i);
	}
}

Vector::~Vector() {
	// TODO Auto-generated destructor stub
	dimension = 0;
	if(directions!= NULL){
		free(directions);
	}
}

void Vector::setDimension(int dim){
	dimension = dim;
	if(directions!= NULL)
	 free(directions);
	directions = (double*)calloc(dimension, sizeof(double));
}

void Vector::setDirection(int dim, double component){
	*(directions+dim) = component;
}

void Vector::setDirections(int dim, double* coArray){
	dimension = dim;
	if(directions != NULL)
		free(directions);
	directions = (double*)calloc(dimension,sizeof(double));
	int i;
	for(i=0;i<dim;i++){
		*(directions+i) = *(coArray+i);
	}
}

int Vector::getDimension(){
	return dimension;
}

double* Vector::getDirections(){
	return directions;
}

double Vector::getDirection(int dim){
	return *(directions+dim);
}

double Vector::magnitude(){
	double retVal = 0;
	for(int index = 0; index < dimension; index++)
		retVal+= this->getDirection(index)*this->getDirection(index);
	return sqrt(retVal);
}

double Vector::dotProduct(class Vector* refVec){
	if(dimension != refVec->getDimension()){
		cerr << "Wrong dimensions for dot product! \n";
		exit(0);
	}
	double retVal = 0;
	int index;
	for(index = 0; index < dimension; index++){
		retVal += this->getDirection(index)*refVec->getDirection(index);
	}

	return retVal;
}

void Vector::scalarProduct(double value){
	for(int index=0;index < dimension;index++)
		this->setDirection(index,this->getDirection(index)*value);
}

void Vector::subtract(class Vector* refVec){
	if(dimension != refVec->getDimension()){
		cerr << "Wrong dimensions for subtraction! \n";
		exit(0);
	}
	for(int index=0;index < dimension;index++)
		this->setDirection(index,this->getDirection(index)-refVec->getDirection(index));
}

void Vector::addPoint(class Point* sourcePoint, class Point* resultPoint){
	int index;
	if(dimension != sourcePoint->getDimension() || dimension != resultPoint->getDimension()){
		cerr << "Error in dimensions while adding point and vector! \n";
		exit(1);
	}
	for(index=0;index<dimension;index++){
		resultPoint->setCoordinate(index,this->getDirection(index) + sourcePoint->getCoordinate(index));
	}
}

class Vector* Vector::getOrthogonal(){

	Vector* orthVec = new Vector(dimension);
	int i;
	for(i=0;i<dimension-1;i++){
		if(*(directions+i) == 0){
			orthVec->setDirection(i,1);
		}
		else{
			orthVec->setDirection(i,1/((double)*(directions+i)));
		}
	}
	double restsum = 0;
	for(i=0;i<dimension-1;i++){
		restsum += (double)(*(directions+i))*orthVec->getDirection(i);
	}
	double lastcoeff = *(directions+dimension-1);
	if(lastcoeff == 0){
		orthVec->setDirection(dimension-1,0);
	}
	else{
		orthVec->setDirection(dimension-1,-1*restsum/lastcoeff);
	}

	return orthVec;

}

void Vector::normalize(){
	double magnitude = 0;
	double value;
	int index;
	for(index = 0; index < dimension; index++){
		value = 0;
		value = *(directions+index);
		magnitude +=  value*value;
	}

	magnitude = sqrt(magnitude);

	for(index=0;index < dimension; index++){
		value = 0;
		value = *(directions+index);
		value = value/magnitude;
		*(directions+index) = value;
	}
}

class Vector** Vector::getAllOrthogonal(){
	class Vector** SetPtr;
	class Vector* TempPtr;
	SetPtr = (class Vector**)calloc(dimension-1, sizeof(class Vector*));
	int numCount = 0;
	int index;
	int nonZero = 0;
	int nZindex;
	for(index = 0;index<dimension;index++){
		if(*(directions+index) == 0){
			TempPtr = new Vector(dimension);
			TempPtr->setDirection(index,1);
			*(SetPtr+numCount) = TempPtr;
			numCount++;
		}
	}
	if(numCount == dimension-1){
		return SetPtr;
	}
	nonZero = dimension-numCount;
	double nZcoeffs[nonZero];
	for(index=0,nZindex=0;index<dimension;index++){
		if(*(directions+index) != 0){
			nZcoeffs[nZindex] = *(directions+index);
			nZindex++;
		}
	}
	double coeff1,coeff2;
	int secondCoeffIndex;
	secondCoeffIndex = nonZero-1;
	while(numCount < dimension-1){
		coeff1 = 0; coeff2 = 0;
		coeff1 = 1;
		coeff2 = -1*(nZcoeffs[0]/nZcoeffs[secondCoeffIndex]);
		TempPtr = new Vector(dimension);
		int countNonZero = 0;
		for(index = 0; index < dimension;index++){
			if(*(directions+index) != 0){
				countNonZero++;
				if(countNonZero == 1){
					TempPtr->setDirection(index,coeff1);
				}
				if(countNonZero == secondCoeffIndex+1){
					TempPtr->setDirection(index,coeff2);
				}
			}
		}
		*(SetPtr+numCount) = TempPtr;
		numCount++;
		secondCoeffIndex--;
	}
	return SetPtr;
}

class Vector* getVector(class Point* ptr1, class Point* ptr2){
	if(ptr1->getDimension() != ptr2->getDimension()){
		cerr << " Mismatch in dimensions while creating the vector! \n";
		exit(1);
	}
	class Vector* retVal = new Vector(ptr1->getDimension());
	for(int index=0;index<ptr1->getDimension();index++){
		retVal->setDirection(index,ptr2->getCoordinate(index)-ptr1->getCoordinate(index));
	}

	return retVal;
}

class Vector** GramSchmidt(class Vector** SetVec, int number, int dim){
	int index1, index2;
	double component1, component2;
	class Vector* TempPtr = new Vector(dim);
	class Vector* DotVector = new Vector(dim);
	class Vector* TempVector1, *TempVector2;
	for(index1=0;index1<number;index1++){
		TempVector1 = (class Vector*)*(SetVec+index1);
		DotVector->setDirections(TempVector1->getDimension(),TempVector1->getDirections());
		for(index2=0;index2<index1;index2++){
			TempVector2 = (class Vector*)*(SetVec+index2);
			TempPtr->setDirections(TempVector2->getDimension(),TempVector2->getDirections());
			component1 = TempPtr->dotProduct(DotVector);
			component2 = TempPtr->magnitude()*TempPtr->magnitude();
			TempPtr->scalarProduct(component1/component2);
			TempVector1->subtract(TempPtr);
		}
	}
	for(index1=0;index1<number;index1++){
		TempVector1 = (class Vector*)*(SetVec+index1);
		TempVector1->normalize();
	}

	return SetVec;
}
