//
// Created by robgrzel on 01.06.17.
//
#ifndef UTILS_STRUCTS_H
#define UTILS_STRUCTS_H


#include "../UtilSources/Math/math.h"
#include "../UtilSources/template_utils.h"
#include "OWaves.h"


//#define DEBUG
#define PRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)

#ifdef DEBUG
	#define DPRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)
#else
	#define DPRINTF( format, ... )
#endif





typedef struct AWaves_t {

	size_t sizeofWavesMpi = sizeof( AWaves_t ) - sizeof( AWavePuls_t* );
	size_t sizeofWaveNum  = sizeof( AWavePuls_t );

	const int myrank;
	const int numWaves;


	AWavePuls* w = nullptr;

	~AWaves_t( );

	AWaves_t( int myrank, int numWaves );

} AWaves;

typedef struct AElem_t {


	int    idElem = -1; //id of elem
	double z0; //position init
	double dotz0; //velocity init
	double d; //draught init
	double m;
	double h; //dz

} AElem;

typedef struct AElems_t {
	const size_t sizeof_1 = sizeof( AElems_t );
	const size_t sizeof_2 = sizeof( AElem_t );

	const int myrank;
	const int numElems;

	bool empty = true;

	AElem* e = nullptr;

	~AElems_t( );

	AElems_t( int myrank, int numElems );

} AElems;

typedef struct ACylinderFloating_t {

	int sizeof_1 = sizeof( ACylinderFloating_t );


	//shared between elements
	const int myrank;
	const int numElems;

	double Grav;
	double Rho;

	double coefDrag;
	double damp33;
	double coefMass;
	double coefAddmass;

	double depth;
	double vx, vy;
	double phi;

	double dt;
	double areaProjected;
	double areaWaterline;

	//for offshore structure (all elements as one thing)
	double massInit, massDisp;
	double draught;
	double height;

	double z0;
	double dz0;
	double diameter;

	//gen waves data
	double tmax;//tmax cant be number what n/4%2==1
	double fsamp;
	double Hs;
	double Tp;

	void gen( AWaves_t& W );

	void gen( AElems_t& E );

	ACylinderFloating_t( int myrank, int numElems );

	ACylinderFloating_t(
			int myrank, int numElems, double dt, double diameter,
			double draught, double height,
			double depth, double vx, double vy, double phi,
			double z0, double coefDrag, double damp33,
			double coefAddmass, double Grav, double Rho,
	        double tmax, double Hs, double Tp
	);

} ACylinderFloatingData;


#define MPI_PACK1 AWaves_t, AWave_t, ACylinderFloating_t, AElems_t, AElem_t


#endif //UTILS_STRUCTS_H
