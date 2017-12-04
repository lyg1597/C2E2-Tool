/*
 * InitialSet.cpp
 *
 *  Created on: Mar 14, 2012
 *      Author: parasara
 */

#include "initial-set.hpp"

#include <iostream>
#include <cstdlib>
using namespace std;

InitialSet::InitialSet() {
	// Auto-generated constructor stub
	State = NULL;
	deltaArray = NULL;
	mode = -1;
	next = NULL;
}

InitialSet::~InitialSet() {
	// Auto-generated destructor stub
	delete [] deltaArray;

}

void InitialSet::setState(class Point* P){
	State = P;
	next = NULL;
}

void InitialSet::setDelta(double* value){
	//cout<<"Point Dimension is "<<State->getDimension()<<endl;
	int arraysize =State->getDimension()-1;
	int i;
	deltaArray = new double[arraysize];
	if(next == NULL){
		double tempArray[arraysize];
		for (i = 0; i <arraysize; i++)
			deltaArray[i] = value[i];
	}
	else{
		next->setDelta(value);
	}
	for (i=0; i<arraysize; i++)
		deltaArray[i] = value[i];
}

void InitialSet::setMode(int modeVal){
	if(next == NULL){
		mode = modeVal;
	}
	else{
		next->setMode(modeVal);
	}
	mode = modeVal;
}

void InitialSet::setNext(class InitialSet* N){
	next = N;
}

class Point* InitialSet::getState(){
	return State;
}

double* InitialSet::getDelta(){
	return deltaArray;
}

int InitialSet::getMode(){
	return mode;
}

class InitialSet* InitialSet::getNext(){
	return next;
}

void InitialSet::append(class InitialSet* L){
	if(next == NULL)
		next = L;
	else
		next->append(L);
}

void InitialSet::add(class Point* P){
	if(State == NULL){
		State = P;
	}
	else if(next == NULL){
		class InitialSet* tempSet = new InitialSet();
		tempSet->setState(P);
		tempSet->setMode(mode);
		next = tempSet;
	}
	else{
		next->add(P);
	}

}

int InitialSet::getLength(){
	if(State == NULL)
		return 0;
	else if(next == NULL)
		return 1;
	else
		return next->getLength() + 1 ;
}

void InitialSet::print(){
	if(State == NULL){
		return;
	}
	else{
		cout<<mode<<endl;
		int dim;
		cout << "[ ";
		for(dim = 0; dim < State->getDimension(); dim++){
			cout << State->getCoordinate(dim) << " ";
		}
		cout << " ]" << endl;
		if(next != NULL){
			next->print();
		}
	}
}
