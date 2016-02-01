/*
 * Annotation.h
 *
 *  Created on: Feb 1, 2013
 *      Author: parasara
 */

#ifndef ANNOTATION_H_
#define ANNOTATION_H_

class Annotation {
private:
	int type;
	char* Annot;
	char* Beta;
	double* kConstArray;
	double* gammaArray;
	double kConst;
	double gamma;

public:
	Annotation();
	virtual ~Annotation();
	void setType(int value);
	int getType();
	void setAnnotation(char* string);
	char* getAnnotation();
	void setBeta(char* string);
	char* getBeta();
	void setKConst(double value);
	double getKConst();
	double getKVal(int mode);
	void setKConst(double* array);
	void setGamma(double value);
	double getGamma();
	double getGammaVal(int mode);
	void setGamma(double* array);
	double getED(double delta, double t1, double t2);
	double getED(double delta, double t1, double t2, int mode);
};

#endif /* ANNOTATION_H_ */
