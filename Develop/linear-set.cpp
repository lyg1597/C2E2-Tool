/*
 * LinearSet.cpp
 *
 *  Created on: Jul 28, 2014
 *      Author: parasara
 */

#include <iostream>
#include <cstdlib>
#include <glpk.h>

#include "linear-set.hpp"

#define DEBUG

using namespace std;

LinearSet::LinearSet() {
	// TODO Auto-generated constructor stub
}

LinearSet::~LinearSet() {
	// TODO Auto-generated destructor stub
}

void LinearSet::setDimensions(int dim){
	dimensions = dim;
}

int LinearSet::getDimensions(){
	return dimensions;
}

void LinearSet::setNumEqns(int numeqns){
	numberOfEquations = numeqns;
}

int LinearSet::getNumEqns(){
	return numberOfEquations;
}

void LinearSet::setMatrix(std::vector<double>& mat)
{
    matrix = mat;
}

std::vector<double>& LinearSet::getMatrix()
{
    return matrix;
}

void LinearSet::setB(std::vector<double>& b_in)
{
    b = b_in;
}

std::vector<double>& LinearSet::getB()
{
    return b;
}

void LinearSet::setMatrixElement(int row, int col, double value){
    matrix[row * dimensions + col] = value;
}

double LinearSet::getMatrixElement(int row, int col){
	return matrix[row*dimensions + col];
}

void LinearSet::setBElement(int index, double value){
    b[index] = value;
}

double LinearSet::getBElement(int index){
	return b[index];
}

int LinearSet::isInternal(class Point* RefPoint){

	int dim, numEq;
	dim = dimensions;

	numEq = numberOfEquations;

	glp_prob* feas;

	feas = glp_create_prob();

	int* irow, *icol;
	double* icoeffs;

	irow = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icol = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icoeffs = (double*)malloc((1+(numEq+1)*(dim+1))*sizeof(double));

	for(int h=0;h<(1+(numEq+1)*(dim+1));h++){
			irow[h] =0;icol[h]=0;icoeffs[h]=0;
	}

	int status;

	glp_set_prob_name(feas,"Feasibility checker");
	glp_set_obj_dir(feas, GLP_MAX);
	glp_add_rows(feas, numEq+1);
	glp_add_cols(feas, dim+1);

	int i,j,k;

	for(i=1;i<=dim;i++){

		if(RefPoint->getDimension() == dimensions){
			//cout << " Same dimensions -- " << RefPoint->getCoordinate(i-1) << " and " << RefPoint->getCoordinate(i-1) << endl;
			glp_set_col_bnds(feas,i,GLP_FX,RefPoint->getCoordinate(i-1),RefPoint->getCoordinate(i-1));
		}
		if(RefPoint->getDimension() == dimensions+1){
			//cout << " Not same dimensions -- " << RefPoint->getCoordinate(i) << " and " << RefPoint->getCoordinate(i) << endl;
			glp_set_col_bnds(feas,i,GLP_FX,RefPoint->getCoordinate(i),RefPoint->getCoordinate(i));
		}

		// glp_set_col_bnds(feas,i,GLP_DB,RefPoint->getCoordinate(i-1),RefPoint->getCoordinate(i-1));
	}
	for(i=1;i<=numEq;i++){
		glp_set_row_bnds(feas,i,GLP_UP,-10000,getBElement(i-1));
	}

	int count;
	count = 0;

	for(j=1;j<=numEq;j++){
		for(i=1;i<=dim;i++){
			irow[count+1] = j; icol[count+1] = i;
			icoeffs[count+1] = getMatrixElement(j-1,i-1);
			count++;
		}
	}

	glp_load_matrix(feas,count,irow,icol,icoeffs);
	glp_set_obj_coef(feas,1,1.0);
	glp_set_obj_dir(feas, GLP_MAX);
	glp_term_out(GLP_OFF);
	glp_simplex(feas, NULL);
	status = glp_get_status(feas);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 0;
	}
	else{
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 1;
	}

}

