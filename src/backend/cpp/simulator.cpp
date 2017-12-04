/*
 * Simulator.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <sstream>

#include <cmath>

#include <cstdlib>  
#include <ctime>
#include <dlfcn.h>

#include <Python.h>

#include "simulator.hpp"
//using namespace std;
// FIXME create a new constructur that sets all the variables
Simulator::Simulator() {
  executable = NULL;
  abs_err = -1;
  rel_err = -1;
  start_time = 0;
  delta_time = -1;
  end_time = -1;
  carsimlistlength = -1;
}

// New constructor
// FIXME will need to fix the code in the main.cpp portion
Simulator::Simulator(double abs_err, double rel_err, double delta_time,
    double end_time) : 
    abs_err(abs_err),
    rel_err(rel_err),
    delta_time(delta_time),
    end_time(end_time),
    start_time(0),
    executable(NULL)
{
}

Simulator::Simulator(double abs_err, double rel_err, double delta_time,
    double end_time, std::string lib_path) :
    abs_err(abs_err),
    rel_err(rel_err),
    delta_time(delta_time),
    end_time(end_time),
    lib_path(lib_path),
    start_time(0),
    executable(NULL)
{ }

void Simulator::setExecutable(char* name){
  executable = name;
}

char* Simulator::getExecutable(){
  return executable;
}

void Simulator::setAbsError(double value){
  abs_err = value;
}

double Simulator::getAbsError(){
  return abs_err;
}

void Simulator::setRelError(double value){
  rel_err = value;
}

double Simulator::getRelError(){
  return rel_err;
}

void Simulator::setTimeStep(double value){
  delta_time = value;
}

double Simulator::getTimeStep(){
  return delta_time;
}

void Simulator::setGlobalTime(double value){
  end_time = value;
}

double Simulator::getGlobalTime(){
  return end_time;
}

void Simulator::Simulate(class Point* Ref){

  std::ofstream outputFile;

  outputFile.open("../work-dir/Config");

  int i;
  for(i=0;i<Ref->getDimension();i++){
    outputFile << Ref->getCoordinate(i) << "\n";
  }

  outputFile << abs_err << "\n";
  outputFile << rel_err << "\n";
  outputFile << delta_time << "\n";
  outputFile << end_time << "\n";
  outputFile.close();

  std::string exec_command = "./" + std::string(executable) + \
    " < ../work-dir/Config > ../work-dir/SimuOutput";
  system(exec_command.c_str());
}

void Simulator::Simulate(class Point* Ref, int mode){

  std::ofstream outputFile;
  outputFile.open("../work-dir/Config");

  int i;
  for(i=0;i<Ref->getDimension();i++){
    outputFile << Ref->getCoordinate(i) << "\n";
  }

  outputFile << abs_err << "\n";
  outputFile << rel_err << "\n";
  outputFile << delta_time << "\n";
  outputFile << end_time << "\n";
  outputFile << mode << "\n";
  outputFile.close();

  std::string exec_command = "./" + std::string(executable) + \
    " < ../work-dir/Config > ../work-dir/SimuOutput";
  system(exec_command.c_str());
}

// FIXME rename this to be InitializeSimulink or something of that sort
// FIXME need to include the path to the library
int Simulator::InitializeSimulink(double sample_delta, double sample_error, 
    const NewPoint &point, const NewPoint &radii)
{
  // Load library dynamically
  void *simulator = dlopen(lib_path.c_str(), RTLD_LAZY);
  if (simulator == NULL)
  {
    std::cerr << "Can't load Simulink library." << std::endl;
  }

  // Load GenerateTrace function
  GenerateTrace = (Trace (*) (NewPoint&, double, double)) 
    dlsym(simulator, "generate_trace");
  if (GenerateTrace == NULL)
  {
    std::cerr << "Can't load function generate_trace" << std::endl;
  }

  // Copy vectors
  center_point = point;
  center_radii = radii;

  // Assign variables
  sample_error = sample_error;
  sample_delta = sample_delta;

  // NOTE this was fixed to include the center trace so I don't have to deal with the + 1
  // FIXME
  //num_samples = (1 / _sample_error) * log(_sample_delta);
  num_samples = 11;
  //num_samples = 30;
  // Range of points is inclusive, hence the + 1
  num_points = (int)((end_time - start_time) / delta_time + 0.5) + 1;
  num_dimensions = (int)center_point.size();

  // Include all samples including center trace
  trace_collection.reserve(num_samples);

  // Simulate center trace
  trace_collection[0] = GenerateTrace(center_point, delta_time, end_time);

  // FIXME
  // Simulate random traces
  //srand(time(NULL));
  vector<NewPoint> rand_points(num_samples, 
      std::vector<double>(num_dimensions, 0.0));
  rand_points[0] = center_point;
  for (int nd = 1; nd < num_dimensions; nd++)
  {
    for (int ns = 1; ns < num_samples; ns++)
    {
      int refine;
      do
      {
        refine = 0;
        
        // Generates a double from -1 to 1
        double rand_double = ((rand() / (double)RAND_MAX) - 0.5) * 2.0;  
        rand_points[ns][nd] = center_point[nd] + center_radii[nd] * rand_double;

        // Check for closeness
        for (int cs = 0; cs < ns; cs++)
        {
          // FIXME make the difference be a constant number
          if (abs(rand_points[ns][nd] - rand_points[cs][nd]) < 1E-6)
          {
            refine = 1;
          }
        }
      } while (refine);
    }
  }

  // Simulate traces
  for (int ns = 1; ns < num_samples; ns++)
  {
    trace_collection[ns] = GenerateTrace(rand_points[ns], delta_time, end_time);
  }

  /*
  for (int ns = 1; ns < num_samples; ns++)
  {
    // Generate starting points for random traces
    for (int nd = 1; nd < num_dimensions; nd++)
    {
      // Generates a double from -1 to 1
      double rand_double = ((rand() / (double)RAND_MAX) - 0.5) * 2.0;  

      // Generate new point with specified radius
      rand_point[nd] = center_point[nd] + center_radii[nd] * rand_double;
    } 
    //Simulate traces
    trace_collection[ns] = GenerateTrace(rand_point, delta_time, end_time);
  }
  */

  return 0;
}

