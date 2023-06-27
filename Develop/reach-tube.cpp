/*
 * ReachTube.cpp
 *
 *  Created on: Jul 24, 2014
 *      Author: parasara
 */

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <ppl.hh>

#include "reach-tube.hpp"
#include "point.hpp"

using namespace std;

ReachTube::ReachTube(){
	dimensions = -1;
	isReachTube = 0;
	reachTubeMode = -1;
}

ReachTube::~ReachTube(){

}

void ReachTube::setDimensions(int dim){
	dimensions = dim;
}

int ReachTube::getDimensions(){
	return dimensions;
}

void ReachTube::setMode(int mode){
	isReachTube = 1;
	reachTubeMode = mode;
}

int ReachTube::getMode(){
	return reachTubeMode;
}

void ReachTube::addUpperBoundState(class Point* upperBoundState){
	upperBound.push_back(upperBoundState);
}

void ReachTube::addLowerBoundState(class Point* lowerBoundState){
	lowerBound.push_back(lowerBoundState);
}

class Point* ReachTube::getUpperBoundState(size_t index){
	if(index < upperBound.size())
		return upperBound.at(index);
	else{
		std::cerr << "Error in size of upperBound " << endl;
	}
}

class Point* ReachTube::getLowerBoundState(size_t index){
	if(index < lowerBound.size())
		return lowerBound.at(index);
	else{
		std::cerr << "Error in size of lowerBound " << endl;
	}
}

int ReachTube::getSize(){
	if(upperBound.size() == lowerBound.size())
		return upperBound.size();
	else
		return 0;
}

class Point* ReachTube::getUpperBound(int index){
	return upperBound.at(index);
}

class Point* ReachTube::getLowerBound(int index){
	return lowerBound.at(index);
}

void ReachTube::parseInvariantTube(char const* filename, int hasMode){
	FILE* tRFile;
	tRFile = fopen(filename,"r");

	if(tRFile == NULL){
		cout << " Is this NULL ?" <<endl;
	}

	class Point* tracePoint;
	int breakloop = 0;
	double bufferReader;
	int flag = 0;
	int number=0;

	if(hasMode==1){
		int mode;
		fscanf(tRFile, "%d", &mode);
	}

	while( fscanf(tRFile,"%lf",&bufferReader) != EOF && breakloop == 0){
		number++;
		tracePoint = new Point(dimensions+1);
		tracePoint->setCoordinate(0,bufferReader);
		for(int j=0;j<dimensions;j++){
			if(fscanf(tRFile,"%lf",&bufferReader) != EOF){
				tracePoint->setCoordinate(j+1,bufferReader);
			}
			else{
				breakloop = 1;
			}
		}

		if(breakloop == 0 && flag == 0){
			lowerBound.push_back(tracePoint);
		}
		if(breakloop == 0 && flag == 1){
			upperBound.push_back(tracePoint);
		}
		flag = 1-flag;
	}
	fclose(tRFile);
}

class ReachTube* ReachTube::bloatReachTube(double bloatingFactor){

	int iterator,j;
	class ReachTube* bloatedTube;

	bloatedTube = new ReachTube();
	bloatedTube->setDimensions(dimensions);
	bloatedTube->setMode(reachTubeMode);

	class Point* tempPointL;
	class Point* tempPointU;

	double value1, value2;
	double tempvalue;

	for(iterator = 0; iterator < lowerBound.size(); iterator++){

		tempPointL = lowerBound.at(iterator);
		tempPointU = upperBound.at(iterator);

		class Point* addingPointL = new Point(tempPointL->getDimension());
		class Point* addingPointU = new Point(tempPointU->getDimension());

		addingPointL->setCoordinate(0,tempPointL->getCoordinate(0));
		addingPointU->setCoordinate(0,tempPointU->getCoordinate(0));

		for(j = 1; j < addingPointL->getDimension(); j++){

			value1 = tempPointL->getCoordinate(j);
			value2 = tempPointU->getCoordinate(j);

			if(value1 >= value2){
				tempvalue = value2;
				value2 = value1;
				value1 = tempvalue;
			}

			addingPointL->setCoordinate(j,value1-bloatingFactor);
			addingPointU->setCoordinate(j,value2+bloatingFactor);

		}

		bloatedTube->addLowerBoundState(addingPointL);
		bloatedTube->addUpperBoundState(addingPointU);

	}