int LinearSet::hasIntersection(class Point* RefPoint, double* deltaArray){

	int dim, numEq;
	dim = dimensions;
	numEq = numberOfEquations;
	double delta;
	glp_prob* feas;

	cout<<numberOfEquations<<endl;
	cout<<dimensions<<endl;
	feas = glp_create_prob();

	int* irow, *icol;
	double* icoeffs;

	irow = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icol = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icoeffs = (double*)malloc((1+(numEq+1)*(dim+1))*sizeof(double));

	for(int h=0;h<(1+(numEq+1)*(dim+1));h++){
			irow[h] =0;icol[h]=0;icoeffs[h]=0;
	}

	int status;

	glp_set_prob_name(feas,"Feasibility checker");
	glp_set_obj_dir(feas, GLP_MAX);
	glp_add_rows(feas, numEq+1);
	glp_add_cols(feas, dim+1);

	int i,j,k;

	for(i=1;i<=dim;i++){	
		delta = deltaArray[i-1];
		if(RefPoint->getDimension() == dimensions){
			//cout << " Same dimensions -- " << PtL->getCoordinate(i-1) << " and " << PtU->getCoordinate(i-1) << endl;
			
			glp_set_col_bnds(feas,i,GLP_DB,RefPoint->getCoordinate(i-1)-delta,RefPoint->getCoordinate(i-1)+delta);
		}
		if(RefPoint->getDimension() == dimensions+1){
//			cout << " Not same dimensions -- " << PtL->getCoordinate(i) << " and " << PtU->getCoordinate(i) << endl;

			glp_set_col_bnds(feas,i,GLP_DB,RefPoint->getCoordinate(i)-delta,RefPoint->getCoordinate(i)+delta);
		}


	
	}
	for(i=1;i<=numEq;i++){
		glp_set_row_bnds(feas,i,GLP_UP,-10000,getBElement(i-1));
	}

	int count;
	count = 0;

	for(j=1;j<=numEq;j++){
		for(i=1;i<=dim;i++){
			irow[count+1] = j; icol[count+1] = i;
			icoeffs[count+1] = getMatrixElement(j-1,i-1);
			count++;
		}
	}

	glp_load_matrix(feas,count,irow,icol,icoeffs);
	glp_set_obj_coef(feas,1,1.0);
	glp_set_obj_dir(feas, GLP_MAX);
	glp_term_out(GLP_OFF);
	glp_simplex(feas, NULL);
	status = glp_get_status(feas);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 0;
	}
	else{

		double value = glp_get_obj_val(feas);
		// cout << "The value of objective is " << value << endl;
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 1;
	}

}


int LinearSet::hasIntersection(class Point* PtL, class Point* PtU){

	int dim, numEq;
	dim = dimensions;
	numEq = numberOfEquations;

	glp_prob* feas;

	feas = glp_create_prob();

	int* irow, *icol;
	double* icoeffs;

	irow = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icol = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icoeffs = (double*)malloc((1+(numEq+1)*(dim+1))*sizeof(double));

	for(int h=0;h<(1+(numEq+1)*(dim+1));h++){
			irow[h] =0;icol[h]=0;icoeffs[h]=0;
	}

	int status;

	glp_set_prob_name(feas,"Feasibility checker");
	glp_set_obj_dir(feas, GLP_MAX);
	glp_add_rows(feas, numEq+1);
	glp_add_cols(feas, dim+1);

	int i,j,k;

	for(i=1;i<=dim;i++){
		if(PtL->getDimension() == dimensions){
			//cout << " Same dimensions -- " << PtL->getCoordinate(i-1) << " and " << PtU->getCoordinate(i-1) << endl;
			glp_set_col_bnds(feas,i,GLP_DB,PtL->getCoordinate(i-1),PtU->getCoordinate(i-1));
		}
		if(PtL->getDimension() == dimensions+1){
//			cout << " Not same dimensions -- " << PtL->getCoordinate(i) << " and " << PtU->getCoordinate(i) << endl;
			glp_set_col_bnds(feas,i,GLP_DB,PtL->getCoordinate(i),PtU->getCoordinate(i));
		}
	}
	for(i=1;i<=numEq;i++){
		glp_set_row_bnds(feas,i,GLP_UP,-10000,getBElement(i-1));
	}

	int count;
	count = 0;

	for(j=1;j<=numEq;j++){
		for(i=1;i<=dim;i++){
			irow[count+1] = j; icol[count+1] = i;
			icoeffs[count+1] = getMatrixElement(j-1,i-1);
			count++;
		}
	}

	glp_load_matrix(feas,count,irow,icol,icoeffs);
	glp_set_obj_coef(feas,1,1.0);
	glp_set_obj_dir(feas, GLP_MAX);
	glp_term_out(GLP_OFF);
	glp_simplex(feas, NULL);
	status = glp_get_status(feas);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 0;
	}
	else{

		double value = glp_get_obj_val(feas);
		// cout << "The value of objective is " << value << endl;
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 1;
	}

}