Trace Simulator::GeneratePointTrace(NewPoint &pnt)
{
  return GenerateTrace(pnt, delta_time, end_time);
}

/*
int Simulator::InitializeSimulinkTraces(const NewPoint &central_point)
{
  // Simulate center trace
  trace_collection[0] = GenerateTrace(center_point, delta_time, end_time);

  // FIXME
  // Simulate random traces
  //srand(time(NULL));
  NewPoint rand_point(num_dimensions, 0.0);
  for (int ns = 1; ns < num_samples; ns++)
  {
    // Generate starting points for random traces
    for (int nd = 1; nd < num_dimensions; nd++)
    {
      // Generates a double from -1 to 1
      double rand_double = ((rand() / (double)RAND_MAX) - 0.5) * 2.0;  

      // Generate new point with specified radius
      rand_point[nd] = center_point[nd] + center_radii[nd] * rand_double;
    } 
    //Simulate traces
    trace_collection[ns] = GenerateTrace(rand_point, delta_time, end_time);
  }

  return 0;
}
*/

bool Simulator::IsStackEmpty(){
  return point_stack.empty();
}

void Simulator::PrintNextReachTube(){
  NewPoint curPoint = point_stack.top();
  NewPoint curPoint_radii = point_radii_stack.top();
  Trace curTrace = trace_stack.top();
  return;
  
  //FIXME
  //Bloat the carsim trace here and print it to reachset.dat?

}

int Simulator::PopStack(NewPoint *c_p, NewPoint *c_r){
  *c_p = point_stack.top();
  point_stack.pop();

  *c_r = point_radii_stack.top();
  point_radii_stack.pop();

  int retval= refinetime_stack.top();
  refinetime_stack.pop();

  return retval;
}

int Simulator::PopStackTrace(Trace *c_t)
{
  *c_t = trace_stack.top();
  trace_stack.pop();

  // FIXME
  return 0;
}