	return bloatedTube;


}

class ReachTube* ReachTube::bloatReachTube(double* deltaArray, class Annotation* currentAnnotation){


	int iterator,j;
	class ReachTube* bloatedTube = new ReachTube();
	bloatedTube->setDimensions(dimensions);
	bloatedTube->setMode(reachTubeMode);

	class Point* tempPointL;
	class Point* tempPointU;

	double value1, value2;
	double tempvalue;

	double tmin, tmax;
	double epsilon;

	double delta;

	//cout << " In bloating \n";

	for(iterator = 0; iterator < lowerBound.size(); iterator++){

		tempPointL = lowerBound.at(iterator);
		tempPointU = upperBound.at(iterator);

		class Point* addingPointL = new Point(tempPointL->getDimension());
		class Point* addingPointU = new Point(tempPointU->getDimension());

		addingPointL->setCoordinate(0,tempPointL->getCoordinate(0));
		addingPointU->setCoordinate(0,tempPointU->getCoordinate(0));

//		cout << " First and last " << tmin << " and " << tmax <<endl;

		tmin = tempPointL->getCoordinate(0);
		tmax = tempPointU->getCoordinate(0);

		

		for(j = 1; j < addingPointL->getDimension(); j++){
			delta = deltaArray[j-1];

			epsilon = currentAnnotation->getED(delta,tmin,tmax,reachTubeMode);
			//cout<<"at dimensions "<<j<< " the epsilon is "<<epsilon<<endl;

			value1 = tempPointL->getCoordinate(j);
			value2 = tempPointU->getCoordinate(j);

			if(value1 >= value2){
				tempvalue = value2;
				value2 = value1;
				value1 = tempvalue;
			}


			addingPointL->setCoordinate(j,value1-epsilon);
			addingPointU->setCoordinate(j,value2+epsilon);

//			cout << "These values are " << value1-epsilon << "  and  " << value2+epsilon <<endl;

		}

		bloatedTube->addLowerBoundState(addingPointL);
		bloatedTube->addUpperBoundState(addingPointU);

	}

	return bloatedTube;

}

void ReachTube::parseGuardsTube(char* filename){

	isReachTube = 0;
	reachTubeMode = -1;

	FILE* tRFile;
	tRFile = fopen(filename,"r");

	//cout << "Opening the file \n";

	class Point* tracePoint;

	int breakloop = 0;

	double bufferReader;
	int tempMode;

	int flag = 0;

	while( fscanf(tRFile,"%lf",&bufferReader) != EOF && breakloop == 0){

		tracePoint = new Point(dimensions+1);
		tracePoint->setCoordinate(0,bufferReader);
//			cout << bufferReader << " \n";
		for(int j=0;j<dimensions;j++){
			if(fscanf(tRFile,"%lf",&bufferReader) != EOF){
				tracePoint->setCoordinate(j+1,bufferReader);
			}
			else{
//					cout << "break loop is one \n";
				breakloop = 1;
			}
		}

		lowerBound.push_back(tracePoint);

		tracePoint = new Point(dimensions+1);

//		tracePoint->setCoordinate(0,bufferReader);
//			cout << bufferReader << " \n";
		for(int j=0;j<=dimensions;j++){
			if(fscanf(tRFile,"%lf",&bufferReader) != EOF){
				tracePoint->setCoordinate(j,bufferReader);
			}
			else{
//					cout << "break loop is one \n";
				breakloop = 1;
			}
		}

		upperBound.push_back(tracePoint);

		if(fscanf(tRFile,"%d",&tempMode) != EOF){
			mode.push_back(tempMode);
			// cout << " mode is " << tempMode << endl;
		}

	}
	fclose(tRFile);


}

