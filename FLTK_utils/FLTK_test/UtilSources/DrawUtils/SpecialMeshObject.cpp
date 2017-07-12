//
// Created by robgrzel on 23.04.17.
//

#include "SpecialMeshObject.h"

extern std::string AppResPath;


OSpecialMesh::OSpecialMesh( int n, int m, float size1, float size2, float xo, float yo, float zo ) {
	std::cout << "OSpecialMesh::OSpecialMesh( int n, int m, float size1, float size2 )\n";
	R1 = size1;//50.0;
	R2 = size2;//5.0;

	Xmax = size1;//50.0f;
	R    = size2;//5.0f;

	N = n;
	M = m;

	c_vertices = NULL;
	c_colors   = NULL;
	c_normals  = NULL;
	c_indices  = NULL;

	NPoints = 0;

	this->xo = xo;
	this->yo = yo;
	this->zo = zo;



}

OSpecialMesh::~OSpecialMesh( ) {
	delete[] c_vertices;
	delete[] c_colors;
	delete[] c_normals;
	delete[] c_indices;
}

inline void OSpecialMesh::GeneratePoint(
		eMeshes type, int n, int m, float& x, float& y, float& z,
		float& nx, float& ny, float& nz,
		float& red, float& green, float& blue, float& alpha
) {
	float dt = 0.0f;
	float dh = 0.0f;
	float zh = 0.0f;
	float du = 0.0f;

	switch ( type ) {
		case CYLINDER:
			//cylinder
			dt = 2 * M_PI / (N - 1);
			dh = Xmax / (M - 1);

			//x = m * dh;
			z = -m * dh;
			y = R * cos( n * dt );
			x = R * sin( n * dt );

			nz = 0;
			ny = cos( n * dt );
			nx = sin( n * dt );

			red   = 0.4 + 0.6 * z / Xmax;
			green = 0;
			blue  = 0;
			alpha = 1.0;
			break;

		case THORUS:
			//tor
			dt = M_PI / (N - 1);
			du = 2 * M_PI / (M - 1);

			//zh = 3;

			//x = (R1 + R2 * cos(m * du)) * cos(n * dt);
			//y = (R1 + R2 * cos(m * du)) * sin(n * dt);
			//z =  R2 * sin(m * du) + zh * (n * dt);

			z = (R1 + R2 * cos( m * du )) * cos( n * dt );
			x = (R1 + R2 * cos( m * du )) * sin( M_PI + n * dt );
			y = R2 * sin( m * du ) + zh * (n * dt);

			nx = cos( m * du ) * cos( n * dt );
			ny = cos( m * du ) * sin( n * dt );
			nz = sin( m * du );

			blue  = 0.5f + 0.5f * cos( n * dt );
			green = 0.5f + 0.5f * sin( n * dt );
			red   = (n + 0.0) / (N - 1.0);
			alpha = 1.0;
			break;

		case SPHERE:
			//sphere
			dt = M_PI / (N - 1);
			du = 2 * M_PI / (M - 1);

			x = R1 * cos( m * du ) * sin( n * dt );
			y = R1 * sin( m * du ) * sin( n * dt );
			z = R1 * cos( n * dt );

			nx = cos( m * du ) * sin( n * dt );
			ny = sin( m * du ) * sin( n * dt );
			nz = cos( n * dt );

			red   = 0.5 + 0.5 * cos( n * dt );
			green = 0.5 + 0.5 * sin( n * dt );
			blue  = (n + 0.0) / (N - 1.0);
			alpha = 1.0;
			break;
	}
}

void OSpecialMesh::operator +=( OVector3 ov3 ) {
	std::cout << "OSpecialMesh::operator+=( OVector3 ov3)\n";

	if (c_vertices) {
		//vertices generation
		float x = ov3.X();
		float y = ov3.Y();
		float z = ov3.Z();

		int       index = 0;//triangle index
		for ( int m     = 0; m < M - 1; m++ ) {
			for ( int n = 0; n < N - 1; n++ ) {
				//first triangle - 012
				translate_coordinates( index, x, y, z );
				//second triangle - 132
				index += 1;//triangle index
				translate_coordinates( index, x, y, z );
				index += 1;
			}
		}

		minmax_coordinates();
		centroid_coordinates();

	};


};