void Simulator::RefineCarSimSet(NewPoint &curPoint, NewPoint &curPoint_radii){

  cout << "curPoint: ";
      for(int i=0; i < 9; i++){
        cout << curPoint[i] << " ";
      }
  cout << "\n";

  cout << "curPoint Radii: ";
      for(int i=0; i < 9; i++){
        cout << curPoint_radii[i] << " ";
      }
  cout << "\n";


  //CAR SIM REFINE
  std::vector<double> desire_val_1;
  std::vector<double> desire_val_2;
   //FIXME HACK WAY
  int d=0;
  if(curPoint_radii[4]>=curPoint_radii[5]){
    desire_val_1.push_back(curPoint[4]+curPoint_radii[4]/2);
    desire_val_1.push_back(curPoint[5]);

    desire_val_2.push_back(curPoint[4]-curPoint_radii[4]/2);
    desire_val_2.push_back(curPoint[5]);
    d = 5;
  }

  else{
    desire_val_1.push_back(curPoint[4]);
    desire_val_1.push_back(curPoint[5]+curPoint_radii[5]/2);

    desire_val_2.push_back(curPoint[4]);
    desire_val_2.push_back(curPoint[5]-curPoint_radii[5]/2);
    d = 4;
  }

  Trace RefineTrace1 = CarSimClosestTrace(desire_val_1);
  Trace RefineTrace2 = CarSimClosestTrace(desire_val_2);


  NewPoint realPoint1 = RefineTrace1[0];
  NewPoint realPoint2 = RefineTrace2[0];


  NewPoint realRadii1;
  NewPoint realRadii2;

  for(int i=0; i<realPoint1.size();i++){
    realRadii1.push_back(realPoint1[i]-curPoint[i]);
    realRadii2.push_back(curPoint[i]-realPoint2[i]);
  }

  if (d == 4){
    if (realRadii1[5]>realRadii2[5])
      realRadii2[5] = realRadii1[5];
    else
      realRadii1[5] = realRadii2[5];

  }

  if (d==5){
    if (realRadii1[4] > realRadii2[4])
      realRadii2[4] = realRadii1[4];
    else
      realRadii1[4] = realRadii2[4];
  }

  realRadii1[d] = curPoint_radii[d];
  realRadii2[d] = curPoint_radii[d];


  realRadii1[1] = realRadii1[d];
  realRadii2[1] = realRadii2[d];
 /* 
  cout << "realPoint1: ";
      for(int i=0; i < 9; i++){
        cout << realPoint1[i] << " ";
      }
  cout << "\n"; 

  cout << "Real Radii1: ";
      for(int i=0; i < 9; i++){
        cout << realRadii1[i] << " ";
      }
  cout << "\n";

  cout << "realPoint2: ";
      for(int i=0; i < 9; i++){
        cout << realPoint2[i] << " ";
      }
  cout << "\n"; 



  cout << "Real Radii2: ";
      for(int i=0; i < 9; i++){
        cout << realRadii2[i] << " ";
      }
  cout << "\n";*/

  if (realPoint2!=curPoint&&curPoint[4]-curPoint_radii[4]<realPoint2[4]&&curPoint[5]-curPoint_radii[5]<realPoint2[5])
  {
    point_stack.push(realPoint2);
    point_radii_stack.push(realRadii2);
    refinetime_stack.push(0);
    trace_stack.push(RefineTrace2);
  }

  if (realPoint1!=curPoint&&curPoint[4]+curPoint_radii[4]>realPoint1[4]&&curPoint[5]+curPoint_radii[5]>realPoint1[5])
  {
    point_stack.push(realPoint1);
    point_radii_stack.push(realRadii1);
    refinetime_stack.push(0);
    trace_stack.push(RefineTrace1);
  }

  

  
  return;
}


