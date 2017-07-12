/*
 *  VariousObjects.h
 *  Dame
 *
 *  Created by Catalin Ciobirca on 3/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VARIOUSOBJECTS_H_
#define _VARIOUSOBJECTS_H_

#if WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#define VDraws OVariousObjects::GetInstance()

#include "../Math/Quaternion.h"

class OVariousObjects {
	protected:
		OVariousObjects( );

		~OVariousObjects( );

		GLfloat quadXY[12];
		GLfloat quadXZ[12];
		GLfloat quadYZ[12];

		GLfloat quad_norm[12];
		GLfloat cube_[24];
	public:
		static OVariousObjects* GetInstance( );

		void DrawCone( GLfloat height, GLfloat radius, GLint points );

		void DrawAxes( GLfloat size, GLfloat linewidth );

		void DrawVerticalXZQuad( GLfloat size_x, GLfloat size_z, float r = 0.25f, float g = 0.5f, float b = 0.9f, float a = 0.7f );

		void DrawVerticalYZQuad( GLfloat size_y, GLfloat size_z, float r = 0.25f, float g = 0.5f, float b = 0.9f, float a = 0.7f );

		void DrawHorizontalQuad( GLfloat size_x, GLfloat size_y, float r = 0.25f, float g = 0.5f, float b = 0.9f, float a = 0.7f );

		void draw_cube( );

		void DrawBox( OVector3 min, OVector3 max );

		void DrawHorizontalObject(
				GLfloat size_x, GLfloat size_y, GLfloat size_z,
				GLfloat dist, GLfloat angle
		);

		void DrawVector( GLfloat ox, GLfloat oy, GLfloat oz, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat length );

		//new functions
		void DrawPlaneEq( OVector3 n, float d ); //draws a quad, included in a plane with the normal "n"
		//and at distance "d" from the origin
		void DrawTriangle( OVector3 v0, OVector3 v1, OVector3 v2 );

		void DrawTriQuad( OVector3 v0, OVector3 v1, OVector3 v2, OVector3 v3 );
};

#endif
