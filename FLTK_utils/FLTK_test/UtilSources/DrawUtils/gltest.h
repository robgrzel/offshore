#pragma once
#ifdef _WIN32
	#include <windows.h>
#else
#endif
#define _USE_MATH_DEFINES

#include <iostream>

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix4x4.h"
#include "VariousObjects.h"


void draw_scal_axis( float radius, float length );

void draw_scal_axes( float length );

void draw_cube( );

void drawin_cube( float xt=0, float yt=0, float zt =0, float xr=0, float yr=0, float zr=0 );
void drawin_object( void (*func)(void), float xt=0, float yt=0, float zt =0, float xr=0, float yr=0, float zr=0 );

void draw_vec_axis( OVector3 origin, OVector3 direction, float length, bool arrow = true, float radius_f = 0.01f );

void draw_vec_axes( float length );