int Simulator::InitializeCarSim(vector<string> set_vars)
{

  //push the varlist that will change over time
  for(int i=0; i<set_vars.size();i++){
    carsimvarlist.push_back(set_vars[i]);
  }

  //load file list
  ifstream infile("../CarSimSimulation/filelist.txt");

  if (!infile.is_open()){
    cout<<"File list for carsim doesn't exist"<<endl;
    exit(-1);
  }

  //init some variables we use to parse file
  std::string s;
  std::vector<std::string> elems;
  std::string::size_type sz;
  string buf;
  
  //read first line of file and get variable position in the list
  std::getline(infile, s);
  stringstream ss(s);
  int counter = 0;
  while (ss >> buf){
    for(int i=0; i<carsimvarlist.size();i++)
      if (buf == carsimvarlist[i])
        carsimvarpos.push_back(counter);
    counter++;
  }

  //read rest of list and push them into list holder
  carsimfilelist.resize(counter,std::vector<double>(0));

  while(std::getline(infile, s)){
    elems.clear();
    stringstream ss(s);
    while (ss >> buf)
        elems.push_back(buf);
    
    for (int i=0; i<counter;i++){
       carsimfilelist[i].push_back(std::stod(elems[i],&sz));
    }
  }

  carsimlistlength = carsimfilelist[0].size();
 

  //Simulate center trace
  int centerfileindex = carsimlistlength/2;
  std::string centerfilename("simuoutput");
  centerfilename+= std::to_string(centerfileindex);
  centerfilename+=".txt";
  centerfilename = "../CarSimSimulation/"+centerfilename;

  //load center simulation file
  ifstream simufile(centerfilename);

  if (!simufile.is_open()){
    cout<<"Simufile list for carsim doesn't exist"<<endl;
    exit(-1);
  }
  //load variable list from simulation file
  std::getline(simufile, s);
  stringstream sms(s);
  while (sms >> buf){
     carsim_simulation_varlist.push_back(buf);
  }

  //load center point from simulation file
  std::getline(simufile, s);
  stringstream smns(s);
  double numbuf;
  
  while(smns>>numbuf){
      center_point.push_back(numbuf);

    if (smns.peek() == ',' || smns.peek() == ' ')
      smns.ignore();
    
  }



  //#FIXME hack way to find center radii 
  center_radii.push_back(0);
  center_radii.push_back(5);
  center_radii.push_back(0);
  center_radii.push_back(0);
  center_radii.push_back((carsimfilelist[1][carsimlistlength-1]-carsimfilelist[1][0])/2);
  center_radii.push_back((carsimfilelist[2][carsimlistlength-1]-carsimfilelist[2][0])/2);
  center_radii.push_back(0);
  center_radii.push_back(0);
  center_radii.push_back(0);


  //load center trace
  center_trace.push_back(center_point);

  NewPoint temp_point;
  while(std::getline(simufile, s)){
    temp_point.clear();
    stringstream ss(s);
    while (ss >> numbuf){
        temp_point.push_back(numbuf);
        if (ss.peek() == ',' || ss.peek() == ' ')
        ss.ignore();
    
    }
    center_trace.push_back(temp_point);
  }


  //Add first point,radii and trace to stack
  point_stack.push(center_point);
  point_radii_stack.push(center_radii);
  trace_stack.push(center_trace);
  num_points = center_trace.size();
  num_dimensions = center_point.size();
  trace_collection.push_back(center_trace);
  // FIXME move this to the top of the function and set it to 11 to include the center trace
  num_samples=25;
  //num_samples=10;
  //srand(time(NULL));
  LoadCarSimTrace(0);
  LoadCarSimTrace(20);
  LoadCarSimTrace(420);
  LoadCarSimTrace(440);
  //load random trace
  //FIXME this will lead to an error when pushing back
  //trace_collection.reserve(num_samples);
  Trace temp_trace;
  for(int i=0; i<num_samples; i++){
    int randomindex = rand() % carsimlistlength;  
    if(randomindex==0||randomindex==20||randomindex==220||randomindex==420||randomindex==440)
      randomindex+=1;
    LoadCarSimTrace(randomindex); 
  }
  refinetime_stack.push(0);
}




void Simulator::LoadCarSimTrace(int index){
  Trace temp_trace;
  std::string s;
  NewPoint temp_point;
  int randomindex = index;
  double numbuf;
  
  cout<<"randomindex is "<< randomindex <<endl;
  
  std::string filename("simuoutput");
  filename+= std::to_string(randomindex);
  filename+=".txt";
  filename = "../CarSimSimulation/"+filename;
  ifstream randsimufile(filename);

  if (!randsimufile.is_open()){
    cout<<"Simufile list for carsim doesn't exist"<<endl;
    exit(-1);
  }

  std::getline(randsimufile, s);

  while(std::getline(randsimufile, s)){
    temp_point.clear();
    stringstream ss(s);
    while (ss >> numbuf){
        temp_point.push_back(numbuf);
        if (ss.peek() == ',' || ss.peek() == ' ')
          ss.ignore();
    }
    temp_trace.push_back(temp_point);
  }
  trace_collection.push_back(temp_trace);
}


