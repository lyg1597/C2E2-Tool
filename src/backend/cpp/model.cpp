#include <ctime>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <Python.h>
#include <cstddef>
#include <cstdio>

#include "model.hpp"
using namespace std;
/***** Constructors *****/
Model::Model() { }

/***** Destructors *****/
Model::~Model() { }

/***** Public functions *****/
// FIXME
void Model::print_model()
{
    std::cout << "Model parareters:" << std::endl 
              << "--------------------" << std::endl;

    // Unsigned integers
    std::cout << "dimensions: " << dimensions << std::endl
              << "modes: " << modes << std::endl
              << "initial_mode: " << initial_mode << std::endl
              << "initial_eqns: " << initial_eqns << std::endl
              << "unsafe_eqns: " << unsafe_eqns << std::endl
              << "annot_type: " << annot_type << std::endl;

    // Integers
    std::cout << "refine: " << refine << std::endl
              << "simu_flag: " << simu_flag << std::endl;

    // Integer vectors
    print_vector<int>("mode_linear: ", mode_linear);

    // Doubles
    std::cout << "absolute error: " << abs_err << std::endl
              << "relative error: " << rel_err << std::endl
              << "delta time: " << delta_time << std::endl
              << "end time: " << end_time << std::endl;
    
    // Doubles vectors
    print_vector<double>("gammas: ", gammas);
    print_vector<double>("k_consts: ", k_consts);
    print_vector<double>("initial_matrix: ", initial_matrix);
    print_vector<double>("initial_b: ", initial_b);
    print_vector<double>("unsafe_matrix: ", unsafe_matrix);
    print_vector<double>("unsafe_b: ", unsafe_b);

    // Strings
    std::cout << "annot_str: " << annot_str << std::endl
              << "beta_str: " << beta_str << std::endl
              << "opt_str: " << opt_str << std::endl
              << "visualize_filename: " << visualize_filename << std::endl;
}

int Model::initialize()
{
    // TODO don't need setAnnotation, setBeta
    // Annotation params
    annotation.setType(annot_type);
    annotation.setGamma(gammas); 
    annotation.setKConsts(k_consts);

    // TODO fix the executable
    // Simulator params
    simulator.setAbsError(abs_err);
    simulator.setRelError(rel_err);
    simulator.setTimeStep(delta_time);
    simulator.setGlobalTime(end_time);
    // FIXME DIR
    simulator.setExecutable("../work-dir/simu");

    // Initial set params
    initial_set.setDimensions(dimensions);
    initial_set.setNumEqns(initial_eqns);
    initial_set.setMatrix(initial_matrix);
    initial_set.setB(initial_b);

    // Unsafe set params
    unsafe_set.setDimensions(dimensions);
    unsafe_set.setNumEqns(unsafe_eqns);
    unsafe_set.setB(unsafe_b);
    unsafe_set.setMatrix(unsafe_matrix);

    Py_Initialize();

    return 0;
}

