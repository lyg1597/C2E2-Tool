/*
 * Annotation.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "annotation.hpp"

// FIXME remove this
using namespace std;

Annotation::Annotation() {
	type = -1;
	Annot = NULL;
	Beta = NULL;
	//kConst = -1;
	//gamma = -1;
	//kConstArray = NULL;
	//gammaArray = NULL;
}

Annotation::~Annotation() { }

void Annotation::setType(int value){
	type = value;
}

int Annotation::getType(){
	return type;
}

void Annotation::setAnnotation(char* string){
	Annot = string;
}

char* Annotation::getAnnotation(){
	return Annot;
}

void Annotation::setBeta(char* string){
	Beta = string;
}

char* Annotation::getBeta(){
	return Beta;
}

/* TODO remove this
void Annotation::setKConst(double value){
	kConst = value;
}

double Annotation::getKConst(){
	return kConst;
}

void Annotation::setKConst(double* array){
	kConstArray = array;
}

double Annotation::getKVal(int mode){
	return *(kConstArray+(mode-1));
}
*/

double Annotation::getKVal(int mode)
{
    return k_consts[mode-1];
}

void Annotation::setKConsts(std::vector<double> k)
{
    k_consts = k;
}

double Annotation::getGammaVal(int mode)
{
    return gammas[mode-1];
}

void Annotation::setGamma(std::vector<double> g)
{
    gammas = g;
}

/* TODO remove this
void Annotation::setGamma(double value){
	gamma = value;
}

double Annotation::getGamma(){
	return gamma;
}

double Annotation::getGammaVal(int mode){
	return *(gammaArray+(mode-1));
}

void Annotation::setGamma(double* array){
	gammaArray = array;
}

double Annotation::getED(double delta, double t1, double t2){
	double delta1;
	double delta2;
	double finRet;
	if(type == 1){
		delta1 = 1.1*kConst*gamma*t1*delta;
		delta2 = 1.1*kConst*gamma*t2*delta;
		if(delta1 > delta2){
			finRet = delta1;
		}
		else{
			finRet = delta2;
		}

		if(finRet < 0){
			finRet = -1*finRet;
		}
		if(finRet == 0){
			finRet = delta;
		}

		return finRet;
	}
	if(type ==2 || type == 3){
		delta1 = 1.1*kConst*exp(gamma*t1)*delta;
		delta2 = 1.1*kConst*exp(gamma*t2)*delta;

		if(delta1 > delta2){
			finRet = delta1;
		}
		else{
			finRet = delta2;
		}

		if(finRet < 0){
			finRet = -1*finRet;
		}
		if(finRet == 0){
			finRet = delta;
		}

		return finRet;

	}
	else{
		return 1.1*delta;
	}
}
*/

double Annotation::getED(double delta, double t1, double t2, int mode){
	double delta1;
	double delta2;
	double finRet;

	double k1,gamma1;
    k1 = k_consts[mode-1];
    gamma1 = gammas[mode-1];
	//k1 = *(kConstArray+(mode-1));
	//gamma1 = *(gammaArray+(mode-1));

	if(type == 1){
		delta1 = 1.1*k1*gamma1*t1*delta;
		delta2 = 1.1*k1*gamma1*t2*delta;
		if(delta1 > delta2){
			finRet = delta1;
		}
		else{
			finRet = delta2;
		}

		if(finRet < 0){
			finRet = -1*finRet;
		}
		if(finRet == 0){
			finRet = delta;
		}

		return finRet;
	}
	if(type ==2 || type == 3){
		delta1 = 1.1*k1*exp(gamma1*t1)*delta;
		delta2 = 1.1*k1*exp(gamma1*t2)*delta;

		if(delta1 > delta2){
			finRet = delta1;
		}
		else{
			finRet = delta2;
		}

		if(finRet < 0){
			finRet = -1*finRet;
		}
		if(finRet == 0){
			finRet = delta;
		}

		return finRet;

	}
	else{
		return 1.1*delta;
	}
}
