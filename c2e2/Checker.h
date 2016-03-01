/*
 * Checker.h
 *
 *  Created on: Feb 2, 2013
 *      Author: parasara
 */

#ifndef CHECKER_H_
#define CHECKER_H_

#include <vector>
#include <iostream>
#include "InitialSet.h"
#include "Point.h"
#include "Annotation.h"
#include "ReachTube.h"
#include "LinearSet.h"
using namespace std;

class Checker {
private:
	vector<char*> forbidden;
public:
	Checker();
	virtual ~Checker();
	void setForbidden(vector<char*> strings);
	vector<char*> getForbidden();
	int check(class InitialSet* sequence, class Annotation* A, double delta);
	int check(class InitialSet* sequence, class Annotation* A, double delta, class Point* Pt3, class Point* Pt4);
	int check(class InitialSet* sequence, class Annotation* A, double delta, class Point* Pt3, class Point* Pt4, int modeSimu);
	int check(class ReachTube* invariantTube, class LinearSet* unsafeSet);
	int checkHybridSimulation(class ReachTube* invariantTube, class LinearSet* unsafeSet);
	void printChecker(class Point* Pt1, class Point* Pt2, double delta, double t1, double t2);
	int checkGLPKSol(class Point* Pt1, class Point* Pt2, double delta, double t1, double t2);
	int checkGLPKSol(class Point* Pt1, class Point* Pt2, double delta, double t1, double t2, class Point* Pt3, class Point* Pt4);
	int checkGLPKSol(class Point* Pt1, class Point* Pt2, class LinearSet* unsafeSet);
	int checkUnsafe(class Point* Pt1, class Point* Pt2, class Point* Pt3, class Point* Pt4, double valueDouble);
};

#endif /* CHECKER_H_ */
