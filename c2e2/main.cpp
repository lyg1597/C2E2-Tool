/*
 * main.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: parasara
 */

#include "Vector.h"
#include "Point.h"
#include "Simulator.h"
#include "Annotation.h"
#include "Checker.h"
#include "InitialSet.h"

#include "Visualizer.h"
#include "ReachTube.h"
#include "LinearSet.h"

#include "RepPoint.h"
#include "CoverStack.h"

#include "simuverifScanner.h"
#include "simuverifParser.h"
#include "simuverif.tab.c"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <Python.h>
#include <dlfcn.h>

#include <stack>

#include <ppl.hh>

using namespace std;

int hybridSimulationCover(Simulator *simulator, Checker *checker, LinearSet *unsafeSet, LinearSet *initialSet, int dimensions, int mode, char *file);
int hybridSimulation(Simulator *simulator, Checker *checker, LinearSet *unsafeSet, int dimensions, Point *origin, int mode, char *file);
vector<Point *> getRepresentativeCover(Point *ptLower, Point *ptUpper, int n, int dimensions);
Point* getPointFromPoly(NNC_Polyhedron poly, int dimensions);

int main(int argc, char* argv[]) {
	//clock_t begin = clock();
	std::time_t start = std::time(NULL);

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

    // New object for simulator
    class Simulator* simVerify = new Simulator();
    simVerify->setAbsError(absoluteError); simVerify->setRelError(relativeError); simVerify->setExecutable(simuName);
    simVerify->setGlobalTime(globalTimeValue); simVerify->setTimeStep(timeStepValue);

    // Generating an object for annotation
    class Annotation* annotVerify = new Annotation();
    annotVerify->setAnnotation(annotationString); annotVerify->setBeta(betaString);
    annotVerify->setGamma(gammaValue); annotVerify->setKConst(kConstValue); annotVerify->setType(annotationTypeValue);
    annotVerify->setKConst(KConstArray); annotVerify->setGamma(gammaValueArray);

    class Checker* checkVerify = new Checker();

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
	}

	CoverStack* ItrStack = initialSet->getCoverStack(deltaArray,initMode,0);	
	ReachTube* nextReach = new ReachTube();
	nextReach->setDimensions(dimensions);

	std::vector<class ReachTube*> resultTube;
	std::vector<class ReachTube*> TraceTube;
	ReachTube* guardSet;
	Point* simulationPoint;

	/* Initialized, now just add this to the Iterator when it becomes NULL */
	int numberSamplePoints = 0;
	int modeSimulated;
	int traceSafeFlag;
	int numberOfPointsIterator = 0;
	int numRefinedPoints = 0;
	int numUnsafePoints = 0;
	FILE *fid = NULL;
	const char *filename = "ComputeLDF.py"; //python code filename
	char input_buff[128];
	char input_buff2[32];
	char input_buff3[32];
	char input_buff4[32];

    int refine_threshold = 10; //Number of times we refine before returning UNKNOWN.

	Py_Initialize(); //starting Python environment

	// read refine order file here
	FILE* tRFile;
	tRFile = fopen("refineorder.txt","r");
	vector<int> refineorder;
	int bufferReader;
	if(tRFile == NULL){
		cout<<"Refine order file does not exist, will refine by default method."<<endl;
		refine_control = 0;
	}
	else{
		cout<<"Custom refine order file detected, scanning file."<<endl;
		while( fscanf(tRFile,"%d",&bufferReader) != EOF ){
			if(bufferReader<=dimensions&&bufferReader>0){
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

	ofstream tmp;
	tmp.open(visuFileName);
	tmp << "hybrid simulation" << endl;
	tmp.close();

	int isSafe = hybridSimulationCover(simVerify, checkVerify, unsafeSet, initialSet, dimensions, initMode, visuFileName);
	//If it is a simulation or a counterexample has been detected, we are done.
	if(simulation_flag || isSafe==-1){
		ofstream resultStream;
		resultStream.open("Result.dat");
		resultStream << isSafe << endl;
		resultStream.close();
		exit(1);
	}

	tmp.open(visuFileName);
	tmp << "reachtube simulation" << endl;
	tmp.close();

	//Reset file.
	tmp.open(visuFileName);
	tmp.close();

	//Try to open dynamic library.
    void *lib = dlopen("./libbloatedsim.so", RTLD_LAZY);
    if(!lib){
    	cerr << "Cannot open library: " << dlerror() << '\n';
    }

	typedef vector<pair<NNC_Polyhedron, int> > (*guard_fn)(int, double *, double *);
	typedef bool (*inv_fn)(int, double *, double *);
	guard_fn guards = (guard_fn) dlsym (lib, "hitsGuard");
	inv_fn invs = (inv_fn) dlsym(lib, "invariantSatisfied"); 

    cout << "Stack size: " << ItrStack->size() << endl;

	RepPoint* curItrRepPoint;
	while(!ItrStack->empty()){
		numberSamplePoints++;
		curItrRepPoint = ItrStack->top();
		ItrStack->pop();

		//Crossed refine threshold, give up now.
		if(curItrRepPoint->getRefineTime()>refine_threshold){
			ofstream resultStream;
			resultStream.open("Result.dat");
			resultStream << "0" << endl;
			resultStream.close();
			exit(-1);
		}

		cout<<"========================POP 1 REP POINT, VERFICATION PROCESS START=================================="<<endl;
		curItrRepPoint->print();
	    cout << "Current stack size: " << ItrStack->size() << endl;

		simulationPoint = curItrRepPoint->getState();
		modeSimulated = curItrRepPoint->getMode();
		double* refDeltaArray = curItrRepPoint->getDeltaArray();

		//Step1. Simulation
		simVerify->Simulate(simulationPoint,modeSimulated);

		//Read simulation result
		class ReachTube* simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("SimuOutput", 0);

		//Step 2. Non-linear bloating
		if (linear_from_parser[modeSimulated-1]==0){
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
		    PyRun_SimpleString(input_buff);
		    sprintf(input_buff2,"CT_step = int(%f)", CT_step);
		    PyRun_SimpleString(input_buff2);
		    sprintf(input_buff3,"state = '%d'", modeforpython);
		    PyRun_SimpleString(input_buff3);
		    sprintf(input_buff4,"Is_linear = int(%d)",linear_from_parser[modeSimulated-1]);
		    PyRun_SimpleString(input_buff4);
		    fid = fopen(filename, "r");
		    PyRun_SimpleFile(fid, filename);
		    fclose(fid);
		}
		else{
		    class ReachTube* bloatedTube;
			bloatedTube = simulationTube->bloatReachTube(refDeltaArray,annotVerify);
			bloatedTube->printReachTube("reachtube.dat",0);
			delete bloatedTube;
		}
		delete simulationTube;

		//Step 3. Check invariant and guard
		simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(modeSimulated);
		simulationTube->parseInvariantTube("reachtube.dat", 1);

		guardSet = new ReachTube();
		guardSet->setDimensions(dimensions);

		int size = simulationTube->getSize();
        double *ptLower, *ptUpper;
        vector< pair<NNC_Polyhedron, int> > guards_hit;
        bool hitGuard = false;
        struct timeval inv_start, inv_end, guard_start, guard_end;
        for(int i=0; i<size; i++){
        		//Print box corners.
        		// cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
        		// cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();
                ptLower = simulationTube->getLowerBound(i)->getCoordinates();
                ptUpper = simulationTube->getUpperBound(i)->getCoordinates();
                
                // gettimeofday(&inv_start, NULL);
                bool inv_true = invs(modeSimulated, ptLower, ptUpper);
                // gettimeofday(&inv_end, NULL);
		    	// cout << "INVARIANT TIME: "<< inv_end.tv_usec - inv_start.tv_usec << endl;
                if(!inv_true){
                	cout << "INVARIANT NOT SATISFIED: " << i << endl;
	        		cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
    	    		cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();
                    simulationTube->clear(i);
                    cout << endl;
                    break;
                }

                // gettimeofday(&guard_start, NULL);
                guards_hit = guards(modeSimulated, ptLower, ptUpper);
                // gettimeofday(&guard_end, NULL);
            	// cout << "GUARD TIME: "<< guard_end.tv_usec - guard_start.tv_usec << endl;
                if(!guards_hit.empty()){
                	cout << "GUARD SATISFIED: " << i << endl;
	        		cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
   		    		cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();
                    guardSet->addGuards(guards_hit);
                    hitGuard = true;
                }
                else if(hitGuard==true){
                    simulationTube->clear(i);
                    break;
                }
                // cout << endl;
        }
		// guardSet->printGuards(); //Print all guards collected over reachtube.

		traceSafeFlag = checkVerify->check(simulationTube, unsafeSet);
		if(traceSafeFlag == 0){
			//Tube unknown, trace to the origin and refine immedately.
			cout << " Tube unknown! Start to refine\n";
			TraceTube.clear();
			double* originDeltaArray;
			if (curItrRepPoint->getParentState()!=NULL)
				originDeltaArray = curItrRepPoint->getParentDeltaArray();
			else
				originDeltaArray = curItrRepPoint->getDeltaArray();
			
			if (refine_control==0){
				cout<<"Using default refinement strategy."<<endl;
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
					for(int i=0; i<dimensions;i++){
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
				cout<<"Using custom refinement strategy."<<endl;
				int refineidx = curItrRepPoint->getRefineTime();
				ItrStack->refine(curItrRepPoint,refineorder[refineidx%refineorder.size()]-1);
			}
			delete curItrRepPoint;
		}
		else if(traceSafeFlag == 1){
			cout<< "Tube Safe! Checking if there are transitions for next mode.\n";
			TraceTube.push_back(simulationTube);
			int ifnextSet = guardSet->getNextSetStack(ItrStack,curItrRepPoint);
			if (!ifnextSet){
				cout << "No more transitions" << endl;
				resultTube.reserve(resultTube.size()+TraceTube.size());
				resultTube.insert(resultTube.end(),TraceTube.begin(),TraceTube.end());
				TraceTube.clear();
			}
			delete curItrRepPoint;
			delete guardSet;
		}
		else if(traceSafeFlag == -1){
			cout<<"Tube Unsafe, Break"<<endl;
			TraceTube.push_back(simulationTube);
			resultTube.reserve(resultTube.size()+TraceTube.size());
			resultTube.insert(resultTube.end(),TraceTube.begin(),TraceTube.end());
			TraceTube.clear();
			delete curItrRepPoint;
			class ReachTube* InvTube = NULL;
			int ResultTubeLength = 0;
			for(ResultTubeLength=0;ResultTubeLength<resultTube.size();ResultTubeLength++){
				InvTube = resultTube.at(ResultTubeLength);
				InvTube->printReachTube(visuFileName,1);
				delete InvTube;
			}
			cout << " The system is unsafe \n";
			ofstream resultStream;
			resultStream.open("Result.dat");
			resultStream << "-1" << endl;
			resultStream.close();
			std::cout << "Execution time: "<< std::difftime(std::time(NULL), start) << " s.\n";
			exit(-1);
		}
	}

	class ReachTube* InvTube = NULL;
	cout << "Size: " << resultTube.size() << endl;
	for(int i=0; i<resultTube.size();i++){
		InvTube = resultTube.at(i);
		InvTube->printReachTube(visuFileName,1);
		delete InvTube;
	}
	cout << "System is safe" << endl;

	ofstream resultStream;
	resultStream.open("Result.dat");
	resultStream << "1" << endl;
	resultStream << numberSamplePoints << endl;
	resultStream << numberRefinements << endl;
	resultStream.close();
	Py_Finalize(); //finalize Python session

	std::cout << "Execution time: "<< std::difftime(std::time(NULL), start) << " s.\n";

	exit(1);
}

int hybridSimulationCover(Simulator *simulator, Checker *checker, LinearSet *unsafeSet, LinearSet *initialSet, int dimensions, int mode, char *file){
	cout << "HYBRID SIMULATION" << endl;
	Point *ptLower = new Point(dimensions+1);
	Point *ptUpper = new Point(dimensions+1);
	for(int i=0; i<dimensions; i++){
		ptLower->setCoordinate(i+1, initialSet->getMin(i));
		ptUpper->setCoordinate(i+1, initialSet->getMax(i));
	}
	int isSafe = 1;

	vector<Point *> pts = getRepresentativeCover(ptLower, ptUpper, 3, dimensions);

	for(int i=0; i<pts.size(); i++){
		cout << "Hybrid Simulation " << i+1 << " -> ";
		pts[i]->print();
		isSafe = hybridSimulation(simulator, checker, unsafeSet, dimensions, pts[i], mode, file);
		if(isSafe==-1){
			cout << "Hybrid Simulation " << i+1 << " unsafe.\n" << endl;
			break;
		}
		else if(isSafe==1){
			cout << "Hybrid Simulation " << i+1 << " safe.\n" << endl;
		}
	}
	
	return isSafe;

 	ofstream resultStream;
	resultStream.open("Result.dat");
	resultStream << isSafe << endl;
	resultStream.close();

	exit(1);
}


//Returns 1 if safe, -1 if unsafe
int hybridSimulation(Simulator *simulator, Checker *checker, LinearSet *unsafeSet, int dimensions, Point *origin, int mode, char *file){
	int isSafe = 1;
	int traceSafeFlag;

	//Try to open dynamic library.
    void *lib = dlopen("./libhybridsim.so", RTLD_LAZY);
    if(!lib){
    	cerr << "Cannot open library: " << dlerror() << '\n';
    }

	typedef vector<pair<NNC_Polyhedron, int> > (*guard_fn)(int, double *, double *);
	typedef bool (*inv_fn)(int, double *, double *);
	guard_fn guards = (guard_fn) dlsym (lib, "hitsGuard");
	inv_fn invs = (inv_fn) dlsym(lib, "invariantSatisfied");
   
    while(true){
    	cout << "Simulating mode " << mode << " from ";
    	origin->print();
		simulator->Simulate(origin, mode);
		ReachTube* simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(mode);
		simulationTube->parseInvariantTube("SimuOutput", 0);
		int size = simulationTube->getSize();
		if (size == 0)
			break;
		double *ptLower, *ptUpper;
		vector< pair<NNC_Polyhedron, int> > guards_hit;
		for(int i=0; i<size; i++){
			ptLower = simulationTube->getLowerBound(i)->getCoordinates();
			ptUpper = simulationTube->getUpperBound(i)->getCoordinates();
			//Print box corners.
			cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
			cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();

			guards_hit = guards(mode, ptLower, ptUpper);
			if(!guards_hit.empty()){
				pair<NNC_Polyhedron, int> guard_taken = guards_hit[rand() % guards_hit.size()];
				mode = guard_taken.second;
				origin = getPointFromPoly(guard_taken.first, dimensions);
				simulationTube->clear(i+1);
				break;
			}
			if(!invs(mode, ptLower, ptUpper)){
				simulationTube->clear(i);
				break;
			}
		}

		// Checks that the given trace is safe w.r.t the boxes given as unsafe set
		traceSafeFlag = checker->checkHybridSimulation(simulationTube, unsafeSet);
		cout<<"checked"<<endl;
		if(traceSafeFlag==1){
	       	simulationTube->printReachTube(file,1);
		}
		else if(traceSafeFlag==-1){
	       	simulationTube->printReachTube(file,2);
	  		isSafe = -1;
			break;
		}
		else{
			cout << "<ERROR> UNKNOWN TUBE IN HYBRID SIMULATION" << endl;
		}

		cout<<"guard_size"<<guards_hit.size()<<endl;
		if(guards_hit.empty()){
			break;
		}
    }
	dlclose ( lib );
	cout<<"is Safe? "<<isSafe<<endl;
	return isSafe;
}

Point* getPointFromPoly(NNC_Polyhedron poly, int dimensions){
	// cout << "Get Point" << endl;
	Point *pt = new Point(dimensions+1);

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
		    	// cout << j << ": " << num << endl;
	    		pt->setCoordinate(j, num);
		  	}
		}
	}
	// cout << "Return Point" << endl;
	return pt;
}


