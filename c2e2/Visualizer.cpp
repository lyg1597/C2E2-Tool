/*
 * Visualizer.cpp
 *
 *  Created on: Feb 3, 2013
 *      Author: parasara
 */

#include "Visualizer.h"
#include "InitialSet.h"
#include "Point.h"
#include <iostream>
#include <fstream>

using namespace std;

Visualizer::Visualizer() {
	// TODO Auto-generated constructor stub
	TemporaryPoints = NULL;
	VisualizePoints = new InitialSet();
	dim1 = -1; dim2 = -1;
	filename = NULL;
}

Visualizer::~Visualizer() {
	// TODO Auto-generated destructor stub
}

void Visualizer::setTempPoints(class InitialSet* temp){
	TemporaryPoints = temp;
}

class InitialSet* Visualizer::getTempPoints(){
	return TemporaryPoints;
}

void Visualizer::setVisualizePoints(class InitialSet* temp){
	VisualizePoints = temp;
}

class InitialSet* Visualizer::getVisualizePoints(){
	return VisualizePoints;
}

void Visualizer::setDim1(int value){
	dim1 = value;
}

int Visualizer::getDim1(){
	return dim1;
}

void Visualizer::setDim2(int value){
	dim2 = value;
}

int Visualizer::getDim2(){
	return dim2;
}

void Visualizer::setFileName(char* string){
	filename = string;
	ofstream ClearData;
	ClearData.open(filename);
	if(ClearData.is_open()){
		ClearData << " set terminal png \n";
		ClearData << " set output '" << filename << ".png'\n";
		ClearData << " set style line 1 linecolor rgb \"blue\"\n" << " set autoscale \n unset label \n set xtic auto \n set ytic auto \n";
		if(dim2 != 0){
			ClearData << " set xlabel \"x" << dim2 << "\"\n set ylabel \"x" << dim1 << "\"\n";
		}
		else{
			ClearData << " set xlabel \"time\"\n set ylabel \"x" << dim1 << "\"\n";
		}
		ClearData << " plot '-' notitle with lines ls 1\n";
	}
	ClearData.close();
}

char* Visualizer::getFileName(){
	return filename;
}

void Visualizer::rehashVisualPoints(){
	return;
}


void Visualizer::plotUnsafe(class Point* UnsafePoint1, class Point* UnsafePoint2, double GT){

	ofstream UnsafeFile;
	UnsafeFile.open("Unsafe.dat");

	if(!UnsafeFile.is_open())
		return;

	UnsafeFile << "\n\n\n";
	if(dim2 == 0){

		cout << " Coming to a place where its not supposed to come  \n";

		UnsafeFile << 0 << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";
		UnsafeFile << GT << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";
		UnsafeFile << GT << " " << UnsafePoint2->getCoordiate(dim1-1) << "\n";
		UnsafeFile << 0 << " " << UnsafePoint2->getCoordiate(dim1-1) << "\n";
		UnsafeFile << 0 << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";

	}
	else{

		UnsafeFile << UnsafePoint1->getCoordiate(dim2-1) << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";
		UnsafeFile << UnsafePoint1->getCoordiate(dim2-1) << " " << UnsafePoint2->getCoordiate(dim1-1) << "\n";
		UnsafeFile << UnsafePoint2->getCoordiate(dim2-1) << " " << UnsafePoint2->getCoordiate(dim1-1) << "\n";
		UnsafeFile << UnsafePoint2->getCoordiate(dim2-1) << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";
		UnsafeFile << UnsafePoint1->getCoordiate(dim2-1) << " " << UnsafePoint1->getCoordiate(dim1-1) << "\n";

	}

	UnsafeFile.close();


}


void Visualizer::Plot(){

	class InitialSet* PlotterSet;
	PlotterSet = VisualizePoints;

	ofstream VisualizeFile;
	VisualizeFile.open(filename);

	while(PlotterSet!=NULL){
		if(PlotterSet->getState() == NULL){
			PlotterSet = PlotterSet->getNext();
		}
		else{
			if(PlotterSet->getDelta() == 0){
				VisualizeFile << " \n \n \n";
				VisualizeFile << PlotterSet->getState()->getCoordiate(0) << "  " << PlotterSet->getState()->getCoordiate(1) << " \n";
				PlotterSet = PlotterSet->getNext();
			}
			else{
				VisualizeFile << PlotterSet->getState()->getCoordiate(0) << "  " << PlotterSet->getState()->getCoordiate(1) << " \n";
				PlotterSet = PlotterSet->getNext();
			}
		}

	}

	VisualizeFile.close();
}