int Model::simulate_verify()
{
    // Timer
    std::time_t timer = std::time(NULL);
    //freopen("../work-dir/log.txt","w",stdout);
    // Initialize the model
    initialize();
    cout <<"printing the model"<<endl;
    //print_model();
    cout <<"finish priting the model"<<endl;


    //open the file to clear the data
    ofstream invFile;
	invFile.open(visualize_filename);
	invFile.close();
    

    

    // Variables
    int mode_simulated;
    int trace_safe_flag;
    int index_iter = 0;
    int num_sample_pnts = 0;
    int num_pnts_iter = 0;
    int num_refinements = 0;
    int num_unsafe_pnts = 0;
    int refine_threshold = 10;

    // TODO comment this block
    std::vector<double> delta_array(dimensions);
    for (int i = 0; i < dimensions; i++)
    {
        delta_array[i] = (initial_set.getMax(i) - initial_set.getMin(i)) / 2;
    }

    // TODO comment this block
    CoverStack *iter_stack = initial_set.getCoverStack(delta_array,
            initial_mode, 0);
    ReachTube *next_reach = new ReachTube();
    next_reach->setDimensions(dimensions);

    //
    // TODO comment this block
    std::vector<ReachTube*> result_tube;
    std::vector<ReachTube*> trace_tube;
    ReachTube *guard_set;
    Point *simulation_point;

    // TODO implement the custom refine order file

    // Initialize refinement dimensions
    std::vector<int> refine_flag(dimensions, 0);
    for (int i = 0; i < unsafe_eqns * dimensions; i++)
    {
        if (unsafe_matrix[i] != 0.0)
        {
            refine_flag[i % dimensions] = 1;
        }
    }

    std::vector<int> index_in_unsafe_set;
    for (int i = 0; i < dimensions; i++)
    {
        if (refine_flag[i])
        {
            index_in_unsafe_set.push_back(i);
        }
    }
   

    //return 0;
    // FIXME clean up the following code and create a do-while for combine the
    // simulation and verification into one function since they should be doing
    // similar things

    // FIXME move the hybridSimulationCover function to simulator.cpp
    // Simulate the system and terminate if simulation or counter-example

    
    int safe = hybridSimulationCover(simulator, checker, unsafe_set, initial_set,
            dimensions, initial_mode, visualize_filename);
   

    if (simu_flag || safe == -1)
    {
        return safe;
    }

	invFile.open(visualize_filename);
	invFile.close();

    
	// Open bloating library dynamically
    void *lib = dlopen("../work-dir/libbloatedsim.so", RTLD_LAZY);
    if(!lib){
    	cerr << "Cannot open library: " << dlerror() << '\n';
    }

    
	typedef vector<pair<NNC_Polyhedron, int> > (*guard_fn)(int, double *, double *);
	typedef bool (*inv_fn)(int, double *, double *);
	guard_fn guards = (guard_fn) dlsym (lib, "hitsGuard");
	inv_fn invs = (inv_fn) dlsym(lib, "invariantSatisfied"); 

    // Debugging
    cout << "Stack size: " << iter_stack->size() << endl;

	RepPoint* curItrRepPoint;


	FILE* tRFile;
	tRFile = fopen("../work-dir/refineorder.txt","r");
	vector<int> refineorder;
	int bufferReader;
	if(tRFile == NULL){
		cout<<"Refine order file does not exist, will refine by default method."<<endl;
		refine = 0;
	}
	else{
		cout<<"Custom refine order file detected, scanning file."<<endl;
		while( fscanf(tRFile,"%d",&bufferReader) != EOF ){
			if(bufferReader<=dimensions&&bufferReader>0){
				refineorder.push_back(bufferReader);
			}
		}
	}

	
	while(!iter_stack->empty()){
		num_sample_pnts++;
		curItrRepPoint = iter_stack->top();
		iter_stack->pop();

		//Crossed refine threshold, give up now.
		cout<<"Refine Time "<<curItrRepPoint->getRefineTime() <<endl;
		if(curItrRepPoint->getRefineTime()>refine_threshold){
            std::ofstream resultStream;
			resultStream.open("../work-dir/Result.dat");
			resultStream << "0" << endl;
			resultStream.close();

            return 0;
		}

        // Debugging
		//cout<<"========================POP 1 REP POINT, VERFICATION PROCESS START=================================="<<endl;
		curItrRepPoint->print();
	    //cout << "Current stack size: " << iter_stack->size() << endl;

		simulation_point = curItrRepPoint->getState();
		mode_simulated = curItrRepPoint->getMode();
		
		double* refDeltaArray = curItrRepPoint->getDeltaArray();

		//Step1. Simulation
        // FIXME already simulated the system above so can eliminate this (?)
		simulator.Simulate(simulation_point, mode_simulated);

		//Read simulation result
		class ReachTube* simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(mode_simulated);
		simulationTube->parseInvariantTube("../work-dir/SimuOutput", 0);


		int size_check = simulationTube->getSize();
        if (size_check){
            double *ptLower_check, *ptUpper_check;
            vector< pair<NNC_Polyhedron, int> > guards_hit_check;
            for(int i=0; i<size_check; i++){
                ptLower_check = simulationTube->getLowerBound(i)->getCoordinates();
                ptUpper_check = simulationTube->getUpperBound(i)->getCoordinates();
                //Print box corners.
                //cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
                //cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();
 
                guards_hit_check = guards(mode_simulated, ptLower_check, ptUpper_check);
                if(!guards_hit_check.empty()){
                    simulationTube->clear(i+15);
                    break;
                }
            }
            ofstream rewriteSimuOutput;
            rewriteSimuOutput.open("../work-dir/SimuOutput");
             
            for(int i=0; i<simulationTube->getSize(); i++){
                ptLower_check = simulationTube->getLowerBound(i)->getCoordinates();
                ptUpper_check = simulationTube->getUpperBound(i)->getCoordinates();
                //Print box corners.
                for (int j = 0; j < simulationTube->getLowerBound(i)->getDimension(); j++){
                    rewriteSimuOutput << simulationTube->getLowerBound(i)->getCoordinate(j);
                    rewriteSimuOutput << ' ';
                }
                rewriteSimuOutput << endl;
                for (int j = 0; j < simulationTube->getUpperBound(i)->getDimension(); j++){
                    rewriteSimuOutput << simulationTube->getUpperBound(i)->getCoordinate(j);
                    rewriteSimuOutput << ' ';
                }
                rewriteSimuOutput << endl;
            }
            rewriteSimuOutput.close();
        }
        //cout << "Finished check" << endl << endl << endl;
        // END
         
		// Step 2. Non-linear bloating
        // FIXME remove the input_buffs
        // FIXME migrate he Python code to C++ b/c this is inefficient

        char input_buff[128];
        char input_buff2[32];
        char input_buff3[32];
        char input_buff4[32];
        FILE *fid = NULL;
        const char *file_name = "../work-dir/ComputeLDF.py";
		if (mode_linear[mode_simulated-1]==0){
			double CT_step = k_consts[mode_simulated-1];
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
		    sprintf(input_buff4,"Is_linear = int(%d)", 
                    mode_linear[mode_simulated-1]);
		    PyRun_SimpleString(input_buff4);
		    fid = fopen(file_name, "r");
		    PyRun_SimpleFile(fid, file_name);
		    fclose(fid);
		    cout << input_buff <<endl;
		    cout << input_buff2 <<endl;
		    cout << input_buff3 <<endl;
		    cout << input_buff4 <<endl;
		}
		else{
		    ReachTube* bloatedTube;
			bloatedTube = simulationTube->bloatReachTube(refDeltaArray, 
                    &annotation);
			bloatedTube->printReachTube("../work-dir/reachtube.dat",0);
			delete bloatedTube;
		}
		delete simulationTube;



		//Step 3. Check invariant and guard
		simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(mode_simulated);
		simulationTube->parseInvariantTube("../work-dir/reachtube.dat", 1);

		guard_set = new ReachTube();
		guard_set->setDimensions(dimensions);

        bool hitGuard = false;
		int size = simulationTube->getSize();
        int refine_unsafe_flag = 0;
        int index_itr = 0;
        double *ptLower, *ptUpper;
        std::vector<int> refine_order;


        std::vector< pair<NNC_Polyhedron, int> > guards_hit;
        struct timeval inv_start, inv_end, guard_start, guard_end;
        for(int i=0; i<size; i++){
        		//Print box corners.
        		// cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
        		// cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();
                ptLower = simulationTube->getLowerBound(i)->getCoordinates();
                ptUpper = simulationTube->getUpperBound(i)->getCoordinates();
                
                // gettimeofday(&inv_start, NULL);
                bool inv_true = invs(mode_simulated, ptLower, ptUpper);
                // gettimeofday(&inv_end, NULL);
		    	// cout << "INVARIANT TIME: "<< inv_end.tv_usec - inv_start.tv_usec << endl;
                if(!inv_true){
                	/*cout << "INVARIANT NOT SATISFIED: " << i << endl;
	        		cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
    	    		cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();*/
                    simulationTube->clear(i);
                    cout << endl;
                    break;
                }

                // gettimeofday(&guard_start, NULL);
                guards_hit = guards(mode_simulated, ptLower, ptUpper);
                // gettimeofday(&guard_end, NULL);
            	// cout << "GUARD TIME: "<< guard_end.tv_usec - guard_start.tv_usec << endl;
                if(!guards_hit.empty()){
                	/*cout << "GUARD SATISFIED: " << i << endl;
	        		cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
   		    		cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();*/
                    guard_set->addGuards(guards_hit);
                    hitGuard = true;
                }
                else if(hitGuard==true){
                    simulationTube->clear(i);
                    break;
                }
                // cout << endl;
        }
		// guard_set->printGuards(); //Print all guards collected over reachtube.
      /*  cout << "ptLower: "; simulationTube->getLowerBound(0)->print();
        cout << "ptUpper: "; simulationTube->getUpperBound(1)->print();
*/
		trace_safe_flag = checker.check(simulationTube, &unsafe_set);
		if(trace_safe_flag == 0){
			//Tube unknown, trace to the origin and refine immedately.
			cout << " Tube unknown! Start to refine\n";
			trace_tube.clear();
			double* originDeltaArray;
			if (curItrRepPoint->getParentState()!=NULL)
				originDeltaArray = curItrRepPoint->getParentDeltaArray();
			else
				originDeltaArray = curItrRepPoint->getDeltaArray();
			
			if (refine == 0){
				cout<<"Using default refinement strategy."<<endl;
				if (refine_unsafe_flag<4)
				{
					//refine the dimension in unsafe equation 2 times
					iter_stack->refine(curItrRepPoint, 
                            index_in_unsafe_set.at(index_itr%index_in_unsafe_set.size()));
					index_itr++;
					refine_unsafe_flag++;
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
					iter_stack->refine(curItrRepPoint,refineidx);
					refine_unsafe_flag = 0;
				}
			}
			else{
				cout<<"Using custom refinement strategy."<<endl;
				int refineidx = curItrRepPoint->getRefineTime();
				iter_stack->refine(curItrRepPoint,refine_order[refineidx%refine_order.size()]-1);
			}
			delete curItrRepPoint;
		}
		else if(trace_safe_flag == 1){
			cout<< "Tube Safe! Checking if there are transitions for next mode.\n";
			trace_tube.push_back(simulationTube);
			int ifnextSet = guard_set->getNextSetStack(iter_stack, curItrRepPoint);
			if (!ifnextSet){
				cout << "No more transitions" << endl;
				result_tube.reserve(result_tube.size()+trace_tube.size());
				result_tube.insert(result_tube.end(),trace_tube.begin(),trace_tube.end());
				trace_tube.clear();
			}
			delete curItrRepPoint;
			delete guard_set;
		}
		else if(trace_safe_flag == -1){
			cout<<"Tube Unsafe, Break"<<endl;
			trace_tube.push_back(simulationTube);
			result_tube.reserve(result_tube.size()+trace_tube.size());
			result_tube.insert(result_tube.end(),trace_tube.begin(),trace_tube.end());
			trace_tube.clear();
			delete curItrRepPoint;
			class ReachTube* InvTube = NULL;
			int ResultTubeLength = 0;
			for(ResultTubeLength=0;ResultTubeLength<result_tube.size();ResultTubeLength++){
				InvTube = result_tube.at(ResultTubeLength);
				InvTube->printReachTube(visualize_filename,1);
				delete InvTube;
			}
			cout << " The system is unsafe \n";
			ofstream resultStream;
			resultStream.open("../work-dir/Result.dat");
			resultStream << "-1" << endl;
			resultStream.close();
			std::cout << "Execution time: "<< std::difftime(std::time(NULL), timer) << " s.\n";

            return -1;
		}
	}



	class ReachTube* InvTube = NULL;
	cout << "Size: " << result_tube.size() << endl;
	for(int i=0; i<result_tube.size();i++){
		InvTube = result_tube.at(i);
		InvTube->printReachTube(visualize_filename,1);
		delete InvTube;
	}
	cout << "System is safe" << endl;


	ofstream resultStream;
	resultStream.open("../work-dir/Result.dat");
	resultStream << "1" << endl;
	resultStream << num_sample_pnts << endl;
	resultStream << num_refinements << endl;
	resultStream.close();

	//Py_Finalize(); //finalize Python session
	//return 1;


	std::cout << "Execution time: "<< std::difftime(std::time(NULL), timer) << " s.\n";

    dlclose(lib);

    return 1;
}