void OSpecialMesh::translate_centroid_coordinates( float xo, float yo, float zo ) {
	if (c_vertices) {

		minmax_coordinates();
		centroid_coordinates();

		this->xo = xo;
		this->yo = yo;
		this->zo = zo;
		float xoc = xo - xc;
		float yoc = yo - yc;
		float zoc = zo - zc;

		std::cout<<"minmax: zmin="<<zmin<<", zmax="<<zmax<<" -> zc="<<zc<<std::endl;
		std::cout<<"translate: from zc="<<zc<<" -> to zo="<<zo<<" -> by zoc="<<zoc<<std::endl;

		int       index = 0;
		for ( int m     = 0; m < M - 1; m++ ) {
			for ( int n = 0; n < N - 1; n++ ) {
				translate_coordinates( index, 0, 0, zoc );
				index++;
				translate_coordinates( index, 0, 0, zoc );
				index++;

			}
		}

		minmax_coordinates();
		centroid_coordinates();

		std::cout<<"new minmax: zmin="<<zmin<<", zmax="<<zmax<<" -> zc="<<zc<<std::endl;


	};
}

void OSpecialMesh::operator =( OVector3 ov3 ) {
	std::cout << "OSpecialMesh::operator+=( OVector3 ov3)\n";

	translate_centroid_coordinates( ov3.X(), ov3.Y(), ov3.Z() );


};

inline void OSpecialMesh::centroid_coordinates( ) {
	if (c_vertices) {
		minmax_coordinates();

		xc = xmax + xmin;
		yc = ymax + ymin;
		zc = zmax + zmin;

		xc /= 2.;
		yc /= 2.;
		zc /= 2.;
	}
}

inline void OSpecialMesh::minmax_coordinates( ) {
	std::cout << "OSpecialMesh::minmax_coordinates(0)\n";
	if (c_vertices) {
		std::cout << "OSpecialMesh::minmax_coordinates(1 : c_vertices)\n";
		float x11, x12, x13, x21, x22, x23;
		float y11, y12, y13, y21, y22, y23;
		float z11, z12, z13, z21, z22, z23;

		xmin = 0, ymin = 0, zmin = 0;
		xmax = 0, ymax = 0, zmax = 0;

		int       index = 0;//triangle index
		for ( int m     = 0; m < M - 1; m++ ) {
			for ( int n = 0; n < N - 1; n++ ) {
				//first triangle - 012

				x11 = c_vertices[9 * index];
				y11 = c_vertices[9 * index + 1];
				z11 = c_vertices[9 * index + 2];

				x12 = c_vertices[9 * index + 3];
				y12 = c_vertices[9 * index + 4];
				z12 = c_vertices[9 * index + 5];

				x13 = c_vertices[9 * index + 6];
				y13 = c_vertices[9 * index + 7];
				z13 = c_vertices[9 * index + 8];
				//second triangle - 132

				index += 1;//triangle index

				x21 = c_vertices[9 * index];
				y21 = c_vertices[9 * index + 1];
				z21 = c_vertices[9 * index + 2];

				x22 = c_vertices[9 * index + 3];
				y22 = c_vertices[9 * index + 4];
				z22 = c_vertices[9 * index + 5];

				x23 = c_vertices[9 * index + 6];
				y23 = c_vertices[9 * index + 7];
				z23 = c_vertices[9 * index + 8];


				index += 1;

				xmin = minimum( xmin, minimum( x11, x12, x13, x21, x22, x23 ) );
				ymin = minimum( ymin, minimum( y11, y12, y13, y21, y22, y23 ) );
				zmin = minimum( zmin, minimum( z11, z12, z13, z21, z22, z23 ) );

				xmax = maximum( xmax, maximum( x11, x12, x13, x21, x22, x23 ) );
				ymax = maximum( ymax, maximum( y11, y12, y13, y21, y22, y23 ) );
				zmax = maximum( zmax, maximum( z11, z12, z13, z21, z22, z23 ) );
			}
		}

		omin = OVector3( xmin, ymin, zmin );
		omax = OVector3( xmax, ymax, zmax );


	}



std::cout << "OSpecialMesh::minmax_coordinates(E)\n";

}