double LinearSet::getMax(int dimID){
	int dim, numEq;
	dim = dimensions;
	numEq = numberOfEquations;

	glp_prob* feas;

	feas = glp_create_prob();

	int* irow, *icol;
	double* icoeffs;

	irow = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icol = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icoeffs = (double*)malloc((1+(numEq+1)*(dim+1))*sizeof(double));

	for(int h=0;h<(1+(numEq+1)*(dim+1));h++){
			irow[h] =0;icol[h]=0;icoeffs[h]=0;
	}

	int status;

	glp_set_prob_name(feas,"Feasibility checker");
	glp_set_obj_dir(feas, GLP_MAX);
	glp_add_rows(feas, numEq+1);
	glp_add_cols(feas, dim+1);

	int i,j,k;

	for(i=1;i<=dim;i++){
		glp_set_col_bnds(feas,i,GLP_FR,-1000000,1000000);
	}
	for(i=1;i<=numEq;i++){
		glp_set_row_bnds(feas,i,GLP_UP,-10000,getBElement(i-1));
	}

	int count;
	count = 0;

	for(j=1;j<=numEq;j++){
		for(i=1;i<=dim;i++){
			irow[count+1] = j; icol[count+1] = i;
			icoeffs[count+1] = getMatrixElement(j-1,i-1);
			count++;
		}
	}

	glp_load_matrix(feas,count,irow,icol,icoeffs);
	glp_set_obj_coef(feas,dimID+1,1.0);
	glp_set_obj_dir(feas, GLP_MAX);
	glp_term_out(GLP_OFF);
	glp_simplex(feas, NULL);
	status = glp_get_status(feas);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		cout << "Says that its infeasible! "<< endl;
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 0;
	}
	else{
		double value = glp_get_obj_val(feas);
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return value;
	}
}

double LinearSet::getMin(int dimID){
	int dim, numEq;
	dim = dimensions;
	numEq = numberOfEquations;

	glp_prob* feas;

	feas = glp_create_prob();

	int* irow, *icol;
	double* icoeffs;

	irow = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icol = (int*)malloc((1+(numEq+1)*(dim+1))*sizeof(int));
	icoeffs = (double*)malloc((1+(numEq+1)*(dim+1))*sizeof(double));

	for(int h=0;h<(1+(numEq+1)*(dim+1));h++){
			irow[h] =0;icol[h]=0;icoeffs[h]=0;
	}

	int status;

	glp_set_prob_name(feas,"Feasibility checker");
	glp_set_obj_dir(feas, GLP_MIN);
	glp_add_rows(feas, numEq+1);
	glp_add_cols(feas, dim+1);

	int i,j,k;

	for(i=1;i<=dim;i++){
		glp_set_col_bnds(feas,i,GLP_FR,-1000000,1000000);
	}
	for(i=1;i<=numEq;i++){
		glp_set_row_bnds(feas,i,GLP_UP,-10000,getBElement(i-1));
	}

	int count;
	count = 0;

	for(j=1;j<=numEq;j++){
		for(i=1;i<=dim;i++){
			irow[count+1] = j; icol[count+1] = i;
			icoeffs[count+1] = getMatrixElement(j-1,i-1);
			count++;
		}
	}

	glp_load_matrix(feas,count,irow,icol,icoeffs);
	glp_set_obj_coef(feas,dimID+1,1.0);
	glp_set_obj_dir(feas, GLP_MIN);
	glp_term_out(GLP_OFF);
	glp_simplex(feas, NULL);
	status = glp_get_status(feas);
	if(status == GLP_INFEAS || status == GLP_NOFEAS || status == GLP_UNDEF){
		cout << "Says that its infeasible! "<< endl;
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return 0;
	}
	else{
		double value = glp_get_obj_val(feas);
		glp_delete_prob(feas);
		free(irow);free(icol);free(icoeffs);
		return value;
	}
}

class InitialSet* LinearSet::getCover(double* deltaArray){
	
	// Logic is the following - partition the region with delta
	// and then check is intersection for every delta;

	
	class InitialSet* prevDimIter;
	class InitialSet* currDimIter;
	class InitialSet* tempDimIter;
	class Point* partitionPoint;
	class Point* referencePoint;

	int dimIterator;

	double dimMaxVal, dimMinVal;
	double midVal, refVal;
	double subDimIterator;
	double delta;
	
