/*
 * Polyhedron.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include "Polyhedron.h"
#define PI 3.14159265

#include<iostream>
#include<cstdlib>
#include<math.h>
#include<vector>
using namespace std;

Polyhedron::Polyhedron() {
	// TODO Auto-generated constructor stub
	numPoints = 0;
	SetPtr = NULL;
}

Polyhedron::Polyhedron(int num){
	numPoints = num;
	SetPtr = new Point*[num]; //(class Point**)calloc(num,sizeof(class Point*));
	for(int index=0;index<num;index++){
		*(SetPtr+index) = NULL;
	}
}

Polyhedron::~Polyhedron() {
	// TODO Auto-generated destructor stub
	int index;
	for(index=0;index < numPoints;index++)
		free(*(SetPtr+index));
	free(SetPtr);
}

void Polyhedron::setSize(int num){
	numPoints = num;
	SetPtr = new Point*[num]; //(class Point**)calloc(num,sizeof(class Point*));
	for(int index=0;index<num;index++){
		*(SetPtr+index) = NULL;
	}
}

void Polyhedron::setPoints(int num, class Point** CollPtr){
	if(num != numPoints){
		for(int index=0;index < numPoints;index++)
			free(*(SetPtr+index));
		free(SetPtr);
		SetPtr = new Point*[num]; //(class Point**)calloc(num,sizeof(class Point*));
	}
	class Point* TempPoint;

	for(int index=0;index<num;index++){
		if(*(SetPtr+index) != NULL) free(*(SetPtr+index));
		TempPoint = *(CollPtr+index);
		*(SetPtr+index) = new Point(TempPoint->getDimension(),TempPoint->getCoordinates());
	}
}

void Polyhedron::setPoint(int index, class Point* pointPtr){
	if(*(SetPtr+index) != NULL) free(*(SetPtr+index));
	*(SetPtr+index) = new Point(pointPtr->getDimension(),pointPtr->getCoordinates());
}

int Polyhedron::getSize(){
	return numPoints;
}

class Point** Polyhedron::getPoints(){
	return SetPtr;
}

class Point* Polyhedron::getPoint(int index){
	return *(SetPtr+index);
}

class Polyhedron* createPolyhedron(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon){
	if(Ptr1->getDimension() != Ptr2->getDimension()){
		cerr << "Error while creating polyhedron, mismatch of dimensions \n";
		exit(1);
	}
	int dimension = Ptr1->getDimension();
	class Vector* dirVector;
	dirVector = getVector(Ptr1,Ptr2);
	class Vector** SetPtr;
	SetPtr = dirVector->getAllOrthogonal();
	GramSchmidt(SetPtr,dimension-1,dimension);
	int index;
	class Vector* TempPtr;
	class Polyhedron* retPoly = new Polyhedron(2*depth*(dimension-1));
	class Point* bufferPoint = new Point(dimension);
	for(index=0;index<dimension-1;index++){
		TempPtr = *(SetPtr+index);
		TempPtr->scalarProduct(epsilon);
	}
	for(index=0;index<dimension-1;index++){
		TempPtr = *(SetPtr+index);
		TempPtr->addPoint(Ptr1,bufferPoint);
		retPoly->setPoint(2*index,bufferPoint);
		TempPtr->scalarProduct(-1);
		TempPtr->addPoint(Ptr1,bufferPoint);
		retPoly->setPoint(2*index+1,bufferPoint);
	}
	for(index=0;index<dimension-1;index++){
		TempPtr = *(SetPtr+index);
		TempPtr->addPoint(Ptr2,bufferPoint);
		retPoly->setPoint(2*index+2*(dimension-1),bufferPoint);
		TempPtr->scalarProduct(-1);
		TempPtr->addPoint(Ptr2,bufferPoint);
		retPoly->setPoint(2*index+1+2*(dimension-1),bufferPoint);
	}
	return retPoly;
}

class Polyhedron* createExternalPolyhedron(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon){

	int dimension;
	dimension = Ptr1->getDimension();
	vector < vector <class Point*> > unitpoly;
	int dim;
	dim = dimension;

	class Point* TempPoint;
	class Point* LessPoint;

	double radius;
	double degree;
	double dInc;
	double curValue;
	double mulValue;

	int n;
	n = depth;
	int curDim;
	int index;

	dInc = PI/n;
	unitpoly.resize(dim);
	for(index=0; index < dim-1; index++){
		curDim = index+1;
		if(curDim == 1){
			TempPoint = new Point(curDim);
			TempPoint->setCoordinate(0,1.0);
			unitpoly[index].push_back(TempPoint);
			TempPoint = new Point(curDim);
			TempPoint->setCoordinate(0,-1.0);
			unitpoly[index].push_back(TempPoint);
		}
		else if(curDim == 2){
			radius = 1/cos(dInc/2);
			for(degree = 0; degree < 2*PI; degree += dInc){
				TempPoint = new Point(curDim);
				TempPoint->setCoordinate(0,radius*sin(degree));
				TempPoint->setCoordinate(1,radius*cos(degree));
				unitpoly[index].push_back(TempPoint);
			}
		}
		else{
			radius = 1/cos(dInc/2);
			TempPoint = new Point(curDim);
			TempPoint->setCoordinate(curDim-1,radius);
			unitpoly[index].push_back(TempPoint);
			TempPoint = new Point(curDim);
			TempPoint->setCoordinate(curDim-1,-1*radius);
			unitpoly[index].push_back(TempPoint);
			for(degree = dInc; degree < PI ; degree += dInc){
				curValue = radius*cos(degree);
				mulValue = radius*sin(degree);
				for(int i = 0; i < unitpoly[index-1].size(); i++){
					LessPoint = unitpoly[index-1].at(i);
					TempPoint = new Point(curDim);
					TempPoint->setCoordinate(curDim-1,curValue);
					for(int j = 0; j < curDim-1; j++){
						TempPoint->setCoordinate(j,mulValue*LessPoint->getCoordinate(j));
					}
					unitpoly[index].push_back(TempPoint);
				}
			}
		}
	}

	double dist, maxdist, HD;
	maxdist = 0;
	for(int i = 0 ; i < unitpoly[dim-2].size(); i++){
		dist = 0;
		TempPoint = unitpoly[dim-2].at(i);
		for(int j = 0; j < dim-1; j++){
			dist = TempPoint->getCoordinate(j)*TempPoint->getCoordinate(j);
			if(maxdist < dist)
				maxdist = dist;
		}
	}
	HD = sqrt(maxdist) -1 ;


	int countNumber;
	int components[dimension-1];
	countNumber = pow(2,dimension-1);
	int uPolySize = unitpoly[dim-2].size();
	countNumber = 2*unitpoly[dim-2].size();
	int uPolyIndex = dim-2;
	int pointsIndex;
	int dupPointsIndex;
	int dimIndex;

	class Polyhedron* retPoly = new Polyhedron(countNumber);
	class Vector* dirVector;
	dirVector = getVector(Ptr1,Ptr2);
	class Vector** SetPtr;
	SetPtr = dirVector->getAllOrthogonal();
	GramSchmidt(SetPtr,dimension-1,dimension);
	class Vector* TempVector;
	class Vector* TempVector2;

	class Point* TempPoint2;
	TempVector2 = new Vector(dim);

	for(pointsIndex = 0; pointsIndex<uPolySize;pointsIndex++){

		TempPoint = new Point(dimension);
		TempPoint2 = unitpoly[uPolyIndex].at(pointsIndex);
		TempPoint->setCoordinates(Ptr1->getCoordinates());
		for(dimIndex=0;dimIndex<dim-1;dimIndex++){
			TempVector = *(SetPtr+dimIndex);
			TempVector2->setDirections(dim,TempVector->getDirections());
			TempVector2->scalarProduct(TempPoint2->getCoordinate(dimIndex));
			TempVector2->scalarProduct(epsilon);
			TempVector2->addPoint(TempPoint,TempPoint);
		}
		retPoly->setPoint(pointsIndex,TempPoint);
	}

	for(pointsIndex = uPolySize; pointsIndex<countNumber;pointsIndex++){

		TempPoint = new Point(dimension);
		TempPoint2 = unitpoly[uPolyIndex].at(pointsIndex-uPolySize);
		TempPoint->setCoordinates(Ptr2->getCoordinates());
		for(dimIndex=0;dimIndex<dim-1;dimIndex++){
			TempVector = *(SetPtr+dimIndex);
			TempVector2->setDirections(dim,TempVector->getDirections());
			TempVector2->scalarProduct(TempPoint2->getCoordinate(dimIndex));
			TempVector2->scalarProduct(epsilon);
			TempVector2->addPoint(TempPoint,TempPoint);
		}
		retPoly->setPoint(pointsIndex,TempPoint);
	}

	return retPoly;

	int divider = 2;

	for(pointsIndex=0;pointsIndex<countNumber;pointsIndex++){
		dupPointsIndex = pointsIndex;

		for(int index=0;index<dimension-1;index++){
			components[index] = pointsIndex%divider;
			pointsIndex = pointsIndex/divider;
		}

		pointsIndex = dupPointsIndex;
		TempPoint = new Point(dimension);
		TempPoint->setCoordinates(Ptr1->getCoordinates());
		for(int index=0;index<dimension-1;index++){
			TempVector = *(SetPtr+index);
			if(components[index] == 0)
				TempVector->scalarProduct(-1);
			TempVector->addPoint(TempPoint,TempPoint);
			if(components[index] == 0)
				TempVector->scalarProduct(-1);
		}
		retPoly->setPoint(pointsIndex,TempPoint);
	}

	for(pointsIndex=0;pointsIndex<countNumber;pointsIndex++){
		dupPointsIndex = pointsIndex;

		for(int index=0;index<dimension-1;index++){
			components[index] = pointsIndex%divider;
			pointsIndex = pointsIndex/divider;
		}

		pointsIndex = dupPointsIndex;
		TempPoint = new Point(dimension);
		TempPoint->setCoordinates(Ptr2->getCoordinates());
		for(int index=0;index<dimension-1;index++){
			TempVector = *(SetPtr+index);
			if(components[index] == 0)
				TempVector->scalarProduct(-1);
			TempVector->addPoint(TempPoint,TempPoint);
			if(components[index] == 0)
				TempVector->scalarProduct(-1);
		}
		retPoly->setPoint(countNumber+pointsIndex,TempPoint);
	}

	return retPoly;

}

class Polyhedron* createNewPoly(class Point* Ptr1, class Point* Ptr2, int depth, double epsilon){

	int dim;
	dim = Ptr1->getDimension();
	vector < vector <class Point*> > unitpoly;

	class Point* TempPoint;
	class Point* LessPoint;

	double radius;
	double degree;
	double dInc;
	double curValue;
	double mulValue;

	int n;
	n = depth;
	int curDim;
	int index;
	dim = 6;
	for(n=2;n<21;n++){
		cout << "Refinement number = "<<n<<" \n";
		dInc = PI/n;
		unitpoly.resize(dim);
		for(index=1; index < dim; index++){
			curDim = index+1;
			if(curDim == 2){
				radius = 1/cos(dInc/2);
				for(degree = 0; degree < 2*PI; degree += dInc){
					TempPoint = new Point(curDim);
					TempPoint->setCoordinate(0,radius*sin(degree));
					TempPoint->setCoordinate(1,radius*cos(degree));
					unitpoly[index].push_back(TempPoint);
				}
			}
			else{
				radius = 1/cos(dInc/2);
				TempPoint = new Point(curDim);
				TempPoint->setCoordinate(curDim-1,radius);
				unitpoly[index].push_back(TempPoint);
				TempPoint = new Point(curDim);
				TempPoint->setCoordinate(curDim-1,-1*radius);
				unitpoly[index].push_back(TempPoint);
				for(degree = dInc; degree < PI ; degree += dInc){
					curValue = radius*cos(degree);
					mulValue = radius*sin(degree);
					for(int i = 0; i < unitpoly[index-1].size(); i++){
						LessPoint = unitpoly[index-1].at(i);
						TempPoint = new Point(curDim);
						TempPoint->setCoordinate(curDim-1,curValue);
						for(int j = 0; j < curDim-1; j++){
							TempPoint->setCoordinate(j,mulValue*LessPoint->getCoordinate(j));
						}
						unitpoly[index].push_back(TempPoint);
					}
				}
			}

		}

		double maxDist;
		double Dist;
		double tempDim;
		maxDist = 0;
		Dist = 0;
		if(n%4 == 0){
			for(tempDim = 1; tempDim < dim; tempDim++){
				maxDist = 0;
				for(int i = 0 ; i < unitpoly[tempDim].size(); i++){
					Dist = 0;
					TempPoint = unitpoly[tempDim].at(i);
					for(int j = 0; j <= tempDim; j++){
						Dist += TempPoint->getCoordinate(j)*TempPoint->getCoordinate(j);
					}
					if(maxDist < Dist)
						maxDist = Dist;
				}
				maxDist = sqrt(maxDist);
				cout << " Dimension = "<<tempDim+1<<" - Hd = "<< maxDist-1<< " \n";
			}

		}
		unitpoly.clear();
	}


	exit(0);

	return NULL;
}
