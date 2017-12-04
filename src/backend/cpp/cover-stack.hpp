/*
 * CoverStack.h
 *
 *  Created on: Feb 15, 2016
 *      Author: Bolun
 */

#ifndef COVERSTACK_H_
#define COVERSTACK_H_

#include <stack>          // std::stack

#include "rep-point.hpp"

class CoverStack {
private:
	std::stack<class RepPoint*>  elems;

public:
	CoverStack();
	~CoverStack();

	void push(class RepPoint* P);
	void pop();
	class RepPoint* top();
	int size();
	bool empty();
	void refine(class RepPoint *P, int dimindex);
	
};

#endif 