void ReachTube::addGuards(vector<pair<NNC_Polyhedron,int> > guards){
	isReachTube = 0;
	reachTubeMode = -1;

	int tempMode;
	NNC_Polyhedron poly;
	Point *ptU, *ptL;

	int guardthinprop;
	vector<int> thinvector;
	string thinprop;
	ifstream thinpropfile("../work-dir/ThinVarProp");
	thinvector.push_back(1);
	if(thinpropfile.is_open()){
		while(getline(thinpropfile,thinprop)){
			thinvector.push_back(thinprop == "1");
		}
		thinpropfile.close();
	}
	else cout<< "unable to open thin property file" <<endl;





	for(int i=0; i<guards.size(); i++){
		poly = guards[i].first;
		tempMode = guards[i].second;

		ptL = new Point(dimensions+1);
		ptU = new Point(dimensions+1);
		
		for(int j=0; j<dimensions+1; j++){
			ptL->setCoordinate(j, DBL_MAX);
			ptU->setCoordinate(j, -DBL_MAX);
		}

		Generator_System gs=poly.minimized_generators();
		Generator_System::const_iterator k;
		for(k=gs.begin();k!=gs.end();++k)
		{
			if(k->is_point())
			{
			 	double divisor=mpz_get_d(k->divisor().get_mpz_t());
			  	int dim=int(k->space_dimension());
			  	for(int j=0;j<dim;j++)
			  	{
			    	double dividend=mpz_get_d(k->coefficient(Variable(j)).get_mpz_t());
			    	double num = dividend/divisor;

			    	if(thinvector[j] == 1){
			    		if (num > ptU->getCoordinate(j)){
			    			ptL->setCoordinate(j,num);
			    			ptU->setCoordinate(j,num);
			    		}
			    	}
			    	else{
			    		if(num < ptL->getCoordinate(j))
			    			ptL->setCoordinate(j, num);
				    	if(num > ptU->getCoordinate(j))
				    		ptU->setCoordinate(j, num);

			    	}
			  	}
			}
		}
		mode.push_back(tempMode);
		upperBound.push_back(ptU);
		lowerBound.push_back(ptL);
	}
}

void ReachTube::addGuards(double *ptLower, vector<pair<int, double *> > guards){
	isReachTube = 0;
	reachTubeMode = -1;

	int tempMode;
	// bool exists;
	Point *ptUpper, *ptL;
	for(int i=0; i<guards.size(); i++){
		// exists = false;
		tempMode = guards[i].first;
		ptL = new Point(dimensions+1, ptLower);
		ptUpper = new Point(dimensions+1, guards[i].second);
	
			mode.push_back(tempMode);
			upperBound.push_back(ptUpper);
			lowerBound.push_back(ptL);
		// }
	}
	// cout << "Guard Length: " << mode.size() << endl;
}

void ReachTube::printGuards(){
	for(int i=0; i<mode.size(); i++){
		cout << "LB: "; lowerBound[i]->print();
		cout << "UB: "; upperBound[i]->print();
		cout << "Destination Mode: " << mode[i] << endl << endl;
		// cout << endl;
	}
}

//void ReachTube::printReachTube(char const* filename, int flag){
void ReachTube::printReachTube(const std::string file_name, int flag){
	ofstream invFile;
	if(flag == 0){
		invFile.open(file_name);
		invFile << " " << reachTubeMode << endl;
	}
	if(flag == 1){
		if(lowerBound.size()>=1){
			invFile.open(file_name, ios::app);
			invFile << " % mode " << reachTubeMode << endl;
		}
	}
	if(flag == 2){
		if(lowerBound.size()>=1){
			invFile.open(file_name, ios::app);
			invFile << " % mode " << reachTubeMode << " unsafe"<< endl;
		}
	}
//	invFile << " " << reachTubeMode << endl;

	int iterator,i;
	class Point* tempPoint;
	for(iterator = 0; iterator < lowerBound.size(); iterator++){
		tempPoint = lowerBound.at(iterator);
		for(i=0;i<tempPoint->getDimension();i++){
			invFile << " " << tempPoint->getCoordinate(i);
		}
		invFile << endl;

		tempPoint = upperBound.at(iterator);
		for(i=0;i<tempPoint->getDimension();i++){
			invFile << " " << tempPoint->getCoordinate(i);
		}
		invFile << endl;

	}

	invFile.close();
}

int ReachTube::getGuardMode(int index){
	if( mode.size() > index ){
		return mode.at(index);
	}
	return -1;
}