inline void OSpecialMesh::translate_coordinates(
		int index, float xt, float yt, float zt
) {
	c_vertices[9 * index] += xt;
	c_vertices[9 * index + 1] += yt;
	c_vertices[9 * index + 2] += zt;

	c_vertices[9 * index + 3] += xt;
	c_vertices[9 * index + 4] += yt;
	c_vertices[9 * index + 5] += zt;

	c_vertices[9 * index + 6] += xt;
	c_vertices[9 * index + 7] += yt;
	c_vertices[9 * index + 8] += zt;
}

inline void OSpecialMesh::AddCoordinates(
		int index, float x0, float y0, float z0,
		float x1, float y1, float z1,
		float x2, float y2, float z2
) {
	c_vertices[9 * index]     = x0;
	c_vertices[9 * index + 1] = y0;
	c_vertices[9 * index + 2] = z0;

	c_vertices[9 * index + 3] = x1;
	c_vertices[9 * index + 4] = y1;
	c_vertices[9 * index + 5] = z1;

	c_vertices[9 * index + 6] = x2;
	c_vertices[9 * index + 7] = y2;
	c_vertices[9 * index + 8] = z2;
}

inline void OSpecialMesh::AddNormals(
		int index, float nx0, float ny0, float nz0,
		float nx1, float ny1, float nz1,
		float nx2, float ny2, float nz2
) {
	c_normals[9 * index]     = nx0;
	c_normals[9 * index + 1] = ny0;
	c_normals[9 * index + 2] = nz0;

	c_normals[9 * index + 3] = nx1;
	c_normals[9 * index + 4] = ny1;
	c_normals[9 * index + 5] = nz1;

	c_normals[9 * index + 6] = nx2;
	c_normals[9 * index + 7] = ny2;
	c_normals[9 * index + 8] = nz2;
}

inline void OSpecialMesh::AddColors(
		int index, float red0, float green0, float blue0, float alpha0,
		float red1, float green1, float blue1, float alpha1,
		float red2, float green2, float blue2, float alpha2
) {
	c_colors[12 * index]     = red0;
	c_colors[12 * index + 1] = green0;
	c_colors[12 * index + 2] = blue0;
	c_colors[12 * index + 3] = alpha0;

	c_colors[12 * index + 4] = red1;
	c_colors[12 * index + 5] = green1;
	c_colors[12 * index + 6] = blue1;
	c_colors[12 * index + 7] = alpha1;

	c_colors[12 * index + 8]  = red2;
	c_colors[12 * index + 9]  = green2;
	c_colors[12 * index + 10] = blue2;
	c_colors[12 * index + 11] = alpha2;
}

void OSpecialMesh::WriteToFile( ) {
	std::cout << "OSpecialMesh::WriteToFile( )\n";
	std::string path = "Cylinder_.txt";
	FILE* f = fopen( path.c_str(), "w" );

	fprintf( f, "%s\n", "[CenterlineProx]" );
	fprintf( f, "%s\n", "[Vessel]" );

	int       nr_triangles = 2 * (N - 1) * (M - 1);
	for ( int i            = 0; i < 3 * nr_triangles; i++ ) {
		int index = c_indices[i];

		float x = c_vertices[3 * index];
		float y = c_vertices[3 * index + 1];
		float z = c_vertices[3 * index + 2];

		float nx = c_normals[3 * index];
		float ny = c_normals[3 * index + 1];
		float nz = c_normals[3 * index + 2];

		float red   = c_colors[4 * index];
		float green = c_colors[4 * index + 1];
		float blue  = c_colors[4 * index + 2];
		//float alpha = c_colors[4 * index + 3];

		int ired   = red * 255;
		int igreen = green * 255;
		int iblue  = blue * 255;
		int color  = (iblue >> 16) + (igreen >> 8) + ired;

		fprintf( f, "%f %f %f %f %f %f %d\n", x, y, z, nx, ny, nz, color );
	}

	fclose( f );
}

