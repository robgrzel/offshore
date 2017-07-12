#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#define piover2		 1.5707963267948966f
#define piover3		 1.0471975511965977f
#define piover4		 0.7853981633974483f
#define piover6		 0.5235987755982988f
#define piover12	 0.2617993877991490f
#define piover180	 0.0174532925199432f
#define _180overpi	57.2957795130823208f

#define deg2rad(x)	((x) * piover180)
#define rad2deg(x)	((x) * _180overpi)

//Very fast sqrt
float SQRT(float number);

//Very fast 1/sqrt
float RSQRT(float number);


inline int maximum( int a, int b ) { return a>b ? a : b ; }
inline int maximum( int a, int b, int c ) { return maximum( maximum(a,b), c ) ; }
inline int maximum( int a, int b, int c, int d, int e, int f ) { return maximum( maximum(a,b,c), maximum(d,e,f) ) ; }

inline float maximum( float a, float b ) { return a>b ? a : b ; }
inline float maximum( float a, float b, float c ) { return maximum( maximum(a,b), c ) ; }
inline float maximum( float a, float b, float c, float d, float e, float f ) { return maximum( maximum(a,b,c), maximum(d,e,f) ) ; }

inline double maximum( double a, double b ) { return a>b ? a : b ; }
inline double maximum( double a, double b, double c ) { return maximum( maximum(a,b), c ) ; }
inline double maximum( double a, double b, double c, double d, double e, double f ) { return maximum( maximum(a,b,c), maximum(d,e,f) ) ; }

inline int minimum( int a, int b ) { return a<b ? a : b ; }
inline int minimum( int a, int b, int c ) { return minimum( minimum(a,b), c ) ; }
inline int minimum( int a, int b, int c, int d, int e, int f ) { return minimum( minimum(a,b,c), minimum(d,e,f) ) ; }

inline float minimum( float a, float b ) { return a<b ? a : b ; }
inline float minimum( float a, float b, float c ) { return minimum( minimum(a,b), c ) ; }
inline float minimum( float a, float b, float c, float d, float e, float f ) { return minimum( minimum(a,b,c), minimum(d,e,f) ) ; }

inline double minimum( double a, double b ) { return a<b ? a : b ; }
inline double minimum( double a, double b, double c ) { return minimum( minimum(a,b), c ) ; }
inline double minimum( double a, double b, double c, double d, double e, double f ) { return minimum( minimum(a,b,c), minimum(d,e,f) ) ; }

