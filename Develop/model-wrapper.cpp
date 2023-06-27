/**
 * \author Matt Potok
 *
 * \brief Interface code between C++ and Python
 *
 * More details go here as code is filled up
 */

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "model.hpp"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(libc2e2)
{
    bp::class_<Model>("Model")
        /***** Functions *****/
        .def("print_model", &Model::print_model)
        .def("simulate_verify", &Model::simulate_verify)

        /***** Variables *****/
        // Unsigned integer
        .def_readwrite("dimensions", &Model::dimensions)
        .def_readwrite("modes", &Model::modes)
        .def_readwrite("initial_mode", &Model::initial_mode)
        .def_readwrite("initial_eqns", &Model::initial_eqns)
        .def_readwrite("unsafe_eqns", &Model::unsafe_eqns)
        .def_readwrite("annot_type", &Model::annot_type)

        // Integer
        .def_readwrite("refine", &Model::refine)
        .def_readwrite("simu_flag", &Model::simu_flag)

        // Integer vector
        .def_readwrite("mode_linear", &Model::mode_linear)

        // Double
        .def_readwrite("abs_err", &Model::abs_err)
        .def_readwrite("rel_err", &Model::rel_err)
        .def_readwrite("delta_time", &Model::delta_time)
        .def_readwrite("end_time", &Model::end_time)

        // Double vector
        .def_readwrite("gammas", &Model::gammas)
        .def_readwrite("k_consts", &Model::k_consts)
        .def_readwrite("initial_matrix", &Model::initial_matrix)
        .def_readwrite("initial_b", &Model::initial_b)
        .def_readwrite("unsafe_matrix", &Model::unsafe_matrix)
        .def_readwrite("unsafe_b", &Model::unsafe_b)

        // String
        .def_readwrite("annot_str", &Model::annot_str)
        .def_readwrite("beta_str", &Model::beta_str)
        .def_readwrite("opt_str", &Model::opt_str)
        .def_readwrite("visualize_filename", &Model::visualize_filename)
        ;

    /***** Vector classes *****/
    // Integer Vector
    bp::class_<std::vector<int>>("IntegerVector")
        .def(bp::vector_indexing_suite<std::vector<int>>())
        ;

    // Double Vector
    bp::class_<std::vector<double>>("DoubleVector")
        .def(bp::vector_indexing_suite<std::vector<double>>())
        ;
};
