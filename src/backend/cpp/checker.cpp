/*
 * Checker.cpp
 *
 *  Created on: Feb 2, 2013
 *      Author: parasara
 */

#include <glpk.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include "checker.hpp"
#include "point.hpp"
#include "initial-set.hpp"

using namespace std;

int newSanityCheck();

Checker::Checker() {
}

Checker::~Checker() {
	// TODO Auto-generated destructor stub
}

void Checker::setForbidden(vector<char*> strings){
	forbidden = strings;
}

vector<char*> Checker::getForbidden(){
	return forbidden;
}

int Checker::check(class InitialSet* sequence, class Annotation* A, double delta){

/*
	double Boluntemp[4];


	class Point* Pt1;
	class Point* Pt2;

	class Point* Refpt1;
	class Point* Refpt2;

	double tmin, tmax;
	int numPoints;

	numPoints = sequence->getLength();

	int i;
	int refDim;
	double valueDouble;

	refDim = sequence->getState()->getDimension();

	Pt1 = new Point(refDim-1);
	Pt2 = new Point(refDim-1);

	class InitialSet* refSequence;

	refSequence = sequence;

	while(refSequence->getNext() != NULL){
		Refpt1 = refSequence->getState();
		refSequence = refSequence->getNext();
		Refpt2 = refSequence->getState();

		refDim = Refpt1->getDimension();
		for(i=1;i<refDim;i++){
			Pt1->setCoordinate(i-1,Refpt1->getCoordinate(i));
			Pt2->setCoordinate(i-1,Refpt2->getCoordinate(i));
		}

		tmin = Refpt1->getCoordinate(0);
		tmax = Refpt2->getCoordinate(0);

		valueDouble = A->getED(delta, tmin, tmax);

		//refSequence->setDelta(valueDouble);
		refSequence->setDelta(Boluntemp);

		int sanityReturn;
		sanityReturn = checkGLPKSol(Pt1,Pt2,valueDouble, tmin, tmax);

		if(sanityReturn == 0){
			return 0;
		}
	}
*/
	return 1;
}

int Checker::check(class InitialSet* sequence, class Annotation* A, double delta, class Point* Pt3, class Point* Pt4){
/*

	double Boluntemp[4];

	class Point* Pt1;
	class Point* Pt2;

	class Point* Refpt1;
	class Point* Refpt2;

	double tmin, tmax;
	int numPoints;

	numPoints = sequence->getLength();

	// cout << " Number of points here is " << numPoints << endl;

	if(numPoints == 1 || numPoints == 0){
		return 1;
	}

	int i;
	int refDim;
	double valueDouble;

	refDim = sequence->getState()->getDimension();

	Pt1 = new Point(refDim-1);
	Pt2 = new Point(refDim-1);

	class InitialSet* refSequence;

	refSequence = sequence;

	while(refSequence->getNext() != NULL){
		Refpt1 = refSequence->getState();
		refSequence = refSequence->getNext();
		Refpt2 = refSequence->getState();

		refDim = Refpt1->getDimension();
		for(i=1;i<refDim;i++){
			Pt1->setCoordinate(i-1,Refpt1->getCoordinate(i));
			Pt2->setCoordinate(i-1,Refpt2->getCoordinate(i));
		}

		tmin = Refpt1->getCoordinate(0);
		tmax = Refpt2->getCoordinate(0);

		valueDouble = A->getED(delta, tmin, tmax);

		//refSequence->setDelta(valueDouble);
		refSequence->setDelta(Boluntemp);
		int sanityReturn;
		int newUnsafe = 0;

		sanityReturn = checkGLPKSol(Pt1,Pt2,valueDouble, tmin, tmax, Pt3, Pt4);
		if(sanityReturn == 0){
			cout << "  value double is " << valueDouble << endl;
			newUnsafe = checkUnsafe(Pt1,Pt2,Pt3,Pt4,valueDouble);
		}

		if(sanityReturn == 0){
			return newUnsafe;
		}
	}
*/
	return 1;

}

