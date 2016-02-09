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
	double delta;

    double*lbv,*ubv;
    double* uslbv, *usubv;
    char simuName[50];


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

    delta = deltaValue;

    visu1 = parser.scanner.val1; visu2 = parser.scanner.val2; strcpy(visuFileName,parser.scanner.visuFile);


    if (parser.scanner.initMode == -1){
    	initMode = 1;
    }
    else{
        initMode = parser.scanner.initMode;
    }


    cout<< "Simulator name - "<<simuName << "\n";
    cout << "absError "<< absoluteError << " relError " << relativeError << "\n";
    cout << "delta " << deltaValue <<"\n";
    cout << "time step " << timeStepValue << " GT " << globalTimeValue << " \n";
    cout << " K " << kConstValue << " gamma " << gammaValue << " visu1 " << visu1 << " visu2 " << visu2 << " File name-"<< visuFileName << "\n";
    cout << "is linear"<<linear_from_parser[0]<<endl;


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
    class Visualizer* simuVisualize = new Visualizer();
    simuVisualize->setDim1(visu1); simuVisualize->setDim2(visu2);

    /*
     * Opening the visu-filename and checking if it is editable
     */
    ofstream tempcleanvisu;
    tempcleanvisu.open(visuFileName);
    tempcleanvisu.close();

	int numberRefinements = 0;

	// Initial and Unsafe set for verification
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

	//class Point* upperBoundPoint = new Point(dimensions);
	//class Point* lowerBoundPoint = new Point(dimensions);

	int dimIndex;


	double deltaArray[dimensions];
	for (dimIndex =0; dimIndex <dimensions; dimIndex++){
		deltaArray[dimIndex] = (initialSet->getMax(dimIndex)-initialSet->getMin(dimIndex))/2;
	}	



	class InitialSet* deltaCover;

	// Computing the delta cover of the initial set.
	deltaCover = initialSet->getCover(deltaArray);
	deltaCover->setMode(initMode);
	deltaCover->setDelta(deltaArray);
	
	/*double * temp1= deltaCover->getDelta();
	cout <<"first value in delta array of deltaCover "<< *(temp1)<<endl;
	cout <<"delta array mem"<< deltaCover->getDelta()<<endl;
	cout <<"state mem" <<deltaCover->getState() <<endl;*/

	
	cout << " Number of sample points " << deltaCover->getLength() << "\n";
	cout << "--- Initializing iterator --- \n";
	class InitialSet* Iterator;
	Iterator = deltaCover;

	/*deltaArray[0] =999;

	double * temp2 = Iterator->getDelta();
	cout <<"first value in delta array of iterator " <<temp2[0]<<endl;
	cout <<"delta array mem" <<Iterator->getDelta()<<endl;
	cout <<"state mem"<<Iterator->getState() <<endl;
	*/
	
	class Point* simulationPoint;
	

	class InitialSet* NextSet;
	NextSet = NULL;
	class ReachTube* nextReach = new ReachTube();
	nextReach->setDimensions(dimensions);

	std::vector<class ReachTube*> safeTubes;
	std::vector<class ReachTube*> unknownTubes;
	std::vector<class ReachTube*> unsafeTubes;

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
	
	while(Iterator!= NULL){

		// Delayed strategy. Collect all the refined states, check whether they are
		// part of the initial set, or the collapsed set.
		double* refDeltaArray = Iterator->getDelta();
		numberSamplePoints++;
		numberOfPointsIterator++;
		cout  << "\n Sample point " << numberSamplePoints << " being checked \n";
		
		simulationPoint = Iterator->getState();
		
		modeSimulated = Iterator->getMode();

		simVerify->Simulate(simulationPoint,modeSimulated);

		class ReachTube* simulationTube = new ReachTube();

		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("SimuOutput");

		


		if (linear_from_parser[modeSimulated-1]==0){
			cout<<"non-linear"<<endl;
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
			cout<<"linear"<<endl;
		    class ReachTube* bloatedTube;

			// Obtaining the bloating of the tube
			bloatedTube = simulationTube->bloatReachTube(refDeltaArray,annotVerify);
			// Printing the bloated tube;
			bloatedTube->printReachTube("reachtube.dat",0);
		}

	    //sleep(1);
		cout << "Started verification process \n";

		system("./invariants");
		system("./guards");
	
		class ReachTube* invariantTube = new ReachTube();
		invariantTube->setDimensions(dimensions);
		invariantTube->setMode(modeSimulated);
		invariantTube->parseInvariantTube("invariant.dat");


		traceSafeFlag = checkVerify->check(invariantTube, unsafeSet);

		//build my own checker for the output
		cout<<"current simulation point "<<numberSamplePoints<<endl;
	
		int checkresult;
		checkresult = invariantTube->checkunsafe(forbM, forbB,forbEquations);
		cout<<"my result is "<< checkresult << " glpk result is "<< traceSafeFlag<< endl;

	

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
				//cout<<"start to collect next Guard"<<endl;
				class InitialSet* nextSet;
				nextSet = guardSet->getNextSet();
				
				Iterator = nextSet;
				//cout << "Number of next states is " << nextLen << endl;

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

			}


	
			if(refineInitialSet == 1){
				int i;
				/*for (i =0 ; i<dimensions; i++){

					if(refDeltaArray[i] < 0.00005) {

						class ReachTube* unknownInvTube = NULL;
						int lengthUnknownTubes = 0;
						for(lengthUnknownTubes=0;lengthUnknownTubes<safeTubes.size();lengthUnknownTubes++){
							unknownInvTube = unknownTubes.at(lengthUnknownTubes);
							unknownInvTube->printReachTube(visuFileName,1);
						}

						ofstream resultStream;
						resultStream.open("Result.dat");
						resultStream << "-2" << endl;
						resultStream << numberSamplePoints << endl;
						resultStream << numberRefinements << endl;
						resultStream.close();
						cout << " Too robustly unsafe, please check design \n";
						cout << "  -- Total points checked " << numberSamplePoints << endl;
						cout << " -- Number of refinements " << numberRefinements << endl;
						exit(-2);
						cout << " Too robustly unsafe, please check design \n";
					}
				}*/

				cout << "Tube unsafe, Need to refine"<<endl;;
				numberRefinements++;

				numRefinedPoints=0;
				numUnsafePoints=0;
				numberOfPointsIterator = 0;

				class InitialSet* refinedSet;

				int refineflag [dimensions];
				for (i=0; i<dimensions; i++){
					refineflag[i] = 0;
				}
				
				int forbMsize = sizeof(forbM)/sizeof(forbM[0]);
				
				for(i=0; i<forbMsize; i++){
					if(forbMsize>0.0||forbMsize<0.0){
						cout<<"forb matrix at "<<i<< " is not 0"<<endl;
						refineflag[i%dimensions] = 1;
					}
				}

				for (i=0; i<dimensions;i++){
					if (refineflag[i]==1)
						refDeltaArray[i] = refDeltaArray[i]*0.5;
				}


				refinedSet = initialSet->getCover(refDeltaArray);
				refinedSet->setMode(initMode);
				

				Iterator = refinedSet;


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


	class InitialSet* counterNext = NULL;
	class Point* tempNextState = NULL;


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

}

