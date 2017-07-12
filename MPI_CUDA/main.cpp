#pragma once

#include <stdio.h>
#include <mpi.h>


#include "../UtilSources/CUDA_DUMMY.h"
#include "structs.h"
#include "MpiSlave.h"
#include "MpiMaster.h"

#define DATA 0
#define RESULT 1
#define FINISH 2
#define DEBUG


#define PRINTF(format, ...) fprintf (stdout, format, ##__VA_ARGS__)

#ifdef DEBUG
#define DPRINTF(format, ...) fprintf (stdout, format, ##__VA_ARGS__)
#else
#define DPRINTF( format, ... )
#endif

#define CHECK_CU_RESULT(err, cufunc)                                     \
    if (err != CUDA_SUCCESS) {                                                                   \
        printf ("Error %d for CUDA Driver API function '%s'.\n",          \
                err, cufunc);                                             \
        exit(-1);                                                         \
}

char a[255];
#define CHECK_CU_ERROR(err, strs)                                     \
    if (err != cudaSuccess) {\
        strncpy( a, strs, 255 );\
        fprintf( stderr, "Failed to %s,errorCode %s", a, cudaGetErrorString( err ) );\
        exit( EXIT_FAILURE );\
    }\

int gpuNum = 0;
int myrank = -1;

typedef struct gpu_device {
	
	CUdevice dev = -1;
	int gpuCap[2] = {-1, -1};
	
};


int set_cuda(int myrank) {
	
	unsigned int Flags = 0;
	
	CHECK_CU_RESULT(cuInit(Flags), "check cuInit");
	CHECK_CU_RESULT(cuDeviceGetCount(&gpuNum), "check cuDeviceGetCount");
	
	gpu_device gpuDevices[gpuNum];
	
	for (int i = 0; i < gpuNum; i++) {
		gpuDevices[i].dev = i;
		
		CHECK_CU_RESULT(cuDeviceComputeCapability(
				&gpuDevices[i].gpuCap[0], &gpuDevices[i].gpuCap[1], i),
		                "check cuDeviceComputeCapability");
		
		printf("myrank : %d, gpu info: gpuNum=%d, gpuId=%d, gpuCap(%d, %d)\n",
		       myrank, gpuNum, gpuDevices[i].dev, gpuDevices[i].gpuCap[0], gpuDevices[i].gpuCap[1]);
		
	}
	
	if (gpuNum == 1)
		cudaSetDevice(0);
	else if (gpuNum == 2) {
		if (myrank % 2) { cudaSetDevice(0); }
		else { cudaSetDevice(1); }
	}
};


int main_mpi(int argc, char **argv) {
	
	
	double diameter = 2;
	double draught = 4;
	double height = 6;
	double depth = 1000;
	double vx = -1;
	double vy = 0;
	double phi = 0;
	double z0 = 1.;
	double coefDrag2 = 0.82;
	double damp33 = 1000;
	double coefAddmass = 0.2;
	double Grav = 9.81;
	double Rho = 1025.;
	double Hs = 3.0;
	double Tp = 10.;
	double t0 = 0;
	double t1 = 600;
	double dt = 0.05;
	double fmax_fmin = 3;
	
	int numSteps = int((t1 - t0) / dt);
	int numElems = int(3e1);
	int numWaves = int(fmax_fmin * t1);
	
	
	int myrank, proccount;
	
	
	MPI_Init(&argc, &argv);
	
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &proccount);
	
	//process_mem_usage( myrank );
	set_cuda(myrank);
	
	
	if (proccount < 0) {
		DPRINTF("Run with at least 2 processes\n");
		MPI_Finalize();
		return -1;
	}
	if (numElems < 0 * (proccount - 1)) {
		DPRINTF("More subranges needed\n");
		MPI_Finalize();
		return -1;
	}
	
	
	if (!myrank) {
		
		
		ACylinderFloating_t aSTRUCT(myrank, numElems, dt, diameter, draught, height, depth,
		                            vx, vy, phi, z0, coefDrag2, damp33, coefAddmass,
		                            Grav, Rho, t1, Hs, Tp
		);
		
		MpiMaster<MPI_PACK>(aSTRUCT, numWaves, numSteps, t0, t1, myrank, proccount);
		
		PRINTF("MASTER RETURNED...\n");
		
	} else {
		
		
		MpiSlave<MPI_PACK>(numElems, numWaves, numSteps, t0, t1, myrank, proccount);
		PRINTF("SLAVE (%d) RETURNED...\n", myrank);
		
	}
	
	MPI_Finalize();
	return 0;
	
}

int main(int argc, char **argv) {
	
	return main_mpi(argc, argv);
	
}