void OSpecialMesh::GenerateData( eMeshes type ) {
	std::cout << "OSpecialMesh::GenerateData(0)\n";
	std::cout << "OSpecialMesh::GenerateData(N=" << N << ", M=" << M << ")\n";
	int nr_triangles = 2 * (N - 1) * (M - 1);
	std::cout << "OSpecialMesh::GenerateData(nr_triangles=" << nr_triangles << ")\n";
	c_vertices = new float[3 * nr_triangles * 3];
	c_colors   = new float[3 * nr_triangles * 4];
	c_normals  = new float[3 * nr_triangles * 3];
	c_indices  = new unsigned int[3 * nr_triangles];
	std::cout << "OSpecialMesh::GenerateData(nr_vertices=" << 3 * nr_triangles * 3 << ")\n";
	float x0, y0, z0, nx0, ny0, nz0, red0, green0, blue0, alpha0;
	float x1, y1, z1, nx1, ny1, nz1, red1, green1, blue1, alpha1;
	float x2, y2, z2, nx2, ny2, nz2, red2, green2, blue2, alpha2;
	float x3, y3, z3, nx3, ny3, nz3, red3, green3, blue3, alpha3;

	//vertices generation
	int       index = 0;//triangle index
	for ( int m     = 0; m < M - 1; m++ ) {
		for ( int n = 0; n < N - 1; n++ ) {
			//0 point
			GeneratePoint( type, n, m, x0, y0, z0, nx0, ny0, nz0, red0, green0, blue0, alpha0 );

			//1 point
			GeneratePoint( type, n + 1, m, x1, y1, z1, nx1, ny1, nz1, red1, green1, blue1, alpha1 );

			//2 point
			GeneratePoint( type, n, m + 1, x2, y2, z2, nx2, ny2, nz2, red2, green2, blue2, alpha2 );

			//3 point
			GeneratePoint( type, n + 1, m + 1, x3, y3, z3, nx3, ny3, nz3, red3, green3, blue3, alpha3 );

			//first triangle - 012
			AddCoordinates( index, x0, y0, z0, x1, y1, z1, x2, y2, z2 );
			AddNormals( index, nx0, ny0, nz0, nx1, ny1, nz1, nx2, ny2, nz2 );
			AddColors( index, red0, green0, blue0, alpha0, red1, green1, blue1, alpha1,
			           red2, green2, blue2, alpha2
			         );

			//second triangle - 132
			index += 1;//triangle index
			AddCoordinates( index, x1, y1, z1, x3, y3, z3, x2, y2, z2 );
			AddNormals( index, nx1, ny1, nz1, nx3, ny3, nz3, nx2, ny2, nz2 );
			AddColors( index, red1, green1, blue1, alpha1, red3, green3, blue3, alpha3,
			           red2, green2, blue2, alpha2
			         );
			index += 1;
		}
	}

	//indices generation
	for ( int i = 0; i < 3 * nr_triangles; i++ ) {
		c_indices[i] = i;
	}
	NPoints = 3 * nr_triangles;

	WriteToFile();
}

