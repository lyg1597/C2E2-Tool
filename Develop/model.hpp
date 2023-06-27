/**
 * \class Model
 * \author Matt Potok
 *
 * \brief The Model data structure contains all information pertinent of a particular
 * model.
 *
 */ 

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include "annotation.hpp"
#include "checker.hpp"
#include "linear-set.hpp"
#include "simulator.hpp"

class Model
{
    public:
        // FIXME need to initialize all the values in verifier to something
        /***** Con/destructors *****/
        Model();
        ~Model();
        Model(int testmodel);

        /***** Functions *****/
        void print_model();
        int initialize();
        int simulate();
        int verify();

        int simulate_verify();

        // FIXME possibly move these function over to simulator.cpp
        int hybridSimulationCover(Simulator &simulator, Checker &checker, 
                LinearSet &unsafeSet, LinearSet &initialSet, int dimensions, 
                int mode, std::string file);
        int hybridSimulation(Simulator &simulator, Checker &checker, 
                LinearSet &unsafeSet, int dimensions, Point *origin, int mode, 
                std::string file);
        vector<Point*> getRepresentativeCover(Point *ptLower, Point *ptUpper, 
                int n, int dimensions);
        Point* getPointFromPoly(NNC_Polyhedron poly, int dimensions);

        /***** Interface variables *****/
        // NOTE Model exposes variables to simplify C++/Python interface
        // Alternative is to create getter/setter functions but it doesn't seem
        // to have much more benefit over public variables for a  wrapper class.
        // The exception are the vectors which can't be interfaces easily with
        // Python.
        unsigned int dimensions;
        unsigned int modes;
        unsigned int initial_mode;
        unsigned int initial_eqns;
        unsigned int unsafe_eqns;
        unsigned int annot_type;

        int refine;
        int simu_flag;

        std::vector<int> mode_linear;

        double abs_err;
        double rel_err;
        double delta_time;
        double end_time;

        std::vector<double> gammas;
        std::vector<double> k_consts;
        std::vector<double> initial_matrix;
        std::vector<double> initial_b;
        std::vector<double> unsafe_matrix;
        std::vector<double> unsafe_b;

        //std::string simulator;
        std::string annot_str;
        std::string beta_str;
        std::string opt_str;
        std::string visualize_filename;

    private:
        /***** Functions *****/
        template <typename T> void print_vector(std::string str, 
                std::vector<T> vec)
        {
            std::cout << str;
            for (auto v: vec)
            {
                std::cout << v << " ";
            }
            std::cout << std::endl;
        }

        /***** Variables *****/
        Annotation annotation;
        Simulator simulator;
        Checker checker;

        LinearSet initial_set;
        LinearSet unsafe_set;
};

#endif /* MODEL_H */