int Checker::check(class InitialSet* sequence, class Annotation* A, double delta, class Point* Pt3, class Point* Pt4, int modeSimu){
/*
	class Point* Pt1;
	class Point* Pt2;

	class Point* Refpt1;
	class Point* Refpt2;

	double tmin, tmax;
	int numPoints;

	numPoints = sequence->getLength();

	// cout << " Number of points here is " << numPoints << endl;

	if(numPoints == 1 || numPoints == 0){
		return 1;
	}

	int i;
	int refDim;
	double valueDouble;

	refDim = sequence->getState()->getDimension();

	Pt1 = new Point(refDim-1);
	Pt2 = new Point(refDim-1);

	double refTime;

	refTime = sequence->getState()->getCoordinate(0);

	class InitialSet* refSequence;

	refSequence = sequence;

	while(refSequence->getNext() != NULL){
		Refpt1 = refSequence->getState();
		refSequence = refSequence->getNext();
		Refpt2 = refSequence->getState();

		refDim = Refpt1->getDimension();
		for(i=1;i<refDim;i++){
			Pt1->setCoordinate(i-1,Refpt1->getCoordinate(i));
			Pt2->setCoordinate(i-1,Refpt2->getCoordinate(i));
		}

		tmin = Refpt1->getCoordinate(0)-refTime;
		tmax = Refpt2->getCoordinate(0)-refTime;

		valueDouble = A->getED(delta, tmin, tmax, modeSimu);

		refSequence->setDelta(valueDouble);

		int sanityReturn;
		int newUnsafe = 0;

		sanityReturn = checkGLPKSol(Pt1,Pt2,valueDouble, tmin, tmax, Pt3, Pt4);
		if(sanityReturn == 0){
			cout << "  value double is " << valueDouble << endl;
			newUnsafe = checkUnsafe(Pt1,Pt2,Pt3,Pt4,valueDouble);
		}

		if(sanityReturn == 0){
			return newUnsafe;
		}
	}
*/
	return 1;

}

int Checker::check(class ReachTube* invariantTube, class LinearSet* unsafeSet){

	int currentMode;
	class Point *PtU, *PtL;
	int sizeReachSet;
	int iterator;

	int isSafe = 1;

	int checkIntersection;

	sizeReachSet = invariantTube->getSize();
	for(iterator=0;iterator<sizeReachSet && isSafe != -1;iterator++){
		PtU = invariantTube->getUpperBound(iterator);
		PtL = invariantTube->getLowerBound(iterator);

		checkIntersection = unsafeSet->hasIntersection(PtL, PtU);
		// cout << "PtL: "; PtL->print();
		// cout << "PtU: "; PtU->print();
		// cout << "Has intersection: " << checkIntersection << endl;
		if(checkIntersection == 1){
			// Check for 2^n possibilities!
			bool UBP, LBP;
			UBP = unsafeSet->isInternal(PtU);
			LBP = unsafeSet->isInternal(PtL);

			if(UBP && LBP){
				isSafe = -1;
			}
			else{
				isSafe = 0;
			}
		}
	}
	return isSafe;
}

int Checker::checkHybridSimulation(class ReachTube* simulation, class LinearSet* unsafeSet){
	Point *ptUpper;

	int size = simulation->getSize();
	for(int i=0; i<size; i++){
		ptUpper = simulation->getUpperBound(i);
		if(unsafeSet->isInternal(ptUpper)){
			return -1;
		}
	}
	return 1;
}

int Checker::checkUnsafe(class Point* Pt1, class Point* Pt2, class Point* Pt3, class Point* Pt4, double valueDouble){

	int dimensions, i;
	dimensions = Pt1->getDimension();
	for(i=0;i<dimensions;i++){
		if(Pt1->getCoordinate(i) + valueDouble <= Pt4->getCoordinate(i) && Pt1->getCoordinate(i) - valueDouble >= Pt3->getCoordinate(i) && Pt2->getCoordinate(i) + valueDouble <= Pt4->getCoordinate(i) && Pt2->getCoordinate(i) + valueDouble >= Pt3->getCoordinate(i)){
			// cout << " " << Pt1->getCoordinate(i) << " " << Pt2->getCoordinate(i) << " " << Pt3->getCoordinate(i) << " " << Pt4->getCoordinate(i) << endl;
			return -1;
		}
	}

	return 0;

}