int Model::hybridSimulationCover(Simulator &simulator, Checker &checker,
        LinearSet &unsafe_set, LinearSet &initialSet, int dimensions,
        int mode, std::string file){
	cout << "HYBRID SIMULATION" << endl;
	Point *ptLower = new Point(dimensions+1);
	Point *ptUpper = new Point(dimensions+1);
	for(int i=0; i<dimensions; i++){
		ptLower->setCoordinate(i+1, initialSet.getMin(i));
		ptUpper->setCoordinate(i+1, initialSet.getMax(i));
	}
	int isSafe = 1;

	vector<Point *> pts = getRepresentativeCover(ptLower, ptUpper, 3, dimensions);

	for(int i=0; i<pts.size(); i++){
		cout << "Hybrid Simulation " << i+1 << " -> ";
		pts[i]->print();
		isSafe = hybridSimulation(simulator, checker, unsafe_set, dimensions, pts[i], mode, file);
		if(isSafe==-1){
			cout << "Hybrid Simulation " << i+1 << " unsafe.\n" << endl;
			break;
		}
		else if(isSafe==1){
			cout << "Hybrid Simulation " << i+1 << " safe.\n" << endl;
		}
	}
	

 	ofstream resultStream;
	resultStream.open("../work-dir/Result.dat");
	resultStream << isSafe << endl;
	resultStream.close();

	return isSafe;
}


