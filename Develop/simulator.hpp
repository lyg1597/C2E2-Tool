/*
 * Simulator.h
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <string>
#include <glpk.h>
#include <stack>

#include "aliases.hpp"
#include "reach-tube.hpp"

class Simulator 
{
  public:
    // Function pointers
    Trace (*GenerateTrace)(NewPoint &point, double dt, double te);

    // Functions
    Simulator();
    Simulator(double abs_err, double rel_err, double delta_time, 
        double end_time);
    Simulator(double abs_err, double rel_err, double delta_time, 
        double end_time, std::string lib_path);

    // TODO remove these
    // Old functions
    void setExecutable(char* name);
    char* getExecutable();
    void setAbsError(double value);
    double getAbsError();
    void setRelError(double value);
    double getRelError();
    void setTimeStep(double value);
    double getTimeStep();
    void setGlobalTime(double value);
    double getGlobalTime();
    void Simulate(class Point* Ref);
    void Simulate(class Point* Ref, int mode);
    

    // Simulink functions
    int InitializeSimulink(double sample_delta, double sample_error, 
        const NewPoint &point, const NewPoint &radii);
    Trace GeneratePointTrace(NewPoint &pnt);
    int ComputeDiscrepancies();
    ReachTube *GenerateBloatTube(Trace &c_t, NewPoint &c_r);
    ReachTube *ConvertTraceToBloat(Trace &c_t);

    // Stack functions
    void PushStack(NewPoint &c_p, NewPoint &c_r, int refinetime);
    void Refine(NewPoint &c_p, NewPoint &c_r, int refinetime, int refineidx);

    // CarSim functions
    int InitializeCarSim(std::vector<std::string> set_vars);
    Trace CarSimClosestTrace(std::vector<double> desire_val);
    bool IsStackEmpty();
    void PrintNextReachTube();
    int PopStack(NewPoint *c_p, NewPoint *c_r);
    int PopStackTrace(Trace *c_t);
    void RefineCarSimSet(NewPoint &curPoint, NewPoint &curPoint_radii);
    int CarSimCheck(ReachTube* bloated_tube);
    void LoadCarSimTrace(int index);

  private:
    // Old variables
    char* executable;
    double abs_err;
    double rel_err;
    double start_time;
    double delta_time;
    double end_time;

    // Simulink variables
    // NOTE these should probably be unsigned int or size_t
    int num_samples;
    int num_points;
    int num_dimensions;
    int sample_delta;
    int sample_error;

    // Path to external Simulink library
    std::string lib_path;

    // Trace variables
    NewPoint center_point;
    NewPoint center_radii; // Should be same dimension as point
    Trace center_trace;
    std::vector<Trace> discrepancies;
    std::vector<Trace> trace_collection;



    // Bloat variables
    std::vector<double> delta;

    // CarSim variables
    std::vector <std::string> carsimvarlist;
    std::vector <int> carsimvarpos;
    std::vector<std::vector<double>> carsimfilelist;
    int carsimlistlength;
    std::vector <std::string> carsim_simulation_varlist;

    // Stack variables
    std::stack <NewPoint> point_stack;
    std::stack <NewPoint> point_radii_stack;
    std::stack <Trace> trace_stack;
    std::stack <int> refinetime_stack;
};

#endif /* SIMULATOR_H_ */