void OSpecialMesh::GenerateDataFromFile( const char* file_name ) {
	std::cout << "OSpecialMesh::GenerateDataFromFile( const char* file_name )\n";
	std::fstream f( file_name, std::ios::in );
	if (!f.is_open()) {
		return;
	}

	std::vector<point3>       coordinates;
	std::vector<point3>       normals;
	std::vector<point4>       colors;
	std::vector<unsigned int> indices;

	coordinates.reserve( 65000 );
	normals.reserve( 65000 );
	colors.reserve( 65000 );
	indices.reserve( 65000 );

	std::string        line;
	std::istringstream linestream;
	point3             coord;
	point3             nor;
	point4             col;
	unsigned int       icol;
	bool               vessel_found = false;
	while ( !f.eof() ) {
		std::getline( f, line );
		if (line != "[Vessel]" && !vessel_found) {
			continue;
		} else if (!vessel_found) {
			vessel_found = true;
			continue;
		}
		linestream.clear();
		linestream.str( line.c_str() );
		linestream >> coord.x >> coord.y >> coord.z >> nor.x >> nor.y >> nor.z >> icol;
		size_t i = 0;
		for ( i = 0; i < coordinates.size(); i++ ) {
			if (coord == coordinates[i]) {
				break;
			}
		}

		if (i == 0) {
			xmin = xmax = coord.x;
			ymin = ymax = coord.y;
			zmin = zmax = coord.z;
		}

		if (i == coordinates.size()) {

			xmin = xmin > coord.x ? coord.x : xmin;
			xmax = xmax < coord.x ? coord.x : xmax;

			ymin = ymin > coord.y ? coord.y : ymin;
			ymax = ymax < coord.y ? coord.y : ymax;

			zmin = zmin > coord.z ? coord.z : zmin;
			zmax = zmax < coord.z ? coord.z : zmax;

			coordinates.push_back( coord );
			normals.push_back( nor );
			col.a = 1.0;//(icol >> 24) & 0xff / 255.0;
			unsigned int blue = ((icol >> 16) & 0xff);
			col.b = 1.0 * blue / 255.0;
			unsigned int green = ((icol >> 8) & 0xff);
			col.g = 1.0 * green / 255.0;
			unsigned int red = ((icol) & 0xff);
			col.r = 1.0 * red / 255.0;
			colors.push_back( col );
		}
		indices.push_back( coordinates.size() - 1 );
	}
	f.close();

	int size = coordinates.size();
	c_vertices = new float[3 * size];
	c_normals  = new float[3 * size];
	c_colors   = new float[4 * size];
	c_indices  = new unsigned int[indices.size()];

	for ( int i = 0; i < size; i++ ) {

		point3 coord = coordinates[i];
		point3 nor   = normals[i];
		point4 col   = colors[i];

		c_vertices[3 * i]     = coord.x;
		c_vertices[3 * i + 1] = coord.y;
		c_vertices[3 * i + 2] = coord.z;

		c_normals[3 * i]     = nor.x;
		c_normals[3 * i + 1] = nor.y;
		c_normals[3 * i + 2] = nor.z;

		c_colors[4 * i]     = col.r;
		c_colors[4 * i + 1] = col.g;
		c_colors[4 * i + 2] = col.b;
		c_colors[4 * i + 3] = col.a;
	}

	for ( size_t i = 0; i < indices.size(); i++ ) {
		c_indices[i] = indices[i];
	}
	NPoints = indices.size();
	std::cout << "GenerateDataFromFile out " << NPoints << " points" << std::endl;
}

void OSpecialMesh::DrawMeshFromFile( const char* file_name ) {
	std::cout << "OSpecialMesh::DrawMeshFromFile( const char* file_name ) \n";
	//	static bool b = false;
	//	if(!b) {
	//		b = true;
	//		GenerateDataFromFile(file_name);
	//	}

	if (c_vertices == NULL) {
		GenerateDataFromFile( file_name );
	}

	//	OGL3DCamera& camera = APPMNGR.GetState()->GetCamera();
	//	OVector3 lookat((xmax + xmin) / 2.0, (ymax + ymin) / 2.0, (zmax + zmin) / 2.0);

	//	camera.LookAt(lookat, camera.GetRoll());

	glVertexPointer( 3, GL_FLOAT, 0, c_vertices );
	glColorPointer( 4, GL_FLOAT, 0, c_colors );
	glNormalPointer( GL_FLOAT, 0, c_normals );
	glDrawElements( GL_TRIANGLES, NPoints, GL_UNSIGNED_INT, c_indices );

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	VDraws->DrawBox( OVector3( xmin, ymin, zmin ), OVector3( xmax, ymax, zmax ) );
}

void OSpecialMesh::DrawMesh( eMeshes type ) {
	std::cout << "OSpecialMesh::DrawMesh( eMeshes type )\n";

	if (c_vertices == NULL) {
		GenerateData( type );
	}

	currentType = type;


	glVertexPointer( 3, GL_FLOAT, 0, c_vertices );
	glColorPointer( 4, GL_FLOAT, 0, c_colors );
	glNormalPointer( GL_FLOAT, 0, c_normals );
	glDrawElements( GL_TRIANGLES, NPoints, GL_UNSIGNED_INT, c_indices );

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );


	std::cout << "--->zmin=" << zmin << std::endl;
	std::cout << "--->zc=" << zc << std::endl;
	std::cout << "--->zmax=" << zmax << std::endl;

	translate_centroid_coordinates(xo,yo,zo);



}
//end class OSpecialMesh