Trace Simulator::CarSimClosestTrace(std::vector<double> desire_val){
  Trace ret_trace;

  std::vector<int> closepos;
  double lowerval, upperval;
  std::vector<double>::iterator low,up;

  for(int i=0; i<desire_val.size(); i++){
    //find upper bound
    up=std::lower_bound(carsimfilelist[carsimvarpos[i]].begin(), carsimfilelist[carsimvarpos[i]].end(), desire_val[i]);
    upperval = *up;
    low = up-1;
    lowerval = *low;

    //check boundary condition
    if(up == carsimfilelist[carsimvarpos[i]].begin()){
        lowerval = *(up);
        low = up;
    }

    if(up == carsimfilelist[carsimvarpos[i]].end()){
        upperval = carsimfilelist[carsimvarpos[i]][carsimfilelist[carsimvarpos[i]].size()-1];
        up = low;
    }

    if(up==low){
      for(int j=0; j<carsimfilelist[carsimvarpos[i]].size();j++)
        if (carsimfilelist[carsimvarpos[i]][j]==upperval){
          closepos.push_back(j);
          break;
        }   
    }


    else{

      if(upperval-desire_val[i]<=desire_val[i]-lowerval){
        
        for(int j=0;j<carsimfilelist[carsimvarpos[i]].size();j++){
          if (carsimfilelist[carsimvarpos[i]][j]==upperval){
            closepos.push_back(j);
            break;
          }
        }
      }
            
      else{
        for(int j=0;j<carsimfilelist[carsimvarpos[i]].size();j++){
          if (carsimfilelist[carsimvarpos[i]][j]==lowerval){
            closepos.push_back(j);
            break;
          }
        }
      }


    }

  }


  int filepos = 0;
  for(int i=0; i<desire_val.size() ; i++)
    filepos += closepos[i];
 /* cout<<"filenum "<<carsimfilelist[0][filepos]<<endl;*/
  int fileidx = carsimfilelist[0][filepos];
  std::string s;
  double numbuf;
  NewPoint temp_point;

  std::string filename("simuoutput");
  filename+= std::to_string(fileidx);
  filename+=".txt";
  filename = "../CarSimSimulation/"+filename;

  /*cout<<filename<<endl;
*/
  ifstream simufile(filename);

  if (!simufile.is_open()){
    cout<<"Simufile list for carsim doesn't exist"<<endl;
    exit(-1);
  }

  //skip first line since we know var
  std::getline(simufile, s);
  //cout<<s<<endl;
  while(std::getline(simufile, s)){
    temp_point.clear();
    stringstream ss(s);
    while (ss >> numbuf){
        temp_point.push_back(numbuf);
        if (ss.peek() == ',' || ss.peek() == ' ')
          ss.ignore();
    }
    ret_trace.push_back(temp_point);
  }

  return ret_trace;

}

