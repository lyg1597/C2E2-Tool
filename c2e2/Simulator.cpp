/*
 * Simulator.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#include "Simulator.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

Simulator::Simulator() {
	// TODO Auto-generated constructor stub
	executable = NULL;
	absError = -1;
	relError = -1;
	timeStep = -1;
	globalTime = -1;

}

Simulator::~Simulator() {
	// TODO Auto-generated destructor stub
}

void Simulator::setExecutable(char* name){
	executable = name;
}

char* Simulator::getExecutable(){
	return executable;
}

void Simulator::setAbsError(double value){
	absError = value;
}

double Simulator::getAbsError(){
	return absError;
}

void Simulator::setRelError(double value){
	relError = value;
}

double Simulator::getRelError(){
	return relError;
}

void Simulator::setTimeStep(double value){
	timeStep = value;
}

double Simulator::getTimeStep(){
	return timeStep;
}

void Simulator::setGlobalTime(double value){
	globalTime = value;
}

double Simulator::getGlobalTime(){
	return globalTime;
}

void Simulator::Simulate(class Point* Ref){

	ofstream outputFile;

	outputFile.open("Config");

	int i;
	for(i=0;i<Ref->getDimension();i++){
		outputFile << Ref->getCoordinate(i) << "\n";
	}

	outputFile << absError << "\n";
	outputFile << relError << "\n";
	outputFile << timeStep << "\n";
	outputFile << globalTime << "\n";
	outputFile.close();

	char* execCommand;
	execCommand = (char*)malloc(100*sizeof(char));
	strcpy(execCommand,"./");
	strcat(execCommand,executable);
	strcat(execCommand," < Config > SimuOutput");


	int a;
	a = system(execCommand);

}

void Simulator::Simulate(class Point* Ref, int mode){

	ofstream outputFile;

	outputFile.open("Config");

	int i;
	for(i=0;i<Ref->getDimension();i++){
		outputFile << Ref->getCoordinate(i) << "\n";
	}

	outputFile << absError << "\n";
	outputFile << relError << "\n";
	outputFile << timeStep << "\n";
	outputFile << globalTime << "\n";
	outputFile << mode << "\n";
	outputFile.close();

	char* execCommand;
	execCommand = (char*)malloc(100*sizeof(char));
	strcpy(execCommand,"./");
	strcat(execCommand,executable);
	strcat(execCommand," < Config > SimuOutput");


	int a;
	a = system(execCommand);

}