int Checker::checkGLPKSol(class Point* Pt1, class Point* Pt2, double epsilon, double t1, double t2, class Point* Pt3, class Point* Pt4){

	int dimensions, dimIndex;
	dimensions = Pt1->getDimension();

	glp_prob* lp;
	lp = glp_create_prob();
	int* irow, *icol;
	double* icoeff;
	irow = (int*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(int));
	icol = (int*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(int));
	icoeff = (double*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(double));

	for(int h=0;h<(1+(dimensions+2)*(dimensions+2));h++){
			irow[h] =0;icol[h]=0;icoeff[h]=0;
	}

	int status;
	glp_set_prob_name(lp,"Does it matter");
	glp_set_obj_dir(lp, GLP_MAX);
	glp_add_rows(lp,dimensions+1);
	for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
		glp_set_row_bnds(lp, dimIndex, GLP_DB, -1*epsilon, epsilon);
	}
	glp_set_row_bnds(lp, dimensions+1, GLP_FX, 1.0, 1.0);

	glp_add_cols(lp,dimensions+2);
	for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
		glp_set_col_bnds(lp,dimIndex,GLP_DB,Pt3->getCoordinate(dimIndex-1),Pt4->getCoordinate(dimIndex-1));
	}
	glp_set_col_bnds(lp,dimensions+1,GLP_DB, 0.0, 1.0);
	glp_set_col_bnds(lp,dimensions+2,GLP_DB, 0.0, 1.0);
	glp_set_obj_coef(lp,dimensions+1,1.0);

	int i,j;
	int count=0;
	for(i=1;i<=dimensions;i++){
		for(j=1;j<=dimensions;j++){
			irow[count+1] = i; icol[count+1] = j;
			if(i==j){
				icoeff[1+count] = 1.0;
			}
			count++;
		}
		irow[count+1] = i; icol[count+1] = dimensions+1;
		icoeff[count+1] = -1*Pt1->getCoordinate(i-1);
		count++;
		irow[count+1] = i; icol[count+1] = dimensions+2;
		icoeff[count+1] = -1*Pt2->getCoordinate(i-1);
		count++;
	}
	for(i=1;i<=dimensions;i++){
		irow[count+1] = dimensions+1; icol[count+1] = i;
		icoeff[count+1] = 0.0;
		count++;
	}
	irow[count+1] = dimensions+1; icol[count+1] = dimensions+1;
	icoeff[count+1] = 1.0;
	count++;
	irow[count+1] = dimensions+1; icol[count+1] = dimensions+2;
	icoeff[count+1] = 1.0;
	count++;
	glp_load_matrix(lp,count,irow,icol,icoeff);
	glp_term_out(GLP_OFF);
	glp_simplex(lp, NULL);
	status = glp_get_status(lp);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		glp_delete_prob(lp);
		free(irow);free(icol);free(icoeff);
		return 1;
	}
	else{
		glp_delete_prob(lp);
		free(irow);free(icol);free(icoeff);
		return 0;
	}

}



