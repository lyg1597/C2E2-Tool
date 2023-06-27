/*
 * generalized-star.hpp
 *
 *  Created on: October 19, 2018
 *      Author: 
 */

#ifndef GENERALIZEDSTAR_H_
#define GENERALIZEDSTAR_H_

#include <vector>
#include "rep-point.hpp"
#include "point.hpp"

class gstar{
    private:
        std::vector<double> basis;
        std::vector<double> center;
        int dimension;
        int mode;
        double time_loc;
    public:
        gstar();
        gstar(RepPoint ptn);
};

#endif /* INITIALSET_H_ */
