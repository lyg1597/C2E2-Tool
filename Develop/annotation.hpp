/*
 * Annotation.h
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#ifndef ANNOTATION_H_
#define ANNOTATION_H_

#include <vector>

class Annotation {
private:
	int type;
	char* Annot;
	char* Beta;

    /*
	double* kConstArray;
	double* gammaArray;
	double kConst;
	double gamma;
    */

    std::vector<double> k_consts;
    std::vector<double> gammas;

public:
	Annotation();
	virtual ~Annotation();
	void setType(int value);
	int getType();
	void setAnnotation(char* string);
	char* getAnnotation();
	void setBeta(char* string);
	char* getBeta();

    /* TODO modify these
	void setKConst(double value);
	double getKConst();
	double getKVal(int mode);
	void setKConst(double* array);

	void setGamma(double value);
	double getGamma();
	double getGammaVal(int mode);
	void setGamma(double* array);
    */

    double getKVal(int mode);
    void setKConsts(std::vector<double> k);

    double getGammaVal(int mode);
    void setGamma(std::vector<double> g);


	//double getED(double delta, double t1, double t2);
	double getED(double delta, double t1, double t2, int mode);
};

#endif /* ANNOTATION_H_ */
