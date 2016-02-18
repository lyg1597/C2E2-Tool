/*
 * main.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include"Polyhedron.h"
#include"Vector.h"
#include"Point.h"
#include"Simulator.h"
#include"Annotation.h"
#include"Checker.h"
//#include"y.tab.c"
#include"InitialSet.h"

#include"Visualizer.h"
#include"ReachTube.h"
#include"LinearSet.h"

#include"RepPoint.h"
#include"CoverStack.h"

#include "simuverifScanner.h"
#include "simuverifParser.h"
#include "simuverif.tab.c"

#include<iostream>

#include<fstream>
#include<cstdlib>
#include<stdlib.h>
#include<ctime>
#include<cmath>
#include<math.h>

#include<time.h>
#include<string.h>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <Python.h>
#include <iostream>
#include <stack> 
using namespace std;


int main(int argc, char* argv[]) {




	Parser parser;
	parser.parse();

	int dimValue;
	double absoluteError;
	double relativeError;
	double deltaValue;
	double timeStepValue;
	double globalTimeValue;
	int annotationTypeValue;
	char* annotationString = NULL;
	char* betaString = NULL;
	double kConstValue;
	double gammaValue;
	int visu1, visu2;
	char visuFileName[50];

	int dimensions;
	int initMode;
	int *linear_from_parser;
	double refine_control;

    double*lbv,*ubv;
    double* uslbv, *usubv;
    char simuName[50];

    int refineunsafeflag = 0;

    dimValue = parser.scanner.dimensions;
    strcpy(simuName,parser.scanner.simulator);
    absoluteError = parser.scanner.absoluteError; relativeError = parser.scanner.relativeError;

    dimensions = dimValue;

    // Parses the values from C++ and parses the values from the input

    double* KConstArray;
    double* gammaValueArray;
    double* initM, *initB, *forbM, *forbB;
    linear_from_parser = parser.scanner.islinear;
    initM = parser.scanner.initMatrix; initB = parser.scanner.initB;
    forbM = parser.scanner.forbMatrix; forbB = parser.scanner.forbB;
    int initEquations, forbEquations;
    initEquations = parser.scanner.initEqns; forbEquations = parser.scanner.forbEqns;
    deltaValue = parser.scanner.deltaVal; timeStepValue = parser.scanner.tStep; globalTimeValue = parser.scanner.tGlobal;
    annotationTypeValue = parser.scanner.typeAnnot;
    KConstArray = parser.scanner.kConst; gammaValueArray = parser.scanner.gamma;
    kConstValue = 1; gammaValue = 0;
    //kConstValue = parser.scanner.kConst; gammaValue = parser.scanner.gamma;

    refine_control = deltaValue;

    visu1 = parser.scanner.val1; visu2 = parser.scanner.val2; strcpy(visuFileName,parser.scanner.visuFile);


    if (parser.scanner.initMode == -1){
    	initMode = 1;
    }
    else{
        initMode = parser.scanner.initMode;
    }


   /* cout<< "Simulator name - "<<simuName << "\n";
    cout << "absError "<< absoluteError << " relError " << relativeError << "\n";
    cout << "delta " << deltaValue <<"\n";
    cout << "time step " << timeStepValue << " GT " << globalTimeValue << " \n";
    cout << " K " << kConstValue << " gamma " << gammaValue << " visu1 " << visu1 << " visu2 " << visu2 << " File name-"<< visuFileName << "\n";
    cout << "is linear"<<linear_from_parser[0]<<endl;*/


    // New object for simulator
    class Simulator* simVerify = new Simulator();
    simVerify->setAbsError(absoluteError); simVerify->setRelError(relativeError); simVerify->setExecutable(simuName);
    simVerify->setGlobalTime(globalTimeValue); simVerify->setTimeStep(timeStepValue);

    // Generating an object for annotation
    class Annotation* annotVerify = new Annotation();
    annotVerify->setAnnotation(annotationString); annotVerify->setBeta(betaString);
    annotVerify->setGamma(gammaValue); annotVerify->setKConst(kConstValue); annotVerify->setType(annotationTypeValue);
    annotVerify->setKConst(KConstArray); annotVerify->setGamma(gammaValueArray);

    // Generating an object for checker
    class Checker* checkVerify = new Checker();

    // Visualizer for generating the reachset text files
    // Might be removed - not used anymore.
    /*class Visualizer* simuVisualize = new Visualizer();
    simuVisualize->setDim1(visu1); simuVisualize->setDim2(visu2);
*/
	int numberRefinements = 0;
	int refinementcounter = 0;

	class LinearSet* initialSet = new LinearSet();
	initialSet->setDimensions(dimensions);
	initialSet->setNumEqns(initEquations);
	initialSet->setMatrix(initM);
	initialSet->setB(initB);

	class LinearSet* unsafeSet = new LinearSet();
	unsafeSet->setDimensions(dimensions);
	unsafeSet->setNumEqns(forbEquations);
	unsafeSet->setMatrix(forbM);
	unsafeSet->setB(forbB);
	double* initMatrix, *bRow;


	int dimIndex;


	double deltaArray[dimensions];
	double initdeltaArray[dimensions];
	for (dimIndex =0; dimIndex <dimensions; dimIndex++){
		deltaArray[dimIndex] = (initialSet->getMax(dimIndex)-initialSet->getMin(dimIndex))/2;
		initdeltaArray[dimIndex] = deltaArray[dimIndex];
		//cout<< "deltaArray at " <<dimIndex<< " is " << deltaArray[dimIndex] <<endl; 
	}	


	//class InitialSet* deltaCover;

	// Computing the delta cover of the initial set.
	/*deltaCover = initialSet->getCover(deltaArray);
	deltaCover->setMode(initMode);
	deltaCover->setDelta(deltaArray);*/

	//deltaCover->print();

	class CoverStack* ItrStack;
	//class RepPoint* reptemp;

	ItrStack = initialSet->getCoverStack(deltaArray,initMode,0);
	/*reptemp = ItrStack->top();
	reptemp->print();
	ItrStack->pop();

	ItrStack->refine(reptemp,1);
	delete reptemp;
	cout<<ItrStack->size()<<endl;
	reptemp = ItrStack->top();
	reptemp->print();
	ItrStack->pop();
	delete reptemp;
	reptemp = ItrStack->top();
	reptemp->print();
	ItrStack->pop();
*/

	
	//cout << " Number of sample points " << deltaCover->getLength() << "\n";
	//cout << "--- Initializing iterator --- \n";
	//class InitialSet* Iterator;
	//Iterator = deltaCover;


	
	class Point* simulationPoint;
	

	//class InitialSet* NextSet;
	//NextSet = NULL;
	class ReachTube* nextReach = new ReachTube();
	nextReach->setDimensions(dimensions);

	std::vector<class ReachTube*> resultTube;
	std::vector<class ReachTube*> TraceTube;
	//std::vector<class ReachTube*> safeTubes;
	//std::vector<class ReachTube*> unknownTubes;
	//std::vector<class ReachTube*> unsafeTubes;

	class ReachTube* guardSet = new ReachTube();
	guardSet->setDimensions(dimensions);


	/* Initialized, now just add this to the Iterator when it becomes NULL */
	int numberSamplePoints =0;
	int modeSimulated;
	int traceSafeFlag;
	int numberOfPointsIterator = 0;
	int numRefinedPoints = 0;
	int numUnsafePoints = 0;
	FILE *fid = NULL;
	char *filename = "ComputeLDF.py"; //python code filename
	char input_buff[128];
	char input_buff2[32];
	char input_buff3[32];
	char input_buff4[32];

	Py_Initialize(); //starting Python environment

	// read refine order file here
	FILE* tRFile;
	tRFile = fopen("refineorder.txt","r");
	vector<int> refineorder;
	int bufferReader;
	if(tRFile == NULL){
		cout<<"refineorder file does not exist, will do refinement by default method"<<endl;
		refine_control = 1.0;
	}
	else{
		cout<<"refineorder file detected, scan refine order file"<<endl;
		while( fscanf(tRFile,"%d",&bufferReader) != EOF ){
			if(bufferReader<=dimensions){
				refineorder.push_back(bufferReader);
			}
		}
	}

	//Check which index is contained in unsafe set
	int refineflag [dimensions];
	for (int i=0; i<dimensions; i++){
		refineflag[i] = 0;
	}

	int forbMsize = forbEquations*dimensions;
	for(int i=0; i<forbMsize; i++){
		if(forbM[i]!=0.0 || forbM[i]!=0.0){
			//cout<<"forb matrix at "<<i<< " is not 0"<<endl;
			refineflag[i%dimensions] = 1;
		}
	}

	std::vector<int> index_in_unsafe_set;
	for(int i=0; i<dimensions; i++){
		if(refineflag[i] == 1){
			index_in_unsafe_set.push_back(i);
		}
	}
	int indexitr = 0;


	//playground
	
