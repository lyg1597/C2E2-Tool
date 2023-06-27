/*
 * generalized-star.cpp
 *
 *  Created on: October 19, 2018
 *      Author: 
 */

#include "generalized-star.hpp"

using namespace std;

gstar::gstar(){
    basis = vector<double> ();
    center = vector<double> ();
    mode = -1;
    dimension = 0;
}

gstar::gstar(RepPoint ptn){
    dimension=ptn.getDimension();
    mode = ptn.getMode();
    double* temp = ptn.getState()->getCoordinates;
    time_loc = *temp;
    for(int i=0;i<dimension;i++){
        center.push_back(temp[i+1]);
    }
    temp=ptn.getDeltaArray();
    for(int i=0;i<dimension;i++){
        basis.push_back(temp[i]);
    }

}