/*
 * CoverStack.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: Bolun
 */


#include <iostream>
#include <cstdlib>

#include "cover-stack.hpp"

using namespace std;

CoverStack::CoverStack() {
	//std::stack<class RepPoint*>  elems;
}

CoverStack::~CoverStack() {
	//delete elems;
}

void CoverStack::push(class RepPoint* P){
	elems.push(P);
}

void CoverStack::pop(){
	if(!elems.empty())
		elems.pop();
}

class RepPoint* CoverStack::top(){
	return elems.top();
}


int CoverStack::size(){
	return elems.size();
}

bool CoverStack::empty(){
	return elems.empty();
}

void CoverStack::refine(class RepPoint *P, int dimindex){


	int dim = P->getDimension();

	if(dimindex>=dim || dimindex<0){
		cout<<"Invaild dimindex"<<endl;
		return;
	}

	
	double max[dim];
	double min[dim];
	double* deltaArray;
	class Point* tempPoint;
	int initMode;
	int reft = P->getRefineTime();
	if (P->getParentState()!=NULL){
		cout<<"===========Trace back to init mode, refine, Push two rep point to Stack!==============="<<endl;
		deltaArray = P->getParentDeltaArray();
		tempPoint = P->getParentState();
		initMode = P->getParentMode();
	}

	else{
		cout<<"======refine happen in init mode, using it's own state, Push two rep point to Stack!===="<<endl;
		deltaArray = P->getDeltaArray();
		tempPoint = P->getState();
		initMode = P->getMode();
	}

	for(int i=0; i<dim; i++){
		max[i] = tempPoint->getCoordinate(i+1)+deltaArray[i];
		min[i] = tempPoint->getCoordinate(i+1)-deltaArray[i];
	}

	deltaArray[dimindex] = deltaArray[dimindex]/2;

	class Point* P1 = new Point(dim+1);
	class RepPoint *RP1 = new RepPoint();
	P1->setCoordinate(0,0);
	for(int i=1;i<=dim;i++)
		P1->setCoordinate(i,min[i-1]+deltaArray[i-1]);
	RP1->setState(P1);
	delete P1;
	RP1->setDimension(dim);
	RP1->setMode(initMode);

    // FIXME temporary fix
    std::vector<double> temp_deltaArray(deltaArray, deltaArray+dim);

	RP1->setDeltaArray(temp_deltaArray);
	RP1->setRefineTime(reft+1);
	elems.push(RP1);

	class Point* P2 = new Point(dim+1);
	class RepPoint *RP2 = new RepPoint();
	P2->setCoordinate(0,0);
	for(int i=1; i<=dim; i++)
		P2->setCoordinate(i,max[i-1]-deltaArray[i-1]);
	RP2->setState(P2);
	delete P2;
	RP2->setDimension(dim);
	RP2->setMode(initMode);
	RP2->setDeltaArray(temp_deltaArray);
	RP2->setRefineTime(reft+1);
	elems.push(RP2);

/*
	cout<<"|     |"<<endl;
	cout<<"|     |"<<endl;
	for(size_t i=0; i<elems.size();i++)
		cout<<"|=====|"<<endl;
	cout<<"-------"<<endl;*/

	return;
}