//Returns 1 if safe, -1 if unsafe
int Model::hybridSimulation(Simulator &simulator, Checker &checker,
        LinearSet &unsafe_set, int dimensions, Point *origin, int mode,
        std::string file){
	int isSafe = 1;
	int trace_safe_flag;

	//Try to open dynamic library.
	// FIXME DIR
    void *lib = dlopen("../work-dir/libhybridsim.so", RTLD_LAZY);
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
		simulator.Simulate(origin, mode);

		ReachTube* simulationTube = new ReachTube();
		simulationTube->setDimensions(dimensions);
		simulationTube->setMode(mode);
		// FIXME FILE PATH
		simulationTube->parseInvariantTube("../work-dir/SimuOutput", 0);
		int size = simulationTube->getSize();
		if (size == 0)
			break;

		
		double *ptLower, *ptUpper;

		
		vector< pair<NNC_Polyhedron, int> > guards_hit;
		for(int i=0; i<size; i++)
		{
			ptLower = simulationTube->getLowerBound(i)->getCoordinates();
			ptUpper = simulationTube->getUpperBound(i)->getCoordinates();
			//Print box corners.
			//cout << "ptLower: "; simulationTube->getLowerBound(i)->print();
			//cout << "ptUpper: "; simulationTube->getUpperBound(i)->print();


			guards_hit = guards(mode, ptLower, ptUpper);
			

			//cout << "guard at "<<i<< " result is " << guards_hit.empty() <<endl;
			if(!guards_hit.empty())
			{	
				pair<NNC_Polyhedron, int> guard_taken = guards_hit[rand() % guards_hit.size()];
				
				mode = guard_taken.second;
				origin = getPointFromPoly(guard_taken.first, dimensions);
				simulationTube->clear(i+1);
				break;
			}


			if(!invs(mode, ptLower, ptUpper))
			{
				simulationTube->clear(i);
				break;
			}
		}

		// Checks that the given trace is safe w.r.t the boxes given as unsafe set
		trace_safe_flag = checker.checkHybridSimulation(simulationTube, &unsafe_set);
		cout<<"checked"<<endl;
		if(trace_safe_flag==1){
	       	simulationTube->printReachTube(file,1);
		}
		else if(trace_safe_flag==-1){
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

Point* Model::getPointFromPoly(NNC_Polyhedron poly, int dimensions){
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


vector<Point*> Model::getRepresentativeCover(Point *ptLower, Point *ptUpper, int n, int dimensions){
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
