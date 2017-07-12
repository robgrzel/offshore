#include <jni.h>
#include "TempCalcJava.h"
//
//###################################################################
//                   TempCalcJava.cpp
//###################################################################
//
//
//  Creator : Chris Anderson
//  (C) UCLA 1997  7/16/97
//
//
// To have this code call the FORTRAN versions; define __FORTRAN_BUILD__
// when compiling.
// Typically this is specified by -D __FORTRAN_BUILD__ as part of the C++
// compilation command (Unix) or as a project setting when working within
// an integrated development environment.
//
//###################################################################
//
#ifndef __FORTRAN_BUILD__
//
// Version which links to C++
//
#include "tempCalcRoutines.h"

JNIEXPORT void JNICALL Java_TempCalcJava_initializeTemperature
(JNIEnv *env, jobject, jdoubleArray Tarray, jint m, jint n, jdouble a, jdouble b,
jdouble c, jdouble d)
{
     jdouble* tPtr   = env->GetDoubleArrayElements(Tarray,0);

     initializeTemperature(tPtr, m, n, a, b, c, d);

     env->ReleaseDoubleArrayElements(Tarray, tPtr,0);
};

JNIEXPORT void JNICALL Java_TempCalcJava_evolveTemperature
(JNIEnv *env, jobject, jdoubleArray Tarray, jint m, jint n, jdouble a, jdouble b,
jdouble c, jdouble d, jdouble dt, jdouble diffusivity, jdoubleArray workArray)
{

     jdouble* tPtr   = env->GetDoubleArrayElements(Tarray,0);
     jdouble* wPtr = env->GetDoubleArrayElements(workArray,0);

     evolveTemperature(tPtr, m, n, a, b, c, d, dt, diffusivity, wPtr);

     env->ReleaseDoubleArrayElements(Tarray,    tPtr,0);
     env->ReleaseDoubleArrayElements(workArray, wPtr,0);
};

#endif

#ifdef __FORTRAN_BUILD__

extern "C" void inittemp_(double* tPtr, long*m, long*n, double*a, double*b,
double*c, double*d);

extern "C" void evtemp_(double* tPtr, long*m, long*n, double*a, double*b,
double*c, double*d,double* dt, double* diffusivity, double* wPtr);

JNIEXPORT void JNICALL Java_TempCalcJava_initializeTemperature
(JNIEnv *env, jobject, jdoubleArray Tarray, jint m, jint n, jdouble a, jdouble b,
jdouble c, jdouble d)
{
     jdouble* tPtr   = env->GetDoubleArrayElements(Tarray,0);


     inittemp_(tPtr, &m, &n, &a, &b, &c, &d);

     env->ReleaseDoubleArrayElements(Tarray, tPtr,0);
};

JNIEXPORT void JNICALL Java_TempCalcJava_evolveTemperature
(JNIEnv *env, jobject, jdoubleArray Tarray, jint m, jint n, jdouble a, jdouble b,
jdouble c, jdouble d, jdouble dt, jdouble diffusivity, jdoubleArray workArray)
{

     jdouble* tPtr   = env->GetDoubleArrayElements(Tarray,0);
     jdouble* wPtr = env->GetDoubleArrayElements(workArray,0);

     evtemp_(tPtr, &m, &n, &a, &b, &c, &d, &dt, &diffusivity, wPtr);

     env->ReleaseDoubleArrayElements(Tarray,    tPtr,0);
     env->ReleaseDoubleArrayElements(workArray, wPtr,0);
};
#endif