// Make this return an error in the future
int Simulator::ComputeDiscrepancies()
{
  // Python objects
  PyObject *py_disc_path, *py_sys_path;
  PyObject *py_name, *py_module, *py_dict;

  // Python functions
  PyObject *py_set_trace_data, *py_handle_disc;
  
  // Initialize Python stuff
  Py_Initialize();  

  // Set path to python code
  // FIXME  this path is relative to where executable is called
  py_disc_path = PyBytes_FromString("./src/");
  py_sys_path = PySys_GetObject((char *)"path");
  PyList_Append(py_sys_path, py_disc_path);

  // Build the name object
  // FIXME change the name of the program
  py_name = PyBytes_FromString((char *)"comp_disc");

  // Load the module object
  // NOTE: I should include the check in the actual code
  py_module = PyImport_Import(py_name);

  // Get dictionary borrowed reference
  py_dict = PyModule_GetDict(py_module);

  // Get function reference
  py_handle_disc = PyDict_GetItemString(py_dict, (char *)"handle_disc");
  py_set_trace_data = PyDict_GetItemString(py_dict, (char *)"set_trace_data");

  // Generate trace list for python
  PyObject *traces = PyList_New(0);
  for (int ns = 0; ns < num_samples; ns++)
  {
    for (int np = 0; np < num_points; np++)
    {
      for (int nd = 0; nd < num_dimensions; nd++)
      {
        double datum = trace_collection[ns][np][nd];
        PyList_Append(traces, PyFloat_FromDouble(datum));
      }
    }
  }

  // Call py_set_trace_data()
  if (py_set_trace_data && PyCallable_Check(py_set_trace_data))
  {
    // Create arguments to pass
    PyObject *py_args = Py_BuildValue("(Oiiidd)", traces, num_samples, 
        num_points, num_dimensions, delta_time, end_time);

    // Call Python function
    PyObject_CallObject(py_set_trace_data, py_args);
    PyErr_Print();
    
    // Decrease reference count
    Py_XDECREF(py_args);
  }
  
  // Decrease reference count
  Py_XDECREF(traces);

  // Call py_handle_disc()
  if (py_handle_disc && PyCallable_Check(py_handle_disc))
  {
    // Call Python function
    PyObject *py_res;
    py_res = PyObject_CallObject(py_handle_disc, NULL);
    PyErr_Print();

    // Parse returned data
    PyObject *py_list, *py_float;
    for (int nd = 0; nd < num_dimensions - 1; nd++)
    {
      vector<vector<double>> int_disc;
      py_list = PyList_GetItem(py_res, nd);
      int py_list_len = PyList_Size(py_list);

      // Parse interval and discrepancy data
      for (int p = 0; p < py_list_len; p += 4)
      {
        vector<double> inter;
        for (int i = 0; i < 4; i++)
        {
          py_float = PyList_GetItem(py_list, p + i);
          inter.push_back(PyFloat_AsDouble(py_float));
        }

        // Append single interval to dimension intervals
        int_disc.push_back(inter);
      }

      // Append dimension interval to discrepancies
      discrepancies.push_back(int_disc);
    }
    
    // Decrement reference counts
    Py_DECREF(py_res);
    Py_DECREF(py_list);
    Py_DECREF(py_float);
  }

  // Decrement refernce counts
  Py_DECREF(py_disc_path);
  Py_DECREF(py_sys_path);

  Py_DECREF(py_name);
  Py_DECREF(py_module);
  Py_DECREF(py_dict);

  Py_DECREF(py_set_trace_data);
  Py_DECREF(py_handle_disc);
   
  // FIXME will need to uncomment this one day
  // Finish Python interpretter
  //Py_Finalize();
  cout<<"discrepancy calculation finished"<<endl;

  return 0;
}

ReachTube *Simulator::GenerateBloatTube(Trace &c_t, NewPoint &c_r)
{
  // FIXME
  // SIMULINK Powertrain
  //int K_VAL = 2000;
  // CARSIM
  int K_VAL = 1;

  ReachTube *b_tube = new ReachTube();
  b_tube->setDimensions(num_dimensions);
  b_tube->setMode(0); // This is useless

  Point *cur_pnt = NULL, *prev_pnt = NULL;
  
  // Initialize bloat tube
  // Iterate over all points in the bloat tube
  for (int pi = 0; pi < num_points; pi++)
  {
    // Create new point
    cur_pnt = new Point(num_dimensions);
    cur_pnt->setCoordinate(0, c_t[pi][0]);

    // Add points
    if (prev_pnt != NULL)
    {
      b_tube->addLowerBoundState(prev_pnt);
      b_tube->addUpperBoundState(cur_pnt);
    }

    // Update prev point
    prev_pnt = new Point(*cur_pnt);
  }

  prev_pnt = NULL;
  cur_pnt = NULL;

  // Set the bloat tube values
  // Iterate over dimensions
  for (int nd = 1; nd < num_dimensions; nd++)
  {
    // Initialize first delta value
    double prev_delta = c_r[nd];

    // Iterate over all intervals in a dimension
    int pi = 0;
    for (size_t it = 0; it < discrepancies[nd - 1].size(); it++)
    {
      vector<double> i_d = discrepancies[nd - 1][it]; 
      
      // Iterate over singular interval
      int end = (int)((i_d[1] - i_d[0]) / delta_time + 0.5);
      for (int i = 0; i < end; i++, pi++)
      {
        // Get points
        Point *low_pnt = b_tube->getLowerBoundState(pi);
        Point *up_pnt = b_tube->getUpperBoundState(pi);

        // Compute delta for next point
        double cur_delta = prev_delta * std::exp(i_d[2] * delta_time);
        double max_delta = max(prev_delta, cur_delta);

        // Bloat points
        // FIXME
        double trace_pnt = c_t[pi][nd];
        low_pnt->setCoordinate(nd, trace_pnt - max_delta * i_d[3]);
        up_pnt->setCoordinate(nd, trace_pnt + max_delta * i_d[3]);
        //low_pnt->setCoordinate(nd, trace_pnt - max_delta * K_VAL);
        //up_pnt->setCoordinate(nd, trace_pnt + max_delta * K_VAL);

        // Update previous
        prev_delta = cur_delta;
      }
    }
  }
    
  // FIXME testing
  //char *file_name = "./out/bloat.txt";
  //b_tube->printReachTube(file_name, 0);

  return b_tube;
}

