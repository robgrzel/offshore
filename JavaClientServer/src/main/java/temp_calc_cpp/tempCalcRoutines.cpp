#include "tempCalcRoutines.h"
#include <math.h>
//
//###################################################################
//                   tempCalcRoutines.cpp
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997  7/16/97
//
//###################################################################
//
void initializeTemperature(double* Tarray, long m, long n, double a, double b,
double c, double d)
{
   double**  T = create2dArrayStructure(Tarray,m,n);

   double hx = (b-a)/double(m-1);
   double hy = (d-c)/double(n-1);

   int    i, j;
   double x, y;

   for(i = 0; i < m; i++)
   {
   x = a + double(i)*hx;
   for(j = 0; j < n; j++)
   {
   y = c + double(j)*hy;
   T[i][j] = sin(4.0*x*y);
   }}

   for(i = 0; i < m; i++)
   {
   T[i][0]   = 0.0;
   T[i][n-1] = 0.0;
   }

   for(j = 0; j < n; j++)
   {
   T[0][j]   = 0.0;
   T[m-1][j] = 0.0;
   }

   destroy2dArrayStructure(T);
}

void evolveTemperature(double* Tdata, long m, long n,
double a, double b, double c, double d, double dt, double diffusivity,
double* workArray)
{
//
// Evolve using Forward Euler timestepping
//
   double**  T    = create2dArrayStructure(Tdata,m,n);
   double**  W    = create2dArrayStructure(workArray,m,n);

   double hx = (b-a)/double(m-1);
   double hy = (d-c)/double(n-1);

   int    i,j;

   for(i = 1; i < m-1; i++)
   {
   for(j = 1; j < n-1; j++)
   {
   W[i][j] = diffusivity*dt*
   (((T[i+1][j] - 2.0*T[i][j] + T[i-1][j])/(hx*hx)) +
    ((T[i][j+1] - 2.0*T[i][j] + T[i][j-1])/(hy*hy)));
   }}

   for(i = 1; i < m-1; i++)
   {
   for(j = 1; j < n-1; j++)
   {
   T[i][j] += W[i][j];
   }}

   destroy2dArrayStructure(T);
   destroy2dArrayStructure(W);
}

double** create2dArrayStructure(double* data, long m, long n)
{
//
// This routine creates a 2D array structure for the
// one dimensional array pointed to by data.
//
   int i;
   double** A   = new double*[m];
   for(i = 0; i < m; i++){A[i] = &data[i*n];}
   return A;
}

void destroy2dArrayStructure(double** A)
{
   delete [] A;
}