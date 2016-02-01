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


	/*  TODO - Visualizer file to be changed to ReachSet.dat */

	ofstream VisualizeFile;
	VisualizeFile.open(filename, ios::app);
	if(!VisualizeFile.is_open())
		return;
	class Point* Point1;
	class Point* Point2;
	double deltaVal;

//	class InitialSet* AllPointsAdded = new InitialSet();

	class InitialSet* Iterator = TemporaryPoints;

	if(dim1 != -1 && dim2 != -1){

		if(Iterator->getNext() == NULL && Iterator != NULL){
			Point1 = Iterator->getState();
			deltaVal = Iterator->getDelta();
			if(Point1 != NULL){
				if(dim2 == 0){

					VisualizeFile << " \n \n \n";

					VisualizeFile << Point1->getCoordiate(0) - 0.001 << " " << Point1->getCoordiate(dim1) - deltaVal;
					VisualizeFile << Point1->getCoordiate(0) - 0.001 << " " << Point1->getCoordiate(dim1) + deltaVal;
					VisualizeFile << Point1->getCoordiate(0) << " " << Point1->getCoordiate(dim1) + deltaVal;
					VisualizeFile << Point1->getCoordiate(0) << " " << Point1->getCoordiate(dim1) - deltaVal;
					VisualizeFile << Point1->getCoordiate(0) - 0.001 << " " << Point1->getCoordiate(dim1) - deltaVal;
				}else{

					VisualizeFile << " \n \n \n";

					VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal;
					VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal;
					VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal;
					VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal;
					VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal;

				}
			}
		}

		while(Iterator->getNext()!=NULL){
			Point1 = Iterator->getState();
			Iterator = Iterator->getNext();
			Point2 = Iterator->getState();
			deltaVal = Iterator->getDelta();

			if(dim2 == 0){

				VisualizeFile << " \n \n \n";

				double mindim1,maxdim1,mindim2,maxdim2;

				double v1, v2, v3, v4;
				v1 = Point1->getCoordiate(dim1) - deltaVal;
				v2 = Point2->getCoordiate(dim1) - deltaVal;
				v3 = Point1->getCoordiate(dim1) + deltaVal;
				v4 = Point2->getCoordiate(dim1) + deltaVal;

				mindim2 = v1 < v2 ? v1 : v2;
				maxdim2 = v3 > v4 ? v3 : v4;

				VisualizeFile << Point1->getCoordiate(dim2) << " " << mindim2 << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) << " " << maxdim2 << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) << " " << maxdim2 << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) << " " << mindim2 << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) << " " << mindim2 << "\n";

			}
			else{

				VisualizeFile << " \n \n \n";

				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";


				// ----------------------- //

				VisualizeFile << " \n \n \n";

				/*
				VisualizeFile << " \n \n \n";

				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) - deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) - deltaVal << " " << Point2->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";

				// ----------------------- //

				VisualizeFile << " \n \n \n";

				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point2->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) - deltaVal << " " << Point2->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";


				// ----------------------- //


				VisualizeFile << " \n \n \n";

				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) - deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) - deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";


				// ----------------------- //

				VisualizeFile << " \n \n \n";

				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point2->getCoordiate(dim1) - deltaVal << "\n";
				VisualizeFile << Point2->getCoordiate(dim2) + deltaVal << " " << Point2->getCoordiate(dim1) + deltaVal << "\n";
				VisualizeFile << Point1->getCoordiate(dim2) + deltaVal << " " << Point1->getCoordiate(dim1) + deltaVal << "\n";

				*/

			}
		}


	}

	if(dim1 == -1 && dim2 == -1){

		while(Iterator->getNext() != NULL){
			Point1 = Iterator->getState();
			Iterator = Iterator->getNext();
			Point2 = Iterator->getState();
			deltaVal = Iterator->getDelta();

			int dimensions = Point1->getDimension();

			int dimIndex;

			double value1, value2;

			for(dimIndex = 0; dimIndex < dimensions; dimIndex++){

				value1 = Point1->getCoordiate(dimIndex);
				value2 = Point2->getCoordiate(dimIndex);

				if(dimIndex == 0){

					if(value1 <= value2){
						VisualizeFile << " " << value1;
					}
					else{
						VisualizeFile << " " << value2;
					}
				}
				else{
					if(value1 <= value2){
						VisualizeFile << " " << value1 - deltaVal;
					}
					else{
						VisualizeFile << " " << value2 - deltaVal;
					}
				}
			}
			VisualizeFile << " \n";

			for(dimIndex = 0; dimIndex < dimensions; dimIndex++){

				value1 = Point1->getCoordiate(dimIndex);
				value2 = Point2->getCoordiate(dimIndex);

				if(dimIndex == 0){

					if(value1 <= value2){
						VisualizeFile << " " << value2;
					}
					else{
						VisualizeFile << " " << value1;
					}
				}
				else{
					if(value1 <= value2){
						VisualizeFile << " " << value2 + deltaVal;
					}
					else{
						VisualizeFile << " " << value1 + deltaVal;
					}
				}
			}
			VisualizeFile << " \n";


		}

	}


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