ReachTube *Simulator::ConvertTraceToBloat(Trace &c_t)
{
  ReachTube *b_tube = new ReachTube();
  b_tube->setDimensions(num_dimensions);
  b_tube->setMode(0); // This is useless

  Point *cur_pnt = NULL, *prev_pnt = NULL;
  
  // Initialize bloat tube
  // Iterate over all points in the bloat tube
  for (int pi = 0; pi < num_points; pi++)
  {
    // Create new point
    cur_pnt = new Point(num_dimensions);
    for (int nd = 0; nd < num_dimensions; nd++)
    {
      cur_pnt->setCoordinate(nd, c_t[pi][nd]);
    }

    // Add points
    if (prev_pnt != NULL)
    {
      b_tube->addLowerBoundState(prev_pnt);
      b_tube->addUpperBoundState(cur_pnt);
    }

    // Update prev point
    prev_pnt = new Point(*cur_pnt);
  }
  
  // FIXME testing
  std::string file_name = "./out/trace_bloat.txt";
  //char *file_name = "./out/trace_bloat.txt";
  b_tube->printReachTube(file_name, 0);
  
  return b_tube;
}

void Simulator::PushStack(NewPoint &c_p, NewPoint &c_r, int refinetime)
{
    point_stack.push(c_p);
    point_radii_stack.push(c_r);
    refinetime_stack.push(refinetime);
}

void Simulator::Refine(NewPoint &c_p, NewPoint &c_r, int refinetime, int refineidx)
{
  
  double max[num_dimensions];
  double min[num_dimensions];

  max[0] = c_p[0];
  min[0] = c_p[0];
  for(int i=1; i<num_dimensions; i++){
    max[i] = c_p[i] + c_r[i];
    min[i] = c_p[i] + c_r[i];
  }

  c_r[refineidx] = c_r[refineidx]/2;

  NewPoint pt1;
  NewPoint pt2;

  pt1.push_back(max[0]);
  pt2.push_back(min[0]);

  for(int i=1; i<num_dimensions; i++){
    pt1.push_back(max[i]-c_r[i]);
    pt2.push_back(min[i]+c_r[i]);

  }

  point_stack.push(pt2);
  point_radii_stack.push(c_r);
  refinetime_stack.push(refinetime+1);
  
  point_stack.push(pt1);
  point_radii_stack.push(c_r);
  refinetime_stack.push(refinetime+1);
  

}

int Simulator::CarSimCheck(ReachTube* bloated_tube){
    double unsafeval = 1;

    //double unsafeval = 4.0;
    int simulation_size = bloated_tube->getSize();
    double lwdis, updis;
    int unknow_flag = 0;

    for (int i = 0; i < simulation_size; i++){
       Point * upPt = bloated_tube->getUpperBoundState(i);
       Point * lwPt = bloated_tube->getLowerBoundState(i);
       lwdis = lwPt->getCoordinate(1);
       updis = upPt->getCoordinate(1);

       if (lwdis<=unsafeval && updis <=unsafeval){
        return -1;
       }

       if (lwdis<=unsafeval && updis > unsafeval){
        unknow_flag = 1;
       }

    }


    if (unknow_flag == 1)
      return 0;

    return 1;
}