void ReachTube::compressGuardSet(){

	// Logic - get the last state, compute the next state,
	// set upper and lower points, from last, come back
	//

	int modeCollapsed;
	int pointIndex;
	int modeTemp;
	int dimensionsPoint;
	int dimIndex;
	vector<int> nextModes;
	vector<class Point*> tempUpper;
	vector<class Point*> tempLower;
	class Point* refLower;
	class Point* refUpper;
	class Point* tempPointLower;
	class Point* tempPointUpper;


	while(upperBound.size() > 0){
		modeCollapsed = mode.at(mode.size()-1);
		refLower = lowerBound.at(lowerBound.size()-1);
		refUpper = upperBound.at(upperBound.size()-1);

		mode.pop_back(); lowerBound.pop_back(); upperBound.pop_back();

		for(pointIndex=upperBound.size()-1; pointIndex>=0; pointIndex--){

			modeTemp = mode.at(pointIndex);

			if(modeTemp == modeCollapsed){

				tempPointLower = lowerBound.at(pointIndex); tempPointUpper = upperBound.at(pointIndex);
				// Code for collapsing the dimensions;
				for(dimIndex = 0; dimIndex < refLower->getDimension(); dimIndex++){
					// merging the coordinates, lower is the lowest of them, higher is the largest of them.
					if(tempPointLower->getCoordinate(dimIndex) <= refLower->getCoordinate(dimIndex)){
						refLower->setCoordinate(dimIndex,tempPointLower->getCoordinate(dimIndex));
					}
					if(tempPointUpper->getCoordinate(dimIndex) >= refUpper->getCoordinate(dimIndex)){
						refUpper->setCoordinate(dimIndex,tempPointUpper->getCoordinate(dimIndex));
					}
				}

				mode.pop_back(); lowerBound.pop_back(); upperBound.pop_back();
			}

		}

		nextModes.push_back(modeCollapsed);
		tempUpper.push_back(refUpper); tempLower.push_back(refLower);

	}

	for(pointIndex=0; pointIndex<nextModes.size(); pointIndex++){
		mode.push_back(nextModes.at(pointIndex));
		lowerBound.push_back(tempLower.at(pointIndex));
		upperBound.push_back(tempUpper.at(pointIndex));
	}

	nextModes.clear(); tempLower.clear(); tempUpper.clear();


}


class InitialSet* ReachTube::samplingGuardSet(){

	int rectIndex;
	vector<int> nextModes;
	vector<class Point*> tempUpper;
	vector<class Point*> tempLower;

	for(rectIndex=0;rectIndex < lowerBound.size();rectIndex++){



	}
}

double ReachTube::getMaxCoordinate(int dimension, int curMode){

	int pointIndex;
	double maxValue = -100000;
	class Point* tempPoint;
	for(pointIndex = upperBound.size()-1; pointIndex>=0; pointIndex--){
		if(mode.at(pointIndex) == curMode){
			tempPoint = upperBound.at(pointIndex);
			if(maxValue <= tempPoint->getCoordinate(dimension)){
				maxValue = tempPoint->getCoordinate(dimension);
			}
		}
	}
	return maxValue;
}

vector<double> ReachTube::MaxCoordinate(int dimension){
	int pointIndex;
	double maxValue = -100000;
	double pairValue = 0;
	class Point* maxPoint;
	class Point* pairPoint;
	for (pointIndex = upperBound.size()-1; pointIndex>=0; pointIndex--){
		maxPoint = upperBound.at(pointIndex);
		if (maxValue<=maxPoint->getCoordinate(dimension)){
			maxValue = maxPoint->getCoordinate(dimension);
			pairPoint = lowerBound.at(pointIndex);
			pairValue = pairPoint->getCoordinate(dimension);
		}
	}
	std::vector<double> retval;
	retval.push_back(maxValue);
	retval.push_back(pairValue);
	return retval;
}

double ReachTube::getMinCoordinate(int dimension, int curMode){

	int pointIndex;
	double minValue = 100000;
	class Point* tempPoint;
	for(pointIndex = lowerBound.size()-1; pointIndex>=0; pointIndex--){
		if(mode.at(pointIndex) == curMode){
			tempPoint = lowerBound.at(pointIndex);
			if(minValue >= tempPoint->getCoordinate(dimension)){
				minValue = tempPoint->getCoordinate(dimension);
			}
		}
	}
	return minValue;
}

