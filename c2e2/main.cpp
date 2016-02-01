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

class InitialSet* compress(class InitialSet* guardSet);
void printconstraints(char* String, class Polyhedron* reachSet,
		class Polyhedron* unsafeSet);
void printconstraints2(char* String, class Point* Pt1, class Point* Pt2,
		class Polyhedron* unsafeSet, double epsilon);
void printconstraints3(char* String, class Point* Pt1, class Point* Pt2,
		class Point* UP1, class Point* UP2, double epsilon);
void printconstraints4(char* String, class Polyhedron* reachset,
		class Point* UP1, class Point* UP2, double epsilon);
void printrealpaver(char* String, class Point* FirstPoint,
		class Point* SecondPoint, class Polyhedron* unsafeSet, double epsilon);
void printredlog(char* String, class Point* FirstPoint,
		class Point* SecondPoint, class Polyhedron* unsafeSet, double epsilon);
void Verify(int argc, char* argv[]);
double getEpsilon(double delta, int example);
int sanityCheck();
int GsanityCheck(char* Ptr);

int main(int argc, char* argv[]) {


	/* -- For redirecting file input from capdsimannot to
	 * stdin for parsing, currently changes the operation to file input
	// TODO Redirect the input stream from appropriate file.
 	std::ifstream in("capdsimannot");
    std::streambuf *cinbuf = std::cin.rdbuf(); //save old buf
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!
    */

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


    /*
     * Case where initial mode is not specified
     */

    if (parser.scanner.initMode == -1){
    	initMode = 1;
    }
    else{
        initMode = parser.scanner.initMode;
    }


    /*
     * Printing parameters to screen after parsing the input and before verification procedure;
     */

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

	class Point* upperBoundPoint = new Point(dimensions);
	class Point* lowerBoundPoint = new Point(dimensions);

	int dimIndex;

	for(dimIndex = 0; dimIndex < dimensions; dimIndex++){
		upperBoundPoint->setCoordinate(dimIndex,initialSet->getMax(dimIndex));
		lowerBoundPoint->setCoordinate(dimIndex,initialSet->getMin(dimIndex));
	}


	// Generating the partitioning from the initial set - would probably require adding the glpk here
	double minDelta = 1000000;
	double vmax,vmin;

	for(dimIndex=0;dimIndex < dimensions; dimIndex++){
		vmax = upperBoundPoint->getCoordiate(dimIndex);
		vmin = lowerBoundPoint->getCoordiate(dimIndex);
			if(minDelta > 0.6*(vmax - vmin) && vmin != vmax){
				minDelta = 0.6*(vmax - vmin);
			}
	}

	if(delta < 0 && minDelta > 0.00005){
		delta = minDelta;
	}

	class InitialSet* deltaCover;

	// Computing the delta cover of the initial set.
	deltaCover = initialSet->getCover(delta);
	deltaCover->setDelta(delta);
	deltaCover->setMode(initMode);

	// delta for initial set
	double deltaForInit = delta;
	double initialDelta = delta;

	cout << " Number of sample points " << deltaCover->getLength() << "\n";
	cout << "--- Initializing iterator --- \n";
	class InitialSet* Iterator;
	// Iterator = I;
	Iterator = deltaCover;

	double refDelta;
	class Point* simulationPoint;
	int numberSamplePoints;
	int modeSimulated;
	numberSamplePoints = 0;

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

	int continueIterator = 1;
	int numberOfPointsIterator = 0;
	int numRefinedPoints = 0;
	int numUnsafePoints = 0;
	FILE *fid = NULL;
	char *filename = "ComputeLDF.py"; //python code filename
	char input_buff[32];
	char input_buff2[32];
	char input_buff3[32];
	char input_buff4[32];
		  //double x = 0.0;
	cout<<"number of unsafe equation is "<<forbEquations<<endl;
	cout<<"the first value in unsafe matrix is "<<forbM[0]<<endl;
	cout<<"the first value in unsafe B is "<<forbB[0]<<endl;
	cout<<"length of unsafe matrix should be "<<dimensions*forbEquations<<endl;

	Py_Initialize(); //starting Python environment
	while(Iterator!= NULL){

		// Delayed strategy. Collect all the refined states, check whether they are
		// part of the initial set, or the collapsed set.

		numberSamplePoints++;
		numberOfPointsIterator++;
		cout  << "\n Sample point " << numberSamplePoints << " being checked \n";
		class InitialSet* stateTrace = new InitialSet();
		ifstream traceReadFile;
		double bufferReader;
		int traceSafeFlag;
		simulationPoint = Iterator->getState();
		refDelta = Iterator->getDelta();
		modeSimulated = Iterator->getMode();

		simVerify->Simulate(simulationPoint,modeSimulated);

		/*
		 * Here is where the simulation engine is called and the guard intersection
		 * is given to guardSet.dat file. Now, the C2E2 will parse the guardSet.dat file
		 * and get all the boxes and organize them into a set of boxes with delta as the
		 * diameter of each of them. After that, initialize the iterator with the new
		 * set collected.
		 *
		 */

		 //sleep(2);
		// Creates a reachTube from the simulation - sequence of rectangles
		class ReachTube* simulationTube = new ReachTube();

		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("SimuOutput");


		if (linear_from_parser[modeSimulated-1]==0){
			cout<<"non-linear"<<endl;
			double CT_step = KConstArray[modeSimulated-1];
			//double Islinear = gammaValueArray[modeSimulated-1];
			
			int modeforpython = simulationTube->getMode();
			/*
			char pythoncalling[100];
			sprintf(pythoncalling,"python ComputeLDF.py %.6f %.6f %d",refDelta,CT_step,modeforpython );	
			system(pythoncalling); */

			

		    sprintf(input_buff,"delta = %f", refDelta);
		    PyRun_SimpleString(input_buff);
		    sprintf(input_buff2,"CT_step = int(%f)", CT_step);
		    PyRun_SimpleString(input_buff2);
		    sprintf(input_buff3,"state = '%d'", modeforpython);
		    PyRun_SimpleString(input_buff3);
		    sprintf(input_buff4,"Is_linear = int(%f)",linear_from_parser[modeSimulated-1]);
		    PyRun_SimpleString(input_buff4);
		      // opening the file containing python code,
		      // NOTE IT MUST BE OPENED AND CLOSED BEFORE/AFTER EACH PYTHON RUN QUERY
		    fid = fopen(filename, "r");
		      //running the Python code
		    PyRun_SimpleFile(fid, filename);
		    fclose(fid);
		}

		
		else{
			cout<<"linear"<<endl;
		    class ReachTube* bloatedTube;

			// Obtaining the bloating of the tube
			bloatedTube = simulationTube->bloatReachTube(refDelta,annotVerify);
			// Printing the bloated tube;
			bloatedTube->printReachTube("reachtube.dat",0);
		}

	    //sleep(1);
		cout << "Started verification process \n";
		/*
		 * Checks for guards and invariants in the bloated rectangles obtained
		 * from simulation and the bloated tube
		 */
		system("./invariants");
		system("./guards");
		//sleep(3);

		// Parsing the invariants from the invariant file
		class ReachTube* invariantTube = new ReachTube();
		invariantTube->setDimensions(dimensions);
		invariantTube->setMode(modeSimulated);
		invariantTube->parseInvariantTube("invariant.dat");

		/* Checks that the given trace is safe w.r.t the boxes given as unsafe set */
		traceSafeFlag = checkVerify->check(invariantTube, unsafeSet);

		//build my own checker for the output
		cout<<"current simulation point "<<numberSamplePoints<<endl;
		//cout<<"max node trail"<<invariantTube->getMaxCoordinate(1,modeSimulated)<<endl;
		//vector<double> Maxvalue;
		//Maxvalue = invariantTube->MaxCoordinate(1);
		//vector<double> Minvalue;
		//Minvalue = invariantTube->MaxCoordinate(1);
		int checkresult;
		checkresult = invariantTube->checkunsafe(forbM, forbB,forbEquations);
		cout<<"my result is "<< checkresult << " glpk result is "<< traceSafeFlag<< endl;

		//cout<<"number of unsafe equation is "<<forbEquations<<endl;
		//cout<<"the first value in unsafe matrix is "<<forbM[0]<<endl;
		//cout<<"the first value in unsafe B is "<<forbB[0]<<endl;
		//cout<<"length of unsafe matrix should be "<<dimensions*forbEquations<<endl;
		int unsafeI=0;
		int lenofunsafeM = dimensions*forbEquations;
		 



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
		//Iterator = NULL;

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
				double nowDelta = refDelta;
				int nextLen;
				int error;
				int exceptPoints = 40*numberOfPointsIterator;
				while(1){
					nextSet = guardSet->getNextSet(nowDelta, timeStepValue,exceptPoints);
					if(nextSet != NULL){
						nextLen = nextSet->getLength();
						error = nextSet->getMode();
						if(nextLen <= 20*numberOfPointsIterator && error!=-999){
							break;
						}
						else{
							nowDelta = 1.5*nowDelta;
						}
					}
					if(nextSet == NULL){
						nextLen = 0;
						break;
					}
				}
				//nextSet->print();
				Iterator = nextSet;
				cout << "Number of next states is " << nextLen << endl;

				class ReachTube* safeInvTube = NULL;
				int lengthSafeTubes = 0;
				/*for(lengthSafeTubes=0;lengthSafeTubes<safeTubes.size();lengthSafeTubes++){
					safeInvTube = safeTubes.at(lengthSafeTubes);
					safeInvTube->printReachTube(visuFileName,1);
				}*/

				safeTubes.clear();
				unsafeTubes.clear();
				unknownTubes.clear();

				guardSet = new ReachTube();
				guardSet->setDimensions(dimensions);
				numberOfPointsIterator = 0;
				numRefinedPoints = 0;
				numUnsafePoints = 0;

			}


			// Iterator = nextSet;
			// if(nextSet != NULL){
			//	 cout << " The next set size is " << nextSet->getLength() << " OK \n";
			// }

			if(refineInitialSet == 1){

				if(refDelta < 0.00005) {

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

				cout << " \n Tube unsafe, Need to refine \n New delta = " << refDelta/2.0 << "\n";
				numberRefinements++;

				numRefinedPoints=0;
				numUnsafePoints=0;
				numberOfPointsIterator = 0;

				class InitialSet* refinedSet;
				deltaForInit = deltaForInit*0.5;
				refinedSet = initialSet->getCover(deltaForInit);
				refinedSet->setMode(initMode);
				refinedSet->setDelta(deltaForInit);

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


			//cout << " this is the next set? \n";

			//cout << "The length of the next thing is " << nextSet->getLength() << " OK \n";
			//nextSet->print();
			//exit(1);

			// First deal with the guard sets
			// Assert the next set of states with the modes to be visited


			// Logic that prints all the reachTubes at the end of each round

			/*
			if(NextSet != NULL){

//				cout << " next mode is " << NextSet->getMode() << endl;
//				cout << " Annotation details " << annotVerify->getKVal(NextSet->getMode()) << " and gamma " << annotVerify->getGammaVal(NextSet->getMode()) << endl;

				Iterator = NextSet;
				NextSet = NULL;

			}
			*/
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

class InitialSet* compress(class InitialSet* guardSet){

	if(guardSet == NULL)
		return NULL;

	int dimensions;
	double currDelta;
	double maxDelta;
	double incrementDelta;
	int curMode, nextMode;

	class InitialSet* NewCompressed = NULL;
	class InitialSet* tempCompressed;

	class Point* currState = guardSet->getState();
	if(currState != NULL){
		dimensions = currState->getDimension();
		dimensions = dimensions - 1;
	}

	class InitialSet* currGuardSetPointer;
	class Point* nextState;
	currGuardSetPointer = guardSet;
	while(currGuardSetPointer != NULL){

		tempCompressed = new InitialSet();
		currState = currGuardSetPointer->getState();
		currDelta = currGuardSetPointer->getDelta();
		curMode = currGuardSetPointer->getMode();
		maxDelta = 0;

		while(currGuardSetPointer != NULL && currState != NULL && maxDelta < currDelta/3.0){
			if(currGuardSetPointer->getNext() != NULL){
				nextState = currGuardSetPointer->getNext()->getState();
				nextMode = currGuardSetPointer->getNext()->getMode();
			}
			incrementDelta = 0;
			if(currState != NULL && nextState != NULL && curMode == nextMode){
				for(int h = 1; h < dimensions + 1; h++){
					double tempDiff = abs(currState->getCoordiate(h)-nextState->getCoordiate(h));
					incrementDelta = incrementDelta > tempDiff ? incrementDelta : tempDiff;
				}
			}
			maxDelta = maxDelta + incrementDelta;
			currGuardSetPointer = currGuardSetPointer->getNext();
		}

		tempCompressed->setState(currState);
		tempCompressed->setDelta(maxDelta+currDelta);
		tempCompressed->setMode(curMode);

		if(NewCompressed == NULL){
			NewCompressed = tempCompressed;
		}
		else{
			NewCompressed->append(tempCompressed);
		}

	}

	return NewCompressed;
}

void Verify(int argc, char* argv[]) {

}

void printconstraints4(char* Filename, class Polyhedron* reachSet,
		class Point* UnPt1, class Point* UnPt2, double epsilon){
	ofstream outputFile;
	outputFile.open(Filename);
	int reachIndex, unsafeIndex;
	int dimension, dimIndex;
	int reachSetPoints, unsafeSetPoints;

	dimension = reachSet->getPoint(0)->getDimension();
	reachSetPoints = reachSet->getSize();

	outputFile << "Minimize  \n v: Lr0 \n \n";

	outputFile << "Subject To \n  \n";

	outputFile << "sumr1: ";
	for (reachIndex = 0; reachIndex < reachSetPoints - 1; reachIndex++) {
		outputFile << " Lrs" << reachIndex << " + ";
	}
	outputFile << " Lrs" << reachSetPoints - 1 << " = 1.0 \n";

	for (dimIndex = 0; dimIndex < dimension; dimIndex++) {
		outputFile << "dim" << dimIndex << ": ";
		for (reachIndex = 0; reachIndex < reachSetPoints; reachIndex++) {
			if (reachSet->getPoint(reachIndex)->getCoordiate(dimIndex) >= 0)
				outputFile << " + "
						<< fixed << reachSet->getPoint(reachIndex)->getCoordiate(
								dimIndex) << " Lrs" << reachIndex << " ";
			else
				outputFile << " - "
						<< fixed << -1 * reachSet->getPoint(reachIndex)->getCoordiate(
								dimIndex) << " Lrs" << reachIndex << " ";
		}
		outputFile << " - x"<< dimIndex << " = 0.0 \n";

	}

	for(dimIndex=0;dimIndex<dimension; dimIndex++){
		outputFile << "dimlobd" << dimIndex <<": ";
		outputFile << " x"<<dimIndex << " ";
		outputFile << " >= " << UnPt1->getCoordiate(dimIndex) << " \n";
		outputFile << "dimupbd" << dimIndex <<": ";
		outputFile << " x"<<dimIndex << " ";
		outputFile << " <= " << UnPt2->getCoordiate(dimIndex) << " \n";
	}

	outputFile << "\nBounds  \n";
	for (reachIndex = 0; reachIndex < reachSetPoints; reachIndex++)
		outputFile << " 0 <= Lrs" << reachIndex << " <= 1 \n";

	outputFile << " \nEnd \n";

	outputFile.close();

}

void printconstraints3(char* Filename, class Point* Pt1, class Point* Pt2,
		class Point* UnPt1, class Point* UnPt2, double epsilon){

	ofstream outputFile;
	outputFile.open(Filename);
	int dimensions, dimIndex;
	int unsafeIndex;
	int unsafeSetPoints;

	dimensions = Pt1->getDimension();

	outputFile << "Minimize \n v: x0 \n \n";

	outputFile << "Subject To \n  \n";

	outputFile << "sumr1: Lrs1 + Lrs2 = 1.0 \n";

	outputFile << "sumu1: ";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints - 1; unsafeIndex++) {
		outputFile << " Lus" << unsafeIndex << " + ";
	}
	outputFile << " Lus" << unsafeSetPoints - 1 << " = 1.0 \n";


	for(dimIndex = 0; dimIndex < dimensions; dimIndex++){
		outputFile << " dimxub"<< dimIndex << ": ";
		outputFile << " x"<<dimIndex << " ";
		if(Pt1->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		outputFile << " <= "<< fixed << epsilon <<" \n";

		outputFile << " dimxlb"<< dimIndex << ": ";
		outputFile << " x"<<dimIndex << " ";
		if(Pt1->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		outputFile << " >= "<< fixed << -1*epsilon <<" \n";
	}

	for(dimIndex=0;dimIndex<dimensions; dimIndex++){
		outputFile << "dimlobd" << dimIndex <<": ";
		outputFile << " x"<<dimIndex << " ";
		outputFile << " >= " << UnPt1->getCoordiate(dimIndex) << " \n";
		outputFile << "dimupbd" << dimIndex <<": ";
		outputFile << " x"<<dimIndex << " ";
		outputFile << " <= " << UnPt2->getCoordiate(dimIndex) << " \n";
	}

	outputFile << "\nBounds  \n";
	outputFile << " 0 <= Lrs1 <= 1 \n 0 <= Lrs2 <= 1 \n";

	outputFile << " \nEnd \n";

	outputFile.close();

}

void printconstraints2(char* Filename, class Point* Pt1, class Point* Pt2,
		class Polyhedron* unsafeSet, double epsilon) {

	ofstream outputFile;
	outputFile.open(Filename);
	int dimensions, dimIndex;
	int unsafeIndex;
	int unsafeSetPoints;

	dimensions = Pt1->getDimension();
	unsafeSetPoints = unsafeSet->getSize();

	outputFile << "Minimize \n v: x0 \n \n";

	outputFile << "Subject To \n  \n";

	outputFile << "sumr1: Lrs1 + Lrs2 = 1.0 \n";

	outputFile << "sumu1: ";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints - 1; unsafeIndex++) {
		outputFile << " Lus" << unsafeIndex << " + ";
	}
	outputFile << " Lus" << unsafeSetPoints - 1 << " = 1.0 \n";


	for(dimIndex = 0; dimIndex < dimensions; dimIndex++){
		outputFile << " dimxub"<< dimIndex << ": ";
		outputFile << " x"<<dimIndex << " ";
		if(Pt1->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		outputFile << " <= "<< fixed << epsilon <<" \n";

		outputFile << " dimxlb"<< dimIndex << ": ";
		outputFile << " x"<<dimIndex << " ";
		if(Pt1->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt1->getCoordiate(dimIndex) << " Lrs1 ";
		}
		if(Pt2->getCoordiate(dimIndex) >= 0){
			outputFile << " - " << fixed << Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		else{
			outputFile << " + " << fixed << -1*Pt2->getCoordiate(dimIndex) << " Lrs2 ";
		}
		outputFile << " >= "<< fixed << -1*epsilon <<" \n";
	}

	for(dimIndex=0; dimIndex < dimensions; dimIndex++){
		outputFile << "dim" << dimIndex << ": ";
		outputFile << " x" << dimIndex << " ";
		for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++) {
			if (unsafeSet->getPoint(unsafeIndex)->getCoordiate(dimIndex) >= 0)
				outputFile << " - "
						<< fixed << unsafeSet->getPoint(unsafeIndex)->getCoordiate(
								dimIndex) << " Lus" << unsafeIndex << " ";
			else
				outputFile << " + " << fixed << -1
						* unsafeSet->getPoint(unsafeIndex)->getCoordiate(
								dimIndex) << " Lus" << unsafeIndex << " ";
		}
		outputFile << "  =  0.0 \n";
	}

	outputFile << "\nBounds  \n";
	outputFile << " 0 <= Lrs1 <= 1 \n 0 <= Lrs2 <= 1 \n";

//	for (reachIndex = 0; reachIndex < reachSetPoints; reachIndex++)
//		outputFile << " 0 <= Lrs" << reachIndex << " <= 1 \n";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++)
		outputFile << " 0 <= Lus" << unsafeIndex << " <= 1 \n";

	outputFile << " \nEnd \n";

	outputFile.close();


}

void printconstraints(char* Filename, class Polyhedron* reachSet,
		class Polyhedron* unsafeSet) {

	ofstream outputFile;
	outputFile.open(Filename);
	int reachIndex, unsafeIndex;
	int dimension, dimIndex;
	int reachSetPoints, unsafeSetPoints;

	dimension = reachSet->getPoint(0)->getDimension();
	reachSetPoints = reachSet->getSize();
	unsafeSetPoints = unsafeSet->getSize();

	outputFile << "Minimize  \n v: Lr0 \n \n";

	outputFile << "Subject To \n  \n";

	outputFile << "sumr1: ";
	for (reachIndex = 0; reachIndex < reachSetPoints - 1; reachIndex++) {
		outputFile << " Lrs" << reachIndex << " + ";
	}
	outputFile << " Lrs" << reachSetPoints - 1 << " = 1.0 \n";

	outputFile << "sumu1: ";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints - 1; unsafeIndex++) {
		outputFile << " Lus" << unsafeIndex << " + ";
	}
	outputFile << " Lus" << unsafeSetPoints - 1 << " = 1.0 \n";

	for (dimIndex = 0; dimIndex < dimension; dimIndex++) {
		outputFile << "dim" << dimIndex << ": ";
		for (reachIndex = 0; reachIndex < reachSetPoints; reachIndex++) {
			if (reachSet->getPoint(reachIndex)->getCoordiate(dimIndex) >= 0)
				outputFile << " + "
						<< fixed << reachSet->getPoint(reachIndex)->getCoordiate(
								dimIndex) << " Lrs" << reachIndex << " ";
			else
				outputFile << " - "
						<< fixed << -1 * reachSet->getPoint(reachIndex)->getCoordiate(
								dimIndex) << " Lrs" << reachIndex << " ";
		}
		for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++) {
			if (unsafeSet->getPoint(unsafeIndex)->getCoordiate(dimIndex) >= 0)
				outputFile << " - "
						<< fixed << unsafeSet->getPoint(unsafeIndex)->getCoordiate(
								dimIndex) << " Lus" << unsafeIndex << " ";
			else
				outputFile << " + " << fixed << -1
						* unsafeSet->getPoint(unsafeIndex)->getCoordiate(
								dimIndex) << " Lus" << unsafeIndex << " ";
		}
		outputFile << "  =  0.0 \n";
	}

	outputFile << "\nBounds  \n";
	for (reachIndex = 0; reachIndex < reachSetPoints; reachIndex++)
		outputFile << " 0 <= Lrs" << reachIndex << " <= 1 \n";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++)
		outputFile << " 0 <= Lus" << unsafeIndex << " <= 1 \n";

	outputFile << " \nEnd \n";

	outputFile.close();
}

void printrealpaver(char* Filename, class Point* FirstPoint,
		class Point* SecondPoint, class Polyhedron* unsafeSet, double epsilon) {
	ofstream outputFile;
	outputFile.open(Filename);

	//	cout<< "opening file in real paver output \n";
	int unsafeIndex, unsafeSetPoints;
	unsafeSetPoints = unsafeSet->getSize();
	int dimensions, dimIndex;
	dimensions = FirstPoint->getDimension();
	outputFile << " Variables \n";
	for (dimIndex = 0; dimIndex < dimensions; dimIndex++) {
		outputFile << " real us" << dimIndex << " in ]-oo, +oo[,\n";
		outputFile << " real cp" << dimIndex << " in ]-oo, +oo[,\n";
	}
	//	cout<< "printing first set of variables\n";
	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++) {
		outputFile << " real Lus" << unsafeIndex << " in [0,1],\n";
	}
	outputFile << " real l in [0,1];\n";
	outputFile << "  \n  \n";
	outputFile << " Constraints \n";

	for (dimIndex = 0; dimIndex < dimensions; dimIndex++) {
		outputFile << " cp" << dimIndex << " = l*" << FirstPoint->getCoordiate(
				dimIndex) << " + (1-l)*" << SecondPoint->getCoordiate(dimIndex)
				<< ",\n \n ";
	}

	//	cout << "printing the us constraint \n";

	for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints; unsafeIndex++) {
		outputFile << " Lus" << unsafeIndex << " + ";
	}
	outputFile << " 0 = 1,\n";

	for (dimIndex = 0; dimIndex < dimensions; dimIndex++) {
		outputFile << " us" << dimIndex << " = ";
		for (unsafeIndex = 0; unsafeIndex < unsafeSetPoints - 1; unsafeIndex++) {
			outputFile << " Lus" << unsafeIndex << "*" << unsafeSet->getPoint(
					unsafeIndex)->getCoordiate(dimIndex) << " + ";
		}
		//		cout << " printing the last variable \n -- "<< dimIndex << " -- "<< dimensions <<"\n";
		outputFile << "  Lus" << unsafeSetPoints - 1 << "*"
				<< unsafeSet->getPoint(unsafeSetPoints - 1)->getCoordiate(
						dimIndex) << " ,\n";
	}

	//	cout << "Just printing the last constraint \n";
	for (dimIndex = 0; dimIndex < dimensions - 1; dimIndex++) {
		outputFile << "( cp" << dimIndex << " - us" << dimIndex << " )^2 + ";
	}
	outputFile << "( cp" << dimensions - 1 << " - us" << dimensions - 1
			<< " )^2 <= " << epsilon * epsilon << ";\n";

	outputFile.close();

}

