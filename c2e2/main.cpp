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


	clock_t begin = clock();

	Parser parser;
	parser.parse();

	int dimValue;
	double absoluteError;
	double relativeError;
	double deltaValue;
	double timeStepValue;
	double globalTimeValue;
	int annotationTypeValue;
	int simulation_flag;
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
    simulation_flag = parser.scanner.simuval;
    int initEquations, forbEquations;
    initEquations = parser.scanner.initEqns; forbEquations = parser.scanner.forbEqns;
    refine_control = parser.scanner.refineVal; timeStepValue = parser.scanner.tStep; globalTimeValue = parser.scanner.tGlobal;
    annotationTypeValue = parser.scanner.typeAnnot;
    KConstArray = parser.scanner.kConst; gammaValueArray = parser.scanner.gamma;
    kConstValue = 1; gammaValue = 0;
    //kConstValue = parser.scanner.kConst; gammaValue = parser.scanner.gamma;

   

    visu1 = parser.scanner.val1; visu2 = parser.scanner.val2; strcpy(visuFileName,parser.scanner.visuFile);


    if (parser.scanner.initMode == -1){
    	initMode = 1;
    }
    else{
        initMode = parser.scanner.initMode;
    }


    //cout<<"The parser value of simu is "<<simulation_flag<<endl;

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



	class CoverStack* ItrStack;
	//class RepPoint* reptemp;

	ItrStack = initialSet->getCoverStack(deltaArray,initMode,0);

	
	class Point* simulationPoint;
	
	class ReachTube* nextReach = new ReachTube();
	nextReach->setDimensions(dimensions);

	std::vector<class ReachTube*> resultTube;
	std::vector<class ReachTube*> TraceTube;
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
		refine_control = 0;
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
			
			if (refine_control==0){
				cout<<"using default method refinement"<<endl;
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
				cout<<"using method written in file for refinement"<<endl;
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

			clock_t end = clock();
			double elapsed_sec = double(end-begin)/CLOCKS_PER_SEC;
			cout << "Execution time "<<elapsed_sec<<endl;
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
	clock_t end = clock();
	double elapsed_sec = double(end-begin)/CLOCKS_PER_SEC;
	cout << "Execution time "<<elapsed_sec<<endl;

	exit(1);
}