vector<double> ReachTube::MinCoordinate(int dimension){
	int pointIndex;
	double minValue = 100000;
	double pairValue = 0;
	class Point* minPoint;
	class Point* pairPoint;
	for (pointIndex = lowerBound.size()-1; pointIndex>=0; pointIndex--){
		minPoint = lowerBound.at(pointIndex);
		if(minValue >= minPoint->getCoordinate(dimension)){
			minValue = minPoint->getCoordinate(dimension);
			pairPoint = upperBound.at(pointIndex);
			pairValue = pairPoint->getCoordinate(dimension);
		}
	}
	std::vector<double> retval;
	retval.push_back(minValue);
	retval.push_back(pairValue);
	return retval;

}
int ReachTube::checkunsafe_rest(double* forM, double* forB, int numofeq, int index){
	int i,equ;
	int unkonwnflag=0;
	int unsafeflag=0;
	int unknown=0;
	double uppersum;
	double lowersum;
	class Point* upperPoint;
	class Point* lowerPoint;
	for (equ=1;equ<numofeq;equ++){
		unsafeflag = 0;
		unkonwnflag = 0;
		uppersum=0.0;
		lowersum=0.0;
		lowerPoint = lowerBound.at(index);
		upperPoint = upperBound.at(index);
		for(i=0;i<dimensions;i++){
			uppersum+=forM[dimensions*equ+i]*upperPoint->getCoordinate(i+1);
			lowersum+=forM[dimensions*equ+i]*lowerPoint->getCoordinate(i+1);
		}
		if (uppersum<forB[equ]){
			if(lowersum<forB[equ]){
				unsafeflag=1;
			}
			unkonwnflag =1;
			unknown = 1;
		}

		if (!unsafeflag)
			if(!unkonwnflag)
				return 1;
	}
	if (unknown)
		return 0;
	return -1;

}
int ReachTube::checkunsafe(double* forM, double* forB, int numofeq){

	int i;
	int restresult=0;
	int pointIndex;
	int unkonwnflag=0;
	double uppersum;
	double lowersum;
	class Point* upperPoint;
	class Point* lowerPoint;
	for (pointIndex = lowerBound.size()-1; pointIndex>=0; pointIndex--){
		uppersum = 0.0;
		lowersum = 0.0;
		lowerPoint = lowerBound.at(pointIndex);
		upperPoint = upperBound.at(pointIndex);
		for(i=0;i<dimensions;i++){
			uppersum+=forM[i]*upperPoint->getCoordinate(i+1);
			lowersum+=forM[i]*lowerPoint->getCoordinate(i+1);
		}
		if (uppersum<forB[0]){
			if (numofeq>1){
				restresult = checkunsafe_rest(forM, forB, numofeq,pointIndex);
				//cout<<"rest result is "<<restresult<<endl;
			}

			if(lowersum<forB[0]){
				//cout<<"it's unsafe, check result constrain"<<endl;

				if (numofeq>1)
				{
					if (restresult == -1)
						return -1;
					if (restresult == 0)
						unkonwnflag=1;
				}
				else
					return -1;
			}		
			if (numofeq>1){
				if (restresult == 1)
					unkonwnflag = 0;
				if (restresult == 0 || restresult == -1)
					unkonwnflag = 1;
			}
			else
				unkonwnflag = 1;
		}

	}
	if (unkonwnflag)
		return 0;
	return 1;


}

class InitialSet* ReachTube::getNextSet(){

	std::vector<int> modesInSet;

	class InitialSet* ToBeReturned = NULL;
	class InitialSet* indexSet;
	class InitialSet* modeSet;

	class Point* currPoint;

	int modeIndex;
	int tempMode;

    std::vector<double> deltas(dimensions);
    std::vector<double> b(2 * dimensions);
    std::vector<double> matrix(2 * dimensions * dimensions, 0);

	for(modeIndex=mode.size()-1;modeIndex>=0;modeIndex--){

		tempMode = mode.at(modeIndex);
		int foundFlag = 0;
		for(int j=0;j < modesInSet.size(); j++){
			if(tempMode == modesInSet.at(j)){
				foundFlag = 1;
			}
		}
		if(foundFlag == 0){
			modesInSet.push_back(tempMode);
		}

	}

//	cout << "collected all the modes! \n";