void printredlog(char* Filename, class Point* FirstPoint,
		class Point* SecondPoint, class Polyhedron* unsafeSet, double epsilon) {

	ofstream outputFile;
	outputFile.open(Filename);

	outputFile << "load redlog;\n";
	outputFile << "rlset ofsf;\n \n";

	int dim, index;
	int unsafeSetIndex;
	dim = FirstPoint->getDimension();

	outputFile << "Formula := ex({ ";
	for (index = 0; index < dim; index++) {
		outputFile << "cp" << index << ",";
		outputFile << "unsafe" << index << ",";
	}
	for (index = 0; index < unsafeSet->getSize(); index++) {
		outputFile << "Lus" << index << ",";
	}
	outputFile << "l},(";

	for (index = 0; index < dim; index++) {
		outputFile << " cp" << index << " = l*" << FirstPoint->getCoordiate(
				index) << " + (1-l)*" << SecondPoint->getCoordiate(index)
				<< " and ";
	}
	outputFile << " l >= 0 and l <= 1 and";

	for (index = 0; index < dim; index++) {
		outputFile << " unsafe" << index << " = ";
		for (unsafeSetIndex = 0; unsafeSetIndex < unsafeSet->getSize() - 1; unsafeSetIndex++) {
			outputFile << " Lus" << unsafeSetIndex << "*"
					<< unsafeSet->getPoint(unsafeSetIndex)->getCoordiate(index)
					<< " + ";
		}
		outputFile << " Lus" << unsafeSetIndex << "*" << unsafeSet->getPoint(
				unsafeSetIndex)->getCoordiate(index) << " and ";
	}
	for (unsafeSetIndex = 0; unsafeSetIndex < unsafeSet->getSize(); unsafeSetIndex++) {
		outputFile << " Lus" << unsafeSetIndex << " >= 0 and Lus"
				<< unsafeSetIndex << " <= 1 and ";
	}
	for (unsafeSetIndex = 0; unsafeSetIndex < unsafeSet->getSize() - 1; unsafeSetIndex++) {
		outputFile << " Lus" << unsafeSetIndex << " + ";
	}
	outputFile << " Lus" << unsafeSetIndex << " = 1 and ";
	for (index = 0; index < dim; index++) {
		outputFile << " (cp" << index << " - unsafe" << index << ")**2 +";
	}
	outputFile << " 0 <= " << epsilon * epsilon << "))$ \n \n";
	outputFile << "rlqe Formula; \n \n";
	outputFile << "end;";
	outputFile.close();

}

double getEpsilon(double delta, int example) {

	double value;
	value = sqrt(1 + ((9.8*9.8)/12));

	return example*value*delta;

}

int sanityCheck(){

	ifstream inputFile("Voutputglpk", ifstream::in);
	char Test[25];

	inputFile >> Test[0];
	while(!inputFile.eof()){
		inputFile >> Test[0];
		if(Test[0] == 'O'){
			for(int i=1;i<25;i++)
				inputFile >> Test[i];
			Test[20] = '\0';
			if(!strcmp(Test,"OPTIMALSOLUTIONFOUND"))
				return 0;
		}
	}
	return 1;
}

int GsanityCheck(char* Ptr){

	ifstream inputFile(Ptr, ifstream::in);
	char Test[25];

	inputFile >> Test[0];
	while(!inputFile.eof()){
		inputFile >> Test[0];
		if(Test[0] == 'O'){
			for(int i=1;i<25;i++)
				inputFile >> Test[i];
			Test[20] = '\0';
			if(!strcmp(Test,"OPTIMALSOLUTIONFOUND"))
				return 0;
		}
	}
	return 1;
}