/*	class Point* TestPoint1;
	class Point* TestPoint2;
	class Point* TestPoint3;
	TestPoint1= new Point(3);
	TestPoint1->setCoordinate(0,0);
	TestPoint1->setCoordinate(1,1);
	TestPoint1->setCoordinate(2,2);
	TestPoint2= new Point(3);
	TestPoint2->setCoordinate(0,0);
	TestPoint2->setCoordinate(1,4);
	TestPoint2->setCoordinate(2,5);
	TestPoint3= new Point(3);
	TestPoint3->setCoordinate(0,0);
	TestPoint3->setCoordinate(1,7);
	TestPoint3->setCoordinate(2,8);


	class Point *temppoint = new Point();
	*temppoint = *TestPoint3;
	TestPoint3->setCoordinate(0,4);
	cout<<TestPoint3->getDimension()<<endl;
	cout<<TestPoint3->getCoordiate(0)<<endl;
	cout<<TestPoint3->getCoordiate(1)<<endl;

	delete TestPoint3;
	cout<<temppoint->getDimension()<<endl;
	cout<<temppoint->getCoordiate(0)<<endl;
	cout<<temppoint->getCoordiate(1)<<endl;


	class RepPoint* repPoint1;
	class RepPoint* repPoint2;
	class RepPoint* temp;


	repPoint1 = new RepPoint();
	repPoint1->setState(TestPoint1);
	repPoint1->setDimension(2);
	repPoint1->setDeltaArray(deltaArray);
	repPoint1->setMode(1);
	repPoint2 = new RepPoint();
	repPoint2->setState(TestPoint2);
	repPoint2->setDimension(2);
	repPoint2->setDeltaArray(deltaArray);
	repPoint2->setMode(1);
	repPoint2->setParentState(TestPoint3);
	repPoint2->setParentDeltaArray(deltaArray);

	temp = new RepPoint();
	temp = *repPoint2;
	repPoint2->setMode(4);
	repPoint2->print();
	temp ->print();

	class CoverStack* initstack;
	initstack = new CoverStack();
	
	
	initstack->push(repPoint2);

	initstack->push(repPoint1);

	cout<<initstack->size()<<endl;

	temp = initstack->top();
	temp->print();
	initstack->pop();

	cout<<initstack->size()<<endl;

	temp = initstack->top();
	temp->print();
	initstack->pop();

	cout<<initstack->empty()<<endl;*/

	cout<<"|     |"<<endl;
	cout<<"|     |"<<endl;
	for(int i=0; i<ItrStack->size();i++)
		cout<<"|=====|"<<endl;
	cout<<"-------"<<endl;

	class RepPoint* curItrRepPoint;
	while(!ItrStack->empty()){

		//Step0. Get value for Rep Point
		numberSamplePoints++;
		//cout  << "\n Sample point " << numberSamplePoints << " being checked \n";
		

		curItrRepPoint = ItrStack->top();
		ItrStack->pop();

		cout<<"========================POP 1 REP POINT, VERFICATION PROCESS START=================================="<<endl;
		cout<<"|     |"<<endl;
		cout<<"|     |"<<endl;
		for(int i=0; i<ItrStack->size();i++)
			cout<<"|=====|"<<endl;
		cout<<"-------"<<endl;
		curItrRepPoint->print();
		


		simulationPoint = curItrRepPoint->getState();
		modeSimulated = curItrRepPoint->getMode();
		double* refDeltaArray = curItrRepPoint->getDeltaArray();

		//Step1. Simulation
		simVerify->Simulate(simulationPoint,modeSimulated);

		//Read simulation result
		class ReachTube* simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("SimuOutput");

		//Step2. Set up python value for Non-Linear bloating, bloating with python helper function
		if (linear_from_parser[modeSimulated-1]==0){
			//cout<<"non-linear model"<<endl;
			double CT_step = KConstArray[modeSimulated-1];
			int modeforpython = simulationTube->getMode();

		    strcpy (input_buff, "delta = [");

  			for (int i = 0; i< dimensions; i++){
    			char temp [8];
    			sprintf(temp,"%f", refDeltaArray[i]);
    			strcat(input_buff,temp);
    			if (i<dimensions-1)
        			strcat(input_buff,",");
  			}
  			strcat(input_buff,"]");
  			//cout<<input_buff<<endl;
		    PyRun_SimpleString(input_buff);
		    sprintf(input_buff2,"CT_step = int(%f)", CT_step);
		    PyRun_SimpleString(input_buff2);
		    sprintf(input_buff3,"state = '%d'", modeforpython);
		    PyRun_SimpleString(input_buff3);
		    sprintf(input_buff4,"Is_linear = int(%f)",linear_from_parser[modeSimulated-1]);
		    PyRun_SimpleString(input_buff4);
		  
		    fid = fopen(filename, "r");
		    PyRun_SimpleFile(fid, filename);
		    fclose(fid);
		}

		// Bloating for Linear Model
		else{
			//cout<<"linear model"<<endl;
		    class ReachTube* bloatedTube;
			// Obtaining the bloating of the tube
			bloatedTube = simulationTube->bloatReachTube(refDeltaArray,annotVerify);
			// Printing the bloated tube;
			bloatedTube->printReachTube("reachtube.dat",0);
		}


		//Step3. Check invarant and guard
		system("./invariants");
		system("./guards");

		//Step4. Check unsafe 
		class ReachTube* invariantTube = new ReachTube();
		invariantTube->setDimensions(dimensions);
		invariantTube->setMode(modeSimulated);
		invariantTube->parseInvariantTube("invariant.dat");
		traceSafeFlag = checkVerify->check(invariantTube, unsafeSet);

		if(traceSafeFlag == 0){
			//Tube unknow, trace to the origin and refine immedately 
			cout << " Tube unknown! Start to Refine\n";
			TraceTube.clear();
			int i;
			double* originDeltaArray;
			if (curItrRepPoint->getParentState()!=NULL)
				originDeltaArray = curItrRepPoint->getParentDeltaArray();
			else
				originDeltaArray = curItrRepPoint->getDeltaArray();
			
			if (refine_control>0){
				//cout<<"using default method refinement"<<endl;
					if (refineunsafeflag<4)
					{
						//refine the dimension in unsafe equation 2 times
						ItrStack->refine(curItrRepPoint,index_in_unsafe_set.at(indexitr%index_in_unsafe_set.size()));
						indexitr++;
						refineunsafeflag++;
					}

					else{
						double max = 0;
						int refineidx=0;
						for(i=0; i<dimensions;i++){
							if (originDeltaArray[i]>max){
								max = originDeltaArray[i];
								refineidx = i;
							}
						}

						ItrStack->refine(curItrRepPoint,refineidx);
						refineunsafeflag = 0;
					}

			}

			else{
				//cout<<"using method written in file for refinement"<<endl;
				int refineidx = curItrRepPoint->getRefineTime();
				ItrStack->refine(curItrRepPoint,refineidx%dimensions);
			}
			delete curItrRepPoint;
		}

		if(traceSafeFlag == 1){
			int ifnextSet=0;
			cout<< "Tube Safe! Check if there is transition for next mode\n";
			TraceTube.push_back(invariantTube);
			guardSet->parseGuardsTube("guard.dat");

			ifnextSet = guardSet->getNextSetStack(ItrStack,curItrRepPoint);

			if (!ifnextSet){
				resultTube.reserve(resultTube.size()+TraceTube.size());
				resultTube.insert(resultTube.end(),TraceTube.begin(),TraceTube.end());
				TraceTube.clear();
			}
			delete curItrRepPoint;
			delete guardSet;
			guardSet = new ReachTube();
			guardSet->setDimensions(dimensions);

		}

		if(traceSafeFlag == -1){
			cout<<"Tube Unsafe, Break"<<endl;
			TraceTube.push_back(invariantTube);
			resultTube.reserve(resultTube.size()+TraceTube.size());
			resultTube.insert(resultTube.end(),TraceTube.begin(),TraceTube.end());
			TraceTube.clear();
			delete curItrRepPoint;
			class ReachTube* InvTube = NULL;
			int ResultTubeLength = 0;
			for(ResultTubeLength=0;ResultTubeLength<resultTube.size();ResultTubeLength++){
				InvTube = resultTube.at(0);
				InvTube->printReachTube(visuFileName,1);
			}

			cout << " The system is unsafe \n";
			ofstream resultStream;
			resultStream.open("Result.dat");
			resultStream << "-1" << endl;
			resultStream.close();

			exit(-1);

		}
		
	}

	class ReachTube* InvTube = NULL;
	int ResultTubeLength = 0;
	for(ResultTubeLength=0;ResultTubeLength<resultTube.size();ResultTubeLength++){
		InvTube = resultTube.at(ResultTubeLength);
		InvTube->printReachTube(visuFileName,1);
	}

	cout << "System is safe" << endl;


	ofstream resultStream;
	resultStream.open("Result.dat");
	resultStream << "1" << endl;
	resultStream << numberSamplePoints << endl;
	resultStream << numberRefinements << endl;
	resultStream.close();
	Py_Finalize(); //finalize Python session

	exit(1);