	for(modeIndex = 0; modeIndex < modesInSet.size(); modeIndex++){
//		cout << "How many modes! \n";
		tempMode = modesInSet.at(modeIndex);

		class InitialSet* TempToBeAdded = new InitialSet();

		int indexDimension;

		for(indexDimension=1;indexDimension<=dimensions; indexDimension++){
			double minValue = getMinCoordinate(indexDimension,tempMode);
			double maxValue = getMaxCoordinate(indexDimension,tempMode);
			/*cout<<"the max value is "<< maxValue<<endl;
			cout<<"the min value is "<< minValue<<endl;*/
			deltas[indexDimension-1] = abs(maxValue-minValue)/2;

			/*
            cout<<"the delta is "<<deltaArray[indexDimension-1]<<endl;
			*(b+2*(indexDimension-1)) = -1*minValue;
			*(b+2*(indexDimension-1)+1) = maxValue;
			*(matrix+2*(indexDimension-1)*dimensions+(indexDimension-1)) = -1;
			*(matrix+(2*(indexDimension-1)+1)*dimensions+(indexDimension-1)) = 1;
            */

            b[2 * (indexDimension - 1)] = -1 * minValue;
            b[2 * (indexDimension - 1) + 1] = maxValue;

            matrix[2 * (indexDimension - 1) * dimensions + 
                (indexDimension - 1)] = -1;
            matrix[(2 * (indexDimension - 1) + 1) * dimensions +
                (indexDimension - 1)] = 1;

		}

		class LinearSet* modeLinear = new LinearSet();
		modeLinear->setDimensions(dimensions);
		modeLinear->setNumEqns(2*dimensions);
		modeLinear->setB(b);
		modeLinear->setMatrix(matrix);
		
		modeSet = modeLinear->getCover(&deltas[0]);
	

		/*cout << "Size of just the partitioning is " << modeSet->getLength() << "\n";*/

		// modeSet->print();

		indexSet = modeSet;

		while(indexSet != NULL){

			// Logic for computing whether the state has any intersection or not
			// Then logic for computing time step and adding the value of time stamps

			currPoint = indexSet->getState();
			int curMode = tempMode;

            // FIXME
			if(checkIntersection(curMode, currPoint, &deltas[0])){

//				cout << "Checking intersections \n";
                // FIXME
				double tmin = getMinTime(curMode, currPoint, &deltas[0]);
				//cout << "  The time minimum time is " << tmin << "\n";
				currPoint->setCoordinate(0,tmin);
				TempToBeAdded->add(currPoint);


			}

			indexSet = indexSet->getNext();

		}

//		cout << "Out of the loop \n";

		// Check if there is intersection, if yes, compute the next set of points
		// and time stamps to be computed from
		//
		// Next thing to do is to check the intersection with any of the modes.

		/*cout << " Size if to be added is " << TempToBeAdded->getLength() << "\n";*/
		TempToBeAdded->setMode(tempMode);
		TempToBeAdded->setDelta(&deltas[0]);
//		cout << " Added? \n";


		if(ToBeReturned == NULL){
			ToBeReturned = TempToBeAdded;
		}
		else{
			ToBeReturned->append(TempToBeAdded);
		}

//		cout << " Total size is " << ToBeReturned->getLength() << "\n";

	}

	/*if(ToBeReturned != NULL){
		ToBeReturned->setDelta(deltaArray);
	}*/

	return ToBeReturned;

}


