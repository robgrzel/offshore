#ifndef  __TEMPCALC__
#define  __TEMPCALC__
//
//###################################################################
//                   tempCalcRoutines.h
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997  7/16/97
//
//###################################################################
//

void initializeTemperature(double* Tarray, long m, long n,
double a, double b, double c, double d);

void evolveTemperature(double* Tarray, long m, long n, double a, double b,
double c, double d, double dt, double diffusivity, double* workArray);

double** create2dArrayStructure(double* data, long m, long n);

void     destroy2dArrayStructure(double** A);

#endif