/*
	//Iterator = NULL;
	while(Iterator!= NULL){

		// Delayed strategy. Collect all the refined states, check whether they are
		// part of the initial set, or the collapsed set.
		double* refDeltaArray = Iterator->getDelta();

		numberSamplePoints++;
		numberOfPointsIterator++;
		cout  << "\n Sample point " << numberSamplePoints << " being checked \n";
		
		simulationPoint = Iterator->getState();
		
		modeSimulated = Iterator->getMode();

		cout <<" current mode is "<<modeSimulated<<endl;

		simVerify->Simulate(simulationPoint,modeSimulated);

		class ReachTube* simulationTube = new ReachTube();

		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("SimuOutput");

		


		if (linear_from_parser[modeSimulated-1]==0){
			cout<<"non-linear model"<<endl;
			double CT_step = KConstArray[modeSimulated-1];
			
			int modeforpython = simulationTube->getMode();
		

		    strcpy (input_buff, "delta = [");
  			for (int i = 0; i< dimensions; i++){
    			char temp [8];
    			sprintf(temp,"%f", refDeltaArray[i]);
    			strcat(input_buff,temp);
    			if (i<dimensions-1)
        			strcat(input_buff,",");
  			}
  			strcat(input_buff,"]");
  			cout<<input_buff<<endl;
		    PyRun_SimpleString(input_buff);
		    sprintf(input_buff2,"CT_step = int(%f)", CT_step);
		    PyRun_SimpleString(input_buff2);
		    sprintf(input_buff3,"state = '%d'", modeforpython);
		    PyRun_SimpleString(input_buff3);
		    sprintf(input_buff4,"Is_linear = int(%f)",linear_from_parser[modeSimulated-1]);
		    PyRun_SimpleString(input_buff4);
		  
		    fid = fopen(filename, "r");
		    PyRun_SimpleFile(fid, filename);
		    fclose(fid);
		}

		
		else{
			cout<<"linear model"<<endl;
		    class ReachTube* bloatedTube;

		    strcpy (input_buff, "delta = [");
  			for (int i = 0; i< dimensions; i++){
    			char temp [8];
    			sprintf(temp,"%f", refDeltaArray[i]);
    			strcat(input_buff,temp);
    			if (i<dimensions-1)
        			strcat(input_buff,",");
  			}
  			strcat(input_buff,"]");
  			cout<<input_buff<<endl;

			// Obtaining the bloating of the tube
			bloatedTube = simulationTube->bloatReachTube(refDeltaArray,annotVerify);
			// Printing the bloated tube;
			bloatedTube->printReachTube("reachtube.dat",0);
		}

	    //sleep(1);
		

		system("./invariants");
		system("./guards");
	
		class ReachTube* invariantTube = new ReachTube();
		invariantTube->setDimensions(dimensions);
		invariantTube->setMode(modeSimulated);
		invariantTube->parseInvariantTube("invariant.dat");


		traceSafeFlag = checkVerify->check(invariantTube, unsafeSet);

		//build my own checker for the output
		cout<<"current simulation point "<<numberSamplePoints<<endl;

	

		if(traceSafeFlag == 0){


			numRefinedPoints++;
			cout << " Tube unknown! \n";
			unknownTubes.push_back(invariantTube);


		}
		if(traceSafeFlag == 1){

			safeTubes.push_back(invariantTube);
			invariantTube->printReachTube(visuFileName,1);

			cout << " Tube verified to be safe \n";

			// Collects all the points as ReachTube
			guardSet->parseGuardsTube("guard.dat");



		}
		if(traceSafeFlag == -1){


			numUnsafePoints++;
			cout << "Tube unsafe \n";
			unsafeTubes.push_back(invariantTube);

			
		}

		Iterator = Iterator->getNext();
	

		if(Iterator == NULL){

			int collectNextGuards=0;
			int refineInitialSet=0;
			int printUnsafeSet=0;

			if(numRefinedPoints == 0 && numUnsafePoints == 0){
				collectNextGuards = 1;
			}
			else if(numRefinedPoints != 0 && numUnsafePoints == 0){
				refineInitialSet = 1;
			}
			else if(numUnsafePoints != 0){
				printUnsafeSet = 1;
			}

			if(collectNextGuards == 1){

				// Logic for printing all the safe invariant tubes and
				// collect all the next states
				cout<<"\n ------------------start to collect next Guard--------------------"<<endl;
				class InitialSet* nextSet;
				nextSet = guardSet->getNextSet();
				
				Iterator = nextSet;
				if (nextSet!=NULL)
					cout << "Number of next states is " << nextSet->getLength() << endl;
				else{
					cout << "Number of next states is 0" << endl;
				}

				class ReachTube* safeInvTube = NULL;
				int lengthSafeTubes = 0;
			

				safeTubes.clear();
				unsafeTubes.clear();
				unknownTubes.clear();

				guardSet = new ReachTube();
				guardSet->setDimensions(dimensions);
				numberOfPointsIterator = 0;
				numRefinedPoints = 0;
				numUnsafePoints = 0;
				refinementcounter = 0;

			}


	
			if(refineInitialSet == 1){
				int i;




				cout << "\n -----------------Tube unsafe, Need to refine-------------------"<<endl;;
				numberRefinements++;
				refinementcounter++;

				numRefinedPoints=0;
				numUnsafePoints=0;
				numberOfPointsIterator = 0;

				class InitialSet* refinedSet;

				int refineflag [dimensions];
				for (i=0; i<dimensions; i++){
					refineflag[i] = 0;
				}
				
				int forbMsize = forbEquations*dimensions;
				//forbMsize =2;
				//cout<< "the forbMSize is "<<forbMsize<<endl;
				if (refine_control>0){

					cout<<"using default method refinement"<<endl;
					if (refineunsafeflag<4)
					{
						//refine the dimension in unsafe equation 2 times

						for(i=0; i<forbMsize; i++){
							if(forbM[i]!=0.0 || forbM[i]!=0.0){
								cout<<"forb matrix at "<<i<< " is not 0"<<endl;
								refineflag[i%dimensions] = 1;
							}
						}

						for (i=0; i<dimensions;i++){
							if (refineflag[i]==1)
								initdeltaArray[i] = initdeltaArray[i]*0.5;
						}

						refineunsafeflag++;
					}

					else{
						//refine the largest dimension once
						double min = 0;
						int refindex=0;
						for(i=0; i<dimensions;i++){
							if (initdeltaArray[i]>min){
								min = initdeltaArray[i];
								refindex = i;
							}
						}

						initdeltaArray[refindex] = initdeltaArray[refindex] *0.5;
						refineunsafeflag = 0;
					}
				}

				else{
					cout<<"using method written in file for refinement"<<endl;
					int refindex = refinementcounter%refineorder.size();
					initdeltaArray[refineorder[refindex]-1]*=0.5;
					cout <<"refine dimensions " << refindex-1 <<endl;  

				}


				refinedSet = initialSet->getCover(initdeltaArray);
				refinedSet->setMode(initMode);
				refinedSet->setDelta(initdeltaArray);
				

				Iterator = refinedSet;
				cout << "Number of refined points is " << refinedSet->getLength() << endl;


			}

			if(printUnsafeSet == 1){

				class ReachTube* unsafeInvTube = NULL;
				int lengthUnsafeTubes = 0;
				//for(lengthUnsafeTubes=0;lengthUnsafeTubes<unsafeTubes.size();lengthUnsafeTubes++){
					unsafeInvTube = unsafeTubes.at(0);
					unsafeInvTube->printReachTube(visuFileName,2);
				//}

				cout << "  -- Total points checked " << numberSamplePoints << endl;
				cout << " -- Number of refinements " << numberRefinements << endl;

				cout << " The system is unsafe \n";
				ofstream resultStream;
				resultStream.open("Result.dat");
				resultStream << "-1" << endl;
				resultStream << numberSamplePoints << endl;
				resultStream << numberRefinements << endl;
				resultStream.close();

				exit(-1);


			}


		
		}


	}



	cout << " ---- Guard Sets being done ----  " << endl;

	cout << "  -- Total points checked " << numberSamplePoints << endl;
	cout << " -- Number of refinements " << numberRefinements << endl;
	cout << "System is safe" << endl;


	ofstream resultStream;
	resultStream.open("Result.dat");
	resultStream << "1" << endl;
	resultStream << numberSamplePoints << endl;
	resultStream << numberRefinements << endl;
	resultStream.close();
	Py_Finalize(); //finalize Python session

	exit(1);
*/
}