int ReachTube::getNextSetStack(class CoverStack* ItrStack, class RepPoint* parentRepPoint){
	std::vector<int> modesInSet;
	class Point* parentPoint;
	double* parentDeltaArray;
	class CoverStack* tempStack;
	class RepPoint* tempRepPoint;
	class Point* tempPoint;
	int refinetime = parentRepPoint->getRefineTime();
	int initMode;

	if (parentRepPoint->getParentState()!=NULL){
		parentPoint = parentRepPoint->getParentState();
		parentDeltaArray = parentRepPoint->getParentDeltaArray();
		initMode = parentRepPoint->getParentMode();
	}
	else{
		parentPoint = parentRepPoint->getState();
		parentDeltaArray = parentRepPoint->getDeltaArray();
		initMode = parentRepPoint->getMode();
	}

	int modeIndex;
	int tempMode;

    std::vector<double> deltas(dimensions);
    std::vector<double> b(2 * dimensions);
    std::vector<double> matrix(2 * dimensions * dimensions, 0);

	for(modeIndex=mode.size()-1; modeIndex>=0; modeIndex--){
		tempMode = mode.at(modeIndex);
		int foundFlag = 0;
		for(int j=0; j<modesInSet.size(); j++){
			if(tempMode == modesInSet.at(j)){
				foundFlag = 1;
			}
		}
		if(foundFlag == 0){
			modesInSet.push_back(tempMode);
		}
	}

	//cout<<"Number of next set is "<<modesInSet.size()<<endl;
	if(modesInSet.size()==0)
		return 0;

	vector<int> thinvector;
	string thinprop;
	ifstream thinpropfile("../work-dir/ThinVarProp");
	thinvector.push_back(1);

	if(thinpropfile.is_open()){
		while(getline(thinpropfile,thinprop)){
			thinvector.push_back(thinprop == "1");
		}
		thinpropfile.close();
	}
	else cout<< "Uable to open thin property file" <<endl;
	cout << "dimensions = " << dimensions << endl;




	for(modeIndex = 0; modeIndex < modesInSet.size(); modeIndex++){
		tempMode = modesInSet.at(modeIndex);
		InitialSet* TempToBeAdded = new InitialSet();
		int indexDimension;

		for(indexDimension=1;indexDimension<=dimensions; indexDimension++){
			double minValue = getMinCoordinate(indexDimension,tempMode);
			double maxValue = getMaxCoordinate(indexDimension,tempMode);
			if (thinvector[indexDimension] == 1)
				minValue = getMaxCoordinate(indexDimension,tempMode);
			
			deltas[indexDimension-1] = abs(maxValue-minValue)/2;
	
            b[2 * (indexDimension - 1)] = -1 * minValue;
            b[2 * (indexDimension - 1) + 1] = maxValue;

            matrix[2 * (indexDimension - 1) * dimensions + 
                (indexDimension - 1)] = -1;
            matrix[(2 * (indexDimension - 1) + 1) * dimensions +
                (indexDimension - 1)] = 1;

            /*
			*(b+2*(indexDimension-1)) = -1*minValue;
			*(b+2*(indexDimension-1)+1) = maxValue;
			*(matrix+2*(indexDimension-1)*dimensions+(indexDimension-1)) = -1;
			*(matrix+(2*(indexDimension-1)+1)*dimensions+(indexDimension-1)) = 1;
            */
		}

		LinearSet* modeLinear = new LinearSet();
		modeLinear->setDimensions(dimensions);
		modeLinear->setNumEqns(2*dimensions);
		modeLinear->setB(b);
		modeLinear->setMatrix(matrix);
		
		//cout<<"Start to collect next rep Point"<<endl;
		tempStack = modeLinear->getCoverStack(deltas, tempMode, refinetime);

		//cout<<"Done collecting next guard"<<endl;
		//cout<<"Check if next set is empty"<< tempStack->empty()<<endl;
		while(!tempStack->empty()){
			tempRepPoint = tempStack->top();
			tempStack->pop();
			tempPoint = new Point(*tempRepPoint->getState());
			int curMode = tempMode;
            // FIXME 
			if(checkIntersection(curMode, tempPoint, &deltas[0])){
				double tmin = getMinTime(curMode, tempPoint, &deltas[0]);
				tempPoint->setCoordinate(0,tmin);
				//cout<<tempPoint->getDimension()<<endl;
				//cout<<"Start to verify next rep Point"<<endl;
				tempRepPoint->setState(tempPoint);
				//cout<<"Start to verify next rep Point"<<endl;
				delete tempPoint;
				tempRepPoint->setDimension(dimensions);
				tempRepPoint->setDeltaArray(deltas);
				tempRepPoint->setParentState(parentPoint);
				tempRepPoint->setParentDeltaArray(parentDeltaArray);
				tempRepPoint->setParentMode(initMode);
				tempRepPoint->setRefineTime(refinetime);
				ItrStack->push(tempRepPoint);

				cout<<"====Find next region Information, recalculating deltaArray, generate one RepPoint, Push to stack===="<<endl;
				// cout<<"|     |"<<endl;
				// cout<<"|     |"<<endl;
				// for(int i=0; i<ItrStack->size();i++)
				// 	cout<<"|=====|"<<endl;
				// cout<<"-------"<<endl;
			}
		}
	}
	//find Next Set, return 1
	return 1;
}








