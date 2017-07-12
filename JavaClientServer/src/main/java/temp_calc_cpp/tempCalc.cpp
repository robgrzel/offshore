#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <math.h>
#include "tempCalcRoutines.h"
//
//###################################################################
//                   tempCalc.cpp
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997 7/16/97
//
//###################################################################
//
int main()
{
//
// Set Problem parameters
//
   double diffusivity = 0.1;
   double a = 0.0; double b = 1.0;
   double c = 0.0; double d = 1.0;
//
// Set Runtime parameters
//
   long   m            = 20;
   long   n            = 20;
 	long   nSteps       = 100;
   long   nOut         = 10;
   double dt           = 0.01;
//
// Allocate space for solution and work arrays
//
   double* Tarray     = new double[m*n];
   double* workArray  = new double[m*n];
//
// Open output file
//
   ofstream Fout("tempOut.dat");

   initializeTemperature(Tarray,m,n,a,b,c,d);
   double time        = 0.0;

   int i;  int j;

   for(i = 1; i <= nSteps; i++)
   {
   evolveTemperature(Tarray,m,n,a,b,c,d,dt,diffusivity,workArray);
   time = time + dt;

   if((i%nOut)== 0)
   {
      cout << " Step " << i << endl;      // print out step to screen

      Fout << m <<  "  "  << n <<  endl;  // output to file tempOut.dat ...
      Fout << time << endl;
      for(j = 0; j < m*n; j++){Fout << Tarray[j] << endl;}
   }

   }

   delete [] Tarray;
   delete [] workArray;
   return 0;
}