vector<Point *> getRepresentativeCover(Point *ptLower, Point *ptUpper, int n, int dimensions){
	int thick_dims = 0;
	bool has_width;
	for(int i=1; i<=dimensions; i++){
		has_width = ptLower->getCoordinate(i)!=ptUpper->getCoordinate(i);
		if(has_width){
			thick_dims++;
		}
	}
	
	int num_pts = (int) pow(n, thick_dims);
	vector<Point *> pts(num_pts);
	for(int i=0; i<num_pts; i++){
		pts[i] = new Point(dimensions+1);
	}
	
	int block_size;
	double val, start, step_size;
	int num_thick_dim = 0;
	for(int i=1; i<=dimensions; i++){
		has_width = ptLower->getCoordinate(i)!=ptUpper->getCoordinate(i);
		if(!has_width){
			val = ptLower->getCoordinate(i);
			for(int j=0; j<num_pts; j++){
				pts[j]->setCoordinate(i, val);
			}
		}
		else{
			start = ptLower->getCoordinate(i);
			step_size = (ptUpper->getCoordinate(i)-ptLower->getCoordinate(i))/(n-1);
			block_size = (int) pow(n, num_thick_dim);
			for(int j=0, pt_i=0; j<num_pts; pt_i++){
				val = start + (pt_i%n)*step_size;
				for(int k=0; k<block_size; k++, j++){
					pts[j]->setCoordinate(i, val);
				}
			}
			num_thick_dim++;
		}
	}

	return pts;
}