int ReachTube::checkIntersection(int curMode, class Point* currPoint, double* deltaArray){

	int index;
	int hasIntersection;
	int dimIndex;
	class Point* upperBoundPoint;
	class Point* lowerBoundPoint;
	double maxDim, minDim;
	double maxPointDim, minPointDim;
	double delta;
	for(index=0;index < mode.size(); index++){
		if(mode.at(index) == curMode){
			hasIntersection = 1;
			upperBoundPoint = upperBound.at(index);
			lowerBoundPoint = lowerBound.at(index);
			for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
				delta = deltaArray[dimIndex-1];
				maxDim = upperBoundPoint->getCoordinate(dimIndex);
				minDim = lowerBoundPoint->getCoordinate(dimIndex);
				maxPointDim = currPoint->getCoordinate(dimIndex)+delta;
				minPointDim = currPoint->getCoordinate(dimIndex)-delta;
				if(minDim > maxPointDim || maxDim < minPointDim){
					hasIntersection = 0;
				}
			}
			if(hasIntersection == 1){
				return 1;
			}
		}
	}
	return 0;

}

double ReachTube::getMinTime(int curMode, class Point* currPoint, double* deltaArray){

	int index;
	int hasIntersection;
	int dimIndex;
	class Point* upperBoundPoint;
	class Point* lowerBoundPoint;
	double maxDim, minDim;
	double maxPointDim, minPointDim;
	double minTime = 1000000;
	double delta;
	for(index=0;index < mode.size(); index++){
		if(mode.at(index) == curMode){
			hasIntersection = 1;
			upperBoundPoint = upperBound.at(index);
			lowerBoundPoint = lowerBound.at(index);
			for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
				delta = deltaArray[dimIndex-1];
				maxDim = upperBoundPoint->getCoordinate(dimIndex);
				minDim = lowerBoundPoint->getCoordinate(dimIndex);
				maxPointDim = currPoint->getCoordinate(dimIndex)+delta;
				minPointDim = currPoint->getCoordinate(dimIndex)-delta;
				if(minDim > maxPointDim || maxDim < minPointDim){
					hasIntersection = 0;
				}
			}
			if(hasIntersection == 1){
				if(minTime > lowerBoundPoint->getCoordinate(0)){
					minTime = lowerBoundPoint->getCoordinate(0);
				}
			}
		}
	}

	return minTime;

}

double ReachTube::getMaxTime(int curMode, class Point* currPoint, double* deltaArray){

	int index;
	int hasIntersection;
	int dimIndex;
	double delta;
	class Point* upperBoundPoint;
	class Point* lowerBoundPoint;
	double maxDim, minDim;
	double maxPointDim, minPointDim;
	double maxTime = -1000000;
	for(index=0;index < mode.size(); index++){
		if(mode.at(index) == curMode){
			hasIntersection = 1;
			upperBoundPoint = upperBound.at(index);
			lowerBoundPoint = lowerBound.at(index);
			for(dimIndex=1;dimIndex<=dimensions;dimIndex++){
				delta = deltaArray[dimIndex-1];
				maxDim = upperBoundPoint->getCoordinate(dimIndex);
				minDim = lowerBoundPoint->getCoordinate(dimIndex);
				maxPointDim = currPoint->getCoordinate(dimIndex)+delta;
				minPointDim = currPoint->getCoordinate(dimIndex)-delta;
				if(minDim > maxPointDim || maxDim < minPointDim){
					hasIntersection = 0;
				}
			}
			if(hasIntersection == 1){
				if(maxTime < upperBoundPoint->getCoordinate(0)){
					maxTime = upperBoundPoint->getCoordinate(0);
				}
			}
		}
	}

	return maxTime;

}

void ReachTube::clear(int from){
	upperBound.erase(upperBound.begin()+from, upperBound.end());
	lowerBound.erase(lowerBound.begin()+from, lowerBound.end());
}