int Checker::checkGLPKSol(class Point* Pt1, class Point* Pt2, double epsilon, double t1, double t2){

	int dimensions, dimIndex;
	double range1=6.0;
	double range2=5.0;
	double range3=100.0;
	double range4=-100.0;
	dimensions = Pt1->getDimension();


	glp_prob* lp;
	lp = glp_create_prob();
	int* irow, *icol;
	double* icoeff;
	irow = (int*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(int));
	icol = (int*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(int));
	icoeff = (double*)malloc((1+(dimensions+2)*(dimensions+2))*sizeof(double));

	for(int h=0;h<(1+(dimensions+2)*(dimensions+2));h++){
			irow[h] =0;icol[h]=0;icoeff[h]=0;
	}

	int status;
	glp_set_prob_name(lp,"Does it matter");
	glp_set_obj_dir(lp, GLP_MAX);
	glp_add_rows(lp,dimensions+1);
	for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
		glp_set_row_bnds(lp, dimIndex, GLP_DB, -1*epsilon, epsilon);
	}
	glp_set_row_bnds(lp, dimensions+1, GLP_FX, 1.0, 1.0);

	glp_add_cols(lp,dimensions+2);
	for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
		if(dimIndex==1){
			glp_set_col_bnds(lp, dimIndex, GLP_DB, range2, range1);
		}
		else if(dimIndex == 2){
			glp_set_col_bnds(lp, dimIndex, GLP_DB, range4, range3);
		}
		else{
			glp_set_col_bnds(lp,dimIndex,GLP_DB,-100.0,100.0);
		}
	}
	glp_set_col_bnds(lp,dimensions+1,GLP_DB, 0.0, 1.0);
	glp_set_col_bnds(lp,dimensions+2,GLP_DB, 0.0, 1.0);
	glp_set_obj_coef(lp,dimensions+1,1.0);

	int i,j;
	int count=0;
	for(i=1;i<=dimensions;i++){
		for(j=1;j<=dimensions;j++){
			irow[count+1] = i; icol[count+1] = j;
			if(i==j){
				icoeff[1+count] = 1.0;
			}
			count++;
		}
		irow[count+1] = i; icol[count+1] = dimensions+1;
		icoeff[count+1] = -1*Pt1->getCoordinate(i-1);
		count++;
		irow[count+1] = i; icol[count+1] = dimensions+2;
		icoeff[count+1] = -1*Pt2->getCoordinate(i-1);
		count++;
	}
	for(i=1;i<=dimensions;i++){
		irow[count+1] = dimensions+1; icol[count+1] = i;
		icoeff[count+1] = 0.0;
		count++;
	}
	irow[count+1] = dimensions+1; icol[count+1] = dimensions+1;
	icoeff[count+1] = 1.0;
	count++;
	irow[count+1] = dimensions+1; icol[count+1] = dimensions+2;
	icoeff[count+1] = 1.0;
	count++;
	glp_load_matrix(lp,count,irow,icol,icoeff);
	glp_term_out(GLP_OFF);
	glp_simplex(lp, NULL);
	status = glp_get_status(lp);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		glp_delete_prob(lp);
		free(irow);free(icol);free(icoeff);
		return 1;
	}
	else{
		glp_delete_prob(lp);
		free(irow);free(icol);free(icoeff);
		return 0;
	}

}

int Checker::checkGLPKSol(class Point* Pt1, class Point* Pt2, class LinearSet* unsafeSet){

	return 0;
}


void Checker::printChecker(class Point* Pt1, class Point* Pt2, double epsilon, double t1, double t2){

	ofstream outputFile;
	outputFile.open("glpkinput");
	int dimensions, dimIndex;

	dimensions = Pt1->getDimension();

	outputFile << "Minimize \n v: Lrs1 \n \n";

	outputFile << "Subject To \n  \n";

	outputFile << "sumr1: Lrs1 + Lrs2 = 1.0 \n";

	for(dimIndex = 0; dimIndex < dimensions; dimIndex++){
		outputFile << " dimxub"<< dimIndex+1 << ": ";
		outputFile << " x"<<dimIndex+1 << " ";
		if(Pt1->getCoordinate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordinate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordinate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordinate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordinate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordinate(dimIndex) << " Lrs2 ";
		}
		outputFile << " <= "<< fixed << epsilon <<" \n";

		outputFile << " dimxlb"<< dimIndex+1 << ": ";
		outputFile << " x"<<dimIndex+1 << " ";
		if(Pt1->getCoordinate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordinate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordinate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordinate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordinate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordinate(dimIndex) << " Lrs2 ";
		}
		outputFile << " >= "<< fixed << -1*epsilon <<" \n";
	}

	int k;
	for(k=0;k< forbidden.size();k++){
		outputFile << " unsafes"<< k+1 <<":";
		outputFile << forbidden.at(k) << "\n";
	}

	outputFile << "\nBounds  \n";
	outputFile << " 0 <= Lrs1 <= 1 \n 0 <= Lrs2 <= 1 \n";
	outputFile << " " << t1 << " <= t <= " << t2 << " \n";

	outputFile << " \nEnd \n";

	outputFile.close();

}

int newSanityCheck(){

	ifstream inputFile("Voutputglpk", ifstream::in);
	char Test[25];

	inputFile >> Test[0];
	while(!inputFile.eof()){
		inputFile >> Test[0];
		if(Test[0] == 'O'){
			for(int i=1;i<25;i++)
				inputFile >> Test[i];
			Test[20] = '\0';
			if(!strcmp(Test,"OPTIMALSOLUTIONFOUND")){
				inputFile.close();
				return 0;
			}
		}
	}
	inputFile.close();
	return 1;
}
