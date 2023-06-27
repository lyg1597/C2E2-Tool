/*
 * RepPoint.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: Bolun
 */

#include <iostream>
#include <cstdlib>

#include "rep-point.hpp"

using namespace std;

RepPoint::RepPoint() {
	State = NULL;
	deltaArray = NULL;
	mode = -1;
	parentState = NULL;
	parentDeltaArray = NULL;
	parentMode = 0;
	dimension = -1;
	refineTime = 0;

}

RepPoint::~RepPoint() {
	delete [] deltaArray;
	delete [] parentDeltaArray;
	delete State;
	delete parentState;

}

RepPoint::RepPoint(const class RepPoint &obj){
	dimension = obj.dimension;
	mode = obj.mode;
	refineTime = obj.refineTime;
	parentMode = obj.parentMode;
	int i;
	deltaArray = new double[dimension];
	for(i=0;i<dimension;i++)
		deltaArray[i] = obj.deltaArray[i];
	State = new Point(*obj.State);

	if (obj.parentDeltaArray!=NULL){
		parentDeltaArray = new double[dimension];
		for(i=0;i<dimension;i++)
			parentDeltaArray[i] = obj.parentDeltaArray[i];
		//cout<<obj.parentDeltaArray[i]<<endl;
	}
		
	if (obj.parentState!=NULL)
		parentState = new Point(*obj.parentState);


}


RepPoint & RepPoint :: operator= (const class RepPoint & obj){
	if (this != &obj){
		delete [] deltaArray;
		delete [] parentDeltaArray;
		delete State;
		delete parentState;
		dimension = obj.dimension;
		mode = obj.mode;
		refineTime = obj.refineTime;
		parentMode = obj.parentMode;
		int i;
		deltaArray = new double[dimension];
		for(i=0;i<dimension;i++)
			deltaArray[i] = obj.deltaArray[i];
		State = new Point(*obj.State);

		if (obj.parentDeltaArray!=NULL){
			parentDeltaArray = new double[dimension];
			for(i=0;i<dimension;i++)
				parentDeltaArray[i] = obj.parentDeltaArray[i];
			//cout<<obj.parentDeltaArray[i]<<endl;
		}
			
		if (obj.parentState!=NULL)
			parentState = new Point(*obj.parentState);

	}
       

       return *this;
}



void RepPoint::setState(class Point* P){
	if (State!= NULL)
		delete State;

	//cout<<P->getDimension()<<endl;
	State = new Point(*P);
}

void RepPoint::setDeltaArray(std::vector<double>& value){
	if (dimension == -1)
		cout << "ERROR! Please set dimension before set deltaArray"<<endl;

	if (deltaArray!=NULL)
		delete[] deltaArray;

	deltaArray = new double[dimension];
	for(int i = 0; i<dimension; i++)
		deltaArray[i] = value[i];
}

void RepPoint::setMode(int modeval){
	mode = modeval;
}

void RepPoint::setParentState(class Point* P){
	if (parentState!=NULL)
		delete parentState;

	parentState = new Point(*P);
}

void RepPoint::setParentDeltaArray(double* value){
	if (dimension == -1)
		cout << "ERROR! Please set dimension before set ParentDeltaArray"<<endl;

	if (parentDeltaArray!=NULL)
		delete[] parentDeltaArray;

	parentDeltaArray = new double[dimension];
	for(int i = 0; i < dimension; i++)
		parentDeltaArray[i] = value[i];
}

void RepPoint::setParentMode(int m){
	parentMode = m;
}

void RepPoint::setDimension(int d){
	dimension = d;
}


void RepPoint::setRefineTime(int t){
	refineTime = t;
}


class Point* RepPoint::getState(){
	return State;
}

double* RepPoint::getDeltaArray(){
	return deltaArray;
}

int RepPoint::getMode(){
	return mode;
}

class Point* RepPoint::getParentState(){
	return parentState;
}

double* RepPoint::getParentDeltaArray(){
	return parentDeltaArray;
}

int RepPoint::getParentMode(){
	return parentMode;
}

int RepPoint::getDimension(){
	return dimension;
}

int RepPoint::getRefineTime(){
	return refineTime;
}

void RepPoint::print(){
	if(State == NULL){
		return;
	}
	else{

		cout<<"************************RepPoint Information*****************************"<<endl;
		cout<<"Current Mode "<<mode<<endl;
		cout<<"Dimension "<<dimension<<endl;
		cout<<"refineTime "<<refineTime<<endl;

		int dim;

		cout << "State [ ";
		for(dim = 0; dim < State->getDimension(); dim++){
			cout << State->getCoordinate(dim) << " ";
		}
		cout << " ]" << endl;
		
		cout << "DeltaArray [ ";
		for(dim = 0; dim<dimension;dim++){
			cout << deltaArray[dim] <<" ";
		}
		cout << " ]" <<endl;

		if (parentState != NULL){
			cout << "ParentState [ ";
			for(dim = 0; dim < parentState->getDimension(); dim++){
				cout << parentState->getCoordinate(dim) << " ";
			}
			cout << " ]" << endl;
		}

		if (parentDeltaArray!= NULL){
			cout << "ParentDeltaArray [ ";
			for(dim = 0; dim<dimension;dim++){
				cout << parentDeltaArray[dim] <<" ";
			}
			cout << " ]" <<endl;
		}

		cout<<"*************************************************************************"<<endl;

	}
}
