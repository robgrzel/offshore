//
// Created by robgrzel on 05.06.17.
//

#ifndef CUDA_TEST_STRUCTS_CUH
#define CUDA_TEST_STRUCTS_CUH

#include "../UtilSources/CUDA_DUMMY.h"
#include "structs.h"

typedef struct AResultSystemStep_t {

	int    myrank, elemId;
	double ddot_z;//
	double dot_z;//
	double z; //
	double theta;
	double eta;
	double u_z;
	double dot_u_z;
	double F_damp;
	double F_drag;
	double F_rest;
	double F_wave;
	double F_added2;
	double F;//

	__host__ __device__ AResultSystemStep_t( );


	__host__ __device__ AResultSystemStep_t(
			const double a, const double b, const double c, const double d, const double e, const double f,
			const double g, const double h, const double i, const double j, const double k,
			double l, double m
	);


	__host__ __device__ AResultSystemStep_t operator =( const AResultSystemStep_t& a );

	__host__ __device__ AResultSystemStep_t operator +( const AResultSystemStep_t& a );

	__host__ __device__ AResultSystemStep_t& operator +=( const AResultSystemStep_t& a );

	__host__ __device__ void calc(
			const AResultSystemStep_t& result, const AWavePuls_t& wave, const AElem_t& elem,
			const ACylinderFloating_t& cons, const int i
	);

	__host__ __device__ void set_initial( const double ddot_z_, const double dot_z_, const double z_, double F_ );

} AResultSystemStep;


typedef struct AResultSystem_t {

	size_t sizeofResultSystem     = sizeof( AResultSystem_t );
	size_t sizeofResultSystemMpi  = sizeof( AResultSystem_t ) - sizeof( AResultSystemStep_t* ) * 7;
	size_t sizeofResultSystemStep = sizeof( AResultSystemStep_t );

	cudaError_t err = cudaSuccess;

	const int threadsPerBlock = 256;
	int       blocksPerGrid;

	int myrank;
	int numElems;
	int numSteps;
	int numWaves;
	int size;

	double t0, t1, dt;

	AResultSystemStep_t** results          = nullptr;
	AResultSystemStep_t* resultsIntegrated = nullptr;
	AResultSystemStep_t* resultsBlocks     = nullptr;
	AResultSystemStep_t* resultsInitial    = nullptr;
	AResultSystemStep_t* d_resultsBlocks   = nullptr;
	AResultSystemStep_t* d_resultsInitial  = nullptr;
	AWavePuls_t        * d_wave            = nullptr;

	~AResultSystem_t( );

	AResultSystem_t( int myrank, int numElems, int numSteps, int numWaves, double t0, double t1 );


	void compute(
			const AWaves_t& wavesData, const ACylinderFloating_t& structData,
			const AElem_t& elemData, int myrank
	);

	void compute_cuda(
			const AWaves_t& wavesData, const ACylinderFloating_t& structData,
			const AElem_t& elemData, int myrank
	);


} AResultSystem;


__device__ void printElemDevice( int id, AResultSystemStep_t& res );

void printElemHost( AResultSystemStep_t& res );

__global__ void kernelPerstep(
		AResultSystemStep_t* d_result, const AWavePuls_t* d_wave,
		const AElem_t d_elem, const ACylinderFloating_t cons,
		const int i
);

void echoError( cudaError_t& err, const char* strs );

#define MPI_PACK AResultSystem_t, AResultSystemStep_t, AWaves_t, AWavePuls_t, AElems_t, AElem_t, ACylinderFloating_t


#endif //CUDA_TEST_STRUCTS_CUH