	for(dimIterator = 0; dimIterator < dimensions; dimIterator++){
		delta = deltaArray[dimIterator];
		if(dimIterator == 0){
			currDimIter = new InitialSet();
			partitionPoint = new Point(dimensions);
			dimMaxVal = getMax(dimIterator);
			dimMinVal = getMin(dimIterator);


			refVal = dimMinVal+delta;
			partitionPoint->setCoordinate(dimIterator,refVal);
			currDimIter->add(partitionPoint);
			refVal = refVal + 2*delta;
			while(refVal<dimMaxVal){
				partitionPoint = new Point (dimensions);
				partitionPoint->setCoordinate(dimIterator,refVal);
				currDimIter->add(partitionPoint);
				refVal = refVal + 2*delta;
			}

		}
		else{
			currDimIter = new InitialSet();
			tempDimIter = prevDimIter;
			dimMaxVal = getMax(dimIterator);
			dimMinVal = getMin(dimIterator);
			midVal = 0.5*(dimMaxVal + dimMinVal);
			#ifdef DEBUG

			double difference = dimMaxVal-dimMinVal;
			//cout<< "the difference in " << dimIterator << " is "<< difference<<endl;

			#endif

			while(tempDimIter != NULL){
				referencePoint = tempDimIter->getState();

				partitionPoint = new Point(dimensions);

				for(subDimIterator = 0; subDimIterator < dimIterator; subDimIterator++){
					partitionPoint->setCoordinate(subDimIterator,referencePoint->getCoordinate(subDimIterator));
				}

				refVal = dimMinVal+delta;
				partitionPoint->setCoordinate(dimIterator,refVal);
				currDimIter->add(partitionPoint);


				refVal = refVal + 2*delta;
				while(refVal < dimMaxVal){
					
					partitionPoint = new Point(dimensions);
					for(subDimIterator = 0; subDimIterator < dimIterator; subDimIterator++){
						partitionPoint->setCoordinate(subDimIterator,referencePoint->getCoordinate(subDimIterator));
					}
					partitionPoint->setCoordinate(dimIterator, refVal);
					currDimIter->add(partitionPoint);
					refVal = refVal + 2*delta;
				}



				tempDimIter = tempDimIter->getNext();
			}

		}

		prevDimIter = currDimIter;
	}


	class InitialSet* finalInitSet = new InitialSet();
	class Point* iteratorPoint;
	class Point* acceptorPoint;
	int finalDimIterator;


	tempDimIter = currDimIter;
	
	while(tempDimIter != NULL){
		iteratorPoint = tempDimIter->getState();
		//if( hasIntersection(iteratorPoint,deltaArray) ==1){
			acceptorPoint = new Point(dimensions+1);
			acceptorPoint->setCoordinate(0,0);
			for(finalDimIterator = 1;finalDimIterator <= dimensions; finalDimIterator++){
				acceptorPoint->setCoordinate(finalDimIterator,iteratorPoint->getCoordinate(finalDimIterator-1));
			}
			finalInitSet->add(acceptorPoint);
		//}
		tempDimIter = tempDimIter->getNext();
	}

	return finalInitSet;


}

CoverStack* LinearSet::getCoverStack(std::vector<double>& delta_arr, int mode,
        int refine_time)
{
    // Note: only works for non-convex set
    CoverStack* cover = new CoverStack();
    RepPoint* rep_pnt = new RepPoint();
    Point pnt(dimensions + 1);
    pnt.setCoordinate(0, 0);

    // Initialize pnt
    for (int i = 1; i <= dimensions; i++)
    {
        pnt.setCoordinate(i, getMin(i - 1) + delta_arr[i - 1]);
    }
    rep_pnt->setState(&pnt);

    // Initialize rep_pnt
    rep_pnt->setDimension(dimensions);
    rep_pnt->setMode(mode);
    rep_pnt->setDeltaArray(delta_arr);

    cover->push(rep_pnt);
    return cover;
}
/*
class CoverStack* LinearSet::getCoverStack(double *deltaArray, int mode, int refineTime){
	//only work for non-convex set
	class CoverStack* initCover = new CoverStack();
	class RepPoint* initRepPoint = new RepPoint();
	class Point* initPoint = new Point(dimensions+1);
	initPoint->setCoordinate(0,0);
	int i;
	for(i=1;i<=dimensions;i++)
		initPoint->setCoordinate(i,getMin(i-1)+deltaArray[i-1]);
	initRepPoint->setState(initPoint);
	delete initPoint;
	initRepPoint->setDimension(dimensions);
	initRepPoint->setMode(mode);
	initRepPoint->setDeltaArray(deltaArray);
	initRepPoint->setRefineTime(refineTime);
	initCover->push(initRepPoint);
	//initRepPoint->print();
	return initCover;
}
*/
