//
// Created by robgrzel on 23.04.17.
//

#ifndef UTILS_SPECIALMESHOBJECT_H
#define UTILS_SPECIALMESHOBJECT_H

#pragma once
#ifdef _WIN32
	#include <windows.h>
#else
#endif
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix4x4.h"
#include "../Utils/math_utils.h"
#include "VariousObjects.h"


class OSpecialMesh {
	public:

		struct point3 {
			float x;
			float y;
			float z;

			bool operator ==( const point3& p ) {
				return x == p.x && y == p.y && z == p.z;
			}
		};

		struct point4 {
			float r;
			float g;
			float b;
			float a;

		};

		enum eMeshes {
			SPHERE,
			CYLINDER,
			THORUS
		};

		OSpecialMesh( int n, int m, float size1, float size2, float xo = 0, float y = 0, float zo = 0 );

		~OSpecialMesh( );

		void WriteToFile( );

		void GenerateData( eMeshes type );

		void DrawMeshFromFile( const char* file_name );

		void DrawMesh( eMeshes type );

		void operator +=( OVector3 ov3 );

		void operator =( OVector3 ov3 );

		OVector3 get_min( ) {
			return omin;
		}


		OVector3 get_max( ) {
			return omax;
		}

	protected:

		void translate_centroid_coordinates(float xo, float yo, float zo);

		inline void centroid_coordinates( );

		inline void minmax_coordinates( );

		inline void GeneratePoint(
				eMeshes type, int n, int m, float& x, float& y, float& z,
				float& nx, float& ny, float& nz,
				float& red, float& green, float& blue, float& alpha
		);

		inline void translate_coordinates(
				int index, float xt, float yt, float zt
		);


		inline void AddCoordinates(
				int index, float x0, float y0, float z0,
				float x1, float y1, float z1,
				float x2, float y2, float z2
		);

		inline void AddNormals(
				int index, float nx0, float ny0, float nz0,
				float nx1, float ny1, float nz1,
				float nx2, float ny2, float nz2
		);

		inline void AddColors(
				int index, float red0, float green0, float blue0, float alpha0,
				float red1, float green1, float blue1, float alpha1,
				float red2, float green2, float blue2, float alpha2
		);

		void GenerateDataFromFile( const char* file_name );

		eMeshes currentType;

		OVector3 omin = {0,0,0};
		OVector3 omax = {0,0,0};

		float xc = 0, yc = 0, zc = 0;
		float xo = 0, yo = 0, zo = 0;

		float xmin = 0, xmax = 0;
		float ymin = 0, ymax = 0;
		float zmin = 0, zmax = 0;


		float R1, R2, R;

		float Xmax;

		int N, M, NPoints;

		float       * c_vertices;
		float       * c_colors;
		float       * c_normals;
		unsigned int* c_indices;

};


#endif //UTILS_SPECIALMESHOBJECT_H
