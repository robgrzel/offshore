//
// Created by robgrzel on 04.06.17.
//
#include "structs.cuh"
#include "OElemCylinder.h"

__host__ __device__ void printf_resultsij_and_id( int id, const AResultSystemStep_t& prev, const AResultSystemStep_t& res ) {
	printf( "prev blockId:%d,ddot_z=%lf, dot_z=%lf,z=%lf,theta=%lf,eta=%lf,u_z=%lf,dot_u_z=%lf,F_damp=%lf, F_drag=%lf,F_prevt=%lf, F_wave=%lf,F=%lf\n",
	        id, prev.ddot_z, prev.dot_z, prev.z, prev.theta, prev.eta, prev.u_z, prev.dot_u_z, prev.F_damp, prev.F_drag, prev.F_rest, prev.F_wave, prev.F
	      );
	printf( "curr blockId:%d,ddot_z=%lf, dot_z=%lf,z=%lf,theta=%lf,eta=%lf,u_z=%lf,dot_u_z=%lf,F_damp=%lf, F_drag=%lf,F_rest=%lf, F_wave=%lf,F=%lf\n",
	        id, res.ddot_z, res.dot_z, res.z, res.theta, res.eta, res.u_z, res.dot_u_z, res.F_damp, res.F_drag, res.F_rest, res.F_wave, res.F
	      );
}


__host__ __device__ void printf_result_and_id( int id, const AResultSystemStep_t& res ) {
	printf( "blockId:%d,ddot_z=%lf, dot_z=%lf,z=%lf,theta=%lf,eta=%lf,u_z=%lf,dot_u_z=%lf,F_damp=%lf, F_drag=%lf,F_rest=%lf, F_wave=%lf,F=%lf\n",
	        id, res.ddot_z, res.dot_z, res.z, res.theta, res.eta, res.u_z, res.dot_u_z, res.F_damp, res.F_drag, res.F_rest, res.F_wave, res.F
	      );
}

__host__ __device__ void printf_results( const AResultSystemStep_t& res ) {
	printf( "ddot_z=%lf, dot_z=%lf,z=%lf,theta=%lf,eta=%lf,u_z=%lf,dot_u_z=%lf,F_damp=%lf, F_drag=%lf,F_rest=%lf, F_wave=%lf,F=%lf\n",
	        res.ddot_z, res.dot_z, res.z, res.theta, res.eta, res.u_z, res.dot_u_z, res.F_damp, res.F_drag, res.F_rest,
	        res.F_wave, res.F
	      );
}


__global__ void kernelPerstep(
		AResultSystemStep_t* d_result, const AWavePuls_t* d_wave, const AElem_t elem,
		const ACylinderFloating_t cons, const int i
) {

	const int id = blockDim.x * blockIdx.x + threadIdx.x;

	const AWavePuls_t t_wave = d_wave[id];



	const AResultSystemStep_t t_result = d_result[id];//from global memory,will not change,for the data source of next data

	d_result[id].calc( t_result, t_wave, elem, cons, i );

	if (id<10) {
		printf_resultsij_and_id( id, t_result, d_result[id] );

	}
}

void echoError( cudaError_t& err, const char* strs ) {
	char a[255];
	if (err != cudaSuccess) {
		strncpy( a, strs, 255 );
		fprintf( stderr, "Failed to %s,errorCode %s", a, cudaGetErrorString( err ) );
		exit( EXIT_FAILURE );
	}
}

//this is useful ,all generators and methods are inside

__host__ __device__ AResultSystemStep_t::AResultSystemStep_t( ) : ddot_z( 0 ), dot_z( 0 ), z( 0 ), theta( 0 ), eta( 0 ),
                                                                  u_z( 0 ), dot_u_z( 0 ), F_damp( 0 ), F_drag( 0 ),
                                                                  F_rest( 0 ), F_wave( 0 ), F_added2( 0 ), F( 0 ) {
};

__host__ __device__ AResultSystemStep_t::AResultSystemStep_t(
		const double ddot_z, const double dot_z, const double z, \
                                const double theta, const double eta, const double u_z, \
                                const double dot_u_z, const double F_damp, const double F_drag, \
                                const double F_rest, const double F_wave, double F_added2, double F
) : ddot_z( ddot_z ), dot_z( dot_z ), z( z ), theta( theta ), eta( eta ),
    u_z( u_z ), dot_u_z( dot_u_z ), F_damp( F_damp ), F_drag( F_drag ),
    F_rest( F_rest ), F_wave( F_wave ), F_added2( F_added2 ), F( F ) {
}

__host__ __device__ void AResultSystemStep_t::set_initial( const double ddot_z_, const double dot_z_, const double z_, double F_ ) {
	//values needed at t=1*dt;
	ddot_z   = ddot_z_;
	dot_z    = dot_z_;
	z        = z_;
	F        = F_;
	//values not needed at t=1*dt;
	theta    = 0;
	eta      = 0;
	u_z      = 0;
	dot_u_z  = 0;
	F_damp   = 0;
	F_drag   = 0;
	F_rest   = 0;
	F_wave   = 0;
	F_added2 = 0;

};


__host__ __device__ AResultSystemStep_t AResultSystemStep_t::operator =( const AResultSystemStep_t& a ) {
	return AResultSystemStep_t( a );
}

__host__ __device__ AResultSystemStep_t AResultSystemStep_t::operator +( const AResultSystemStep_t& a ) {//DEVICE
	return AResultSystemStep_t( a ); //TODO PROPER SUM OF WAVES
}

__host__ __device__ AResultSystemStep_t& AResultSystemStep_t::operator +=( const AResultSystemStep_t& a ) {

	//*TODO proper integration over waves
	ddot_z += a.ddot_z;//
	dot_z += a.dot_z;//
	z += a.z; //

	//theta += a.theta;

	eta += a.eta;
	u_z += a.u_z;
	dot_u_z += a.dot_u_z;

	F_damp += a.F_damp;
	F_drag += a.F_drag;
	F_rest += a.F_rest;
	F_wave += a.F_wave;
	F_added2 += a.F_added2;
	F += a.F;//
	// */
	return *this;

}


__host__ __device__ void AResultSystemStep_t::calc(
		const AResultSystemStep_t& prev, const AWavePuls_t& wave, const AElem_t& elem,
		const ACylinderFloating_t& cons, const int i
) {
	ddot_z = (prev.F - prev.F_added2) / cons.coefMass / elem.m;
	dot_z  = prev.dot_z + ddot_z * cons.dt;
	z      = prev.z + 0.5 * (prev.dot_z + dot_z) * cons.dt;

	theta = i * cons.dt * wave.om - wave.eps;
	eta   = wave.amp * cos( theta );

	u_z     = wave.om * wave.amp * exp( wave.k * (z - elem.d) ) * sin( theta );
	dot_u_z = pow( wave.om, 2 ) * wave.amp * exp( wave.k * (z - elem.d) ) * cos( theta );

	F_damp   = -dot_z * cons.damp33;
	F_drag   = 0.5 * cons.Rho * cons.areaProjected * cons.coefDrag * fabs( u_z - dot_z ) * (u_z - dot_z);
	F_rest   = -1 * cons.Rho * cons.Grav * cons.areaProjected * z;
	F_added2 = elem.m * (1 + cons.coefAddmass) * dot_u_z;
	F_wave   = cons.Grav * cons.Rho * cons.areaProjected * eta;
	F        = F_damp + F_drag + F_rest + F_added2 + F_wave;
}

__global__ void kernelReduceWave( AResultSystemStep_t* d_result, AResultSystemStep_t* g_blockResult ) {


	size_t tinb = blockDim.x;//how many threads in one block
	size_t idx  = blockIdx.x * blockDim.x + threadIdx.x;
	size_t tid  = threadIdx.x;


	extern __shared__ AResultSystemStep_t t_Result[];


	t_Result[tid] = d_result[idx];

	__syncthreads();

	for ( unsigned int s = 1; s < tinb; s *= 2 ) {
		if (tid % (2 * s) == 0) {
			t_Result[tid] += t_Result[tid + s];
		}
		__syncthreads();
	}
	if (tid == 0) {
		g_blockResult[blockIdx.x] = t_Result[0];//this works, so it means the problem is overwrite operator+= doesn't
	}
}

/*
AElem         oed;
AWavePuls_t       owd;
OElemCylinder oec;
OElem         oed;

*/

OElemCylinder oec;
AWavePuls_t       awd;
OWave         ow;
OTime         ot;


AResultSystem_t::~AResultSystem_t( ) {
	printf( "delete AResultSystem_t::AResultSystem_t (0, rank=%d, %d, %d, %d)\n", myrank, numSteps, numElems, numWaves );

	if (myrank) {
	//	free1d(&resultsInitial);
	//	free1d(&resultsBlocks);
		resultsInitial = NULL;
		resultsBlocks  = NULL;

	} else {
	}

	//free2d( &results );
	results = NULL;

	printf( "delete AResultSystem_t::AResultSystem_t (0, rank=%d, %d, %d, %d)\n", myrank, numSteps, numElems, numWaves );

}

template <typename T>
__host__ __device__ void print_arr( T* a, int N ) {
	printf( "A[%d/%d]=%f\n", 0, N, a[0] );
	for ( int i = 1; i < N; i++ ) {
		printf( "A[%d/%d]=%f\n", i, N, a[i] );

	}
}

AResultSystem_t::AResultSystem_t( int myrank, int numElems, int numSteps, int numWaves, double t0, double t1 ) :
		myrank( myrank ), numElems( numElems ), numSteps( numSteps ), numWaves( numWaves ),
		size( numElems * numSteps ), t0( t0 ), t1( t1 ), dt( (t1 - t0) / numSteps ) {
	printf( "AResultSystem_t::AResultSystem_t (rank=%d, %d, %d, %d)\n", myrank, numSteps, numElems, numWaves );

	malloc2d( &results, numElems, numSteps );


	if (myrank) {
		printf( "AResultSystem_t::AResultSystem_t (rank>0, %d, %d, %d)\n", numSteps, numElems, numWaves );

		blocksPerGrid = (numWaves + threadsPerBlock - 1) / threadsPerBlock;

		malloc1d(&resultsInitial,numWaves);
		malloc1d(&resultsBlocks,blocksPerGrid);

		err = cudaMalloc( ( void** ) &d_wave, numWaves * sizeof( AWavePuls_t ) );
		echoError( err, "allocate the d_wave" );

		err = cudaMalloc( ( void** ) &d_resultsInitial, sizeofResultSystemStep * numWaves );
		echoError( err, "allocate d_results" );

		err = cudaMalloc( ( void** ) &d_resultsBlocks, sizeofResultSystemStep * blocksPerGrid );
		echoError( err, "allocate d_BlocksResult" );

		memset( resultsBlocks, 0, sizeofResultSystemStep * blocksPerGrid );//how many waves,how many initilization

		ot = OTime( t0, t1, dt );

		OSUBSYSTEMW += ot;

		//ot = OTime( t0, t1, dt );
	}
}


void AResultSystem_t::compute(
		const AWaves_t& wavesData, const ACylinderFloating_t& structData,
		const AElem_t& elemData, int myrank
) {

	oec = OElemCylinder( structData.coefAddmass, structData.coefDrag, structData.damp33,
	                     structData.diameter, elemData.d, structData.height,
	                     structData.Rho, structData.Grav
	                   );

	oec.initial_conditions( numSteps, dt, elemData.d, 0, 0, elemData.z0, 0, 0, elemData.dotz0 );

	OSUBSYSTEMW += oec;

	for ( int idW = 0; idW < numWaves; idW++ ) {

		awd = wavesData.w[idW];

		ow = OWave( awd.amp, awd.T, structData.depth, structData.vx,
		            structData.vy, structData.phi, structData.Rho, structData.Grav
		          );

		OSUBSYSTEMW += ow;

		OSUBSYSTEMW.initial_conditions( numSteps, dt, elemData.d, 0, 0, elemData.z0, 0, 0, 0 );

	}

	OSUBSYSTEMW.calc();

}


void AResultSystem_t::compute_cuda(
		const AWaves_t& wavesData, const ACylinderFloating_t& structData,
		const AElem_t& elemData, int myrank
) {


	oec = OElemCylinder( structData.coefAddmass, structData.coefDrag, structData.damp33,
	                     structData.diameter, elemData.d, structData.height,
	                     structData.Rho, structData.Grav
	                   );

	oec.initial_conditions( numSteps, dt, elemData.d, 0, 0, elemData.z0, 0, 0, elemData.dotz0 );

	OSUBSYSTEMW += oec;

	for ( int idW = 0; idW < numWaves; idW++ ) {

		awd = wavesData.w[idW];

		//printf("OWAVE ::: awd.amp=%f, awd.T=%f, structData.depth=%f, structData.vx=%f, structData.vy=%f, structData.phi=%f, structData.Rho=%f, structData.Grav=%f \n", awd.amp, awd.T, structData.depth, structData.vx, structData.vy, structData.phi, structData.Rho, structData.Grav );
		ow = OWave( awd.amp, awd.T, structData.depth, structData.vx, structData.vy, structData.phi, structData.Rho, structData.Grav );

		OSUBSYSTEMW += ow;

		OSUBSYSTEMW.initial_conditions( numSteps, dt, elemData.d, 0, 0, elemData.z0, 0, 0, 0 );

	}

	double tmp[12];
	double z, dot_z, ddot_z, F;

	//initially Fadded is 0, so dont neet to retrive for t=0, cos: ddotz=0
	for ( int idW = 0; idW < numWaves; idW++ ) {
		OSUBSYSTEMW.calc_step( idW, 0 );
		OSUBSYSTEMW.get_step_current_elem( tmp );
		//OSUBSYSTEMW.print_current_step();
		z      = tmp[2];
		dot_z  = tmp[5];
		ddot_z = tmp[8];
		F      = tmp[11];
		//printf("OSUBSYSTEMW ::: z=%f, dot_z=%f, ddot_z=%f, F=%f\n",z,dot_z,ddot_z,F);

		resultsInitial[idW].set_initial( ddot_z, dot_z, z, F );
	}
	printf( "Copy h_wave data from the host memory to the CUDA device\n" );
	err = cudaMemcpy( d_wave, wavesData.w, sizeof( AWavePuls_t ) * numWaves, cudaMemcpyHostToDevice );
	echoError( err, "copy from the h_wave to d_wave" );

	printf( "Copy buoy data previous step from the host memory to the CUDA device\n" );
	err = cudaMemcpy( d_resultsInitial, resultsInitial, sizeof( AResultSystemStep_t ) * numWaves, cudaMemcpyHostToDevice );
	echoError( err, "copy from the h_bouy to d_results" );

	ACylinderFloating_t d_structData = structData;
	AElem_t             d_elemData   = elemData;

	results[0][0].myrank = myrank;
	results[0][0].elemId = elemData.idElem;
	for ( int k = 1; k < numSteps; k++ ) {
		//printf( "kernelPerstep kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock );
		//AResultSystemStep_t* d_result, const AWavePuls_t* d_wave, const AElem_t* d_elem,const ACylinderFloating_t& cons, const int i
		kernelPerstep << < blocksPerGrid, threadsPerBlock >> > (d_resultsInitial, d_wave, d_elemData, d_structData, k);//
		err = cudaGetLastError();
		echoError( err, "to launch kernelPerstep kernel" );

		//printf( "kernelReduceWave kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock );
		kernelReduceWave << < blocksPerGrid, threadsPerBlock >> > (d_resultsInitial, d_resultsBlocks);
		err = cudaGetLastError();
		echoError( err, "to launch kernelReduceWave kernel" );

		err = cudaMemcpy( resultsBlocks, d_resultsBlocks, sizeof( AResultSystemStep_t ) * blocksPerGrid, cudaMemcpyDeviceToHost );
		echoError( err, "copy from d_BlocksResult to h_BlocksResult" );
		printf( "copy from d_BlockResult (%d) successfully!\n", blocksPerGrid );
		printf("time step = %d :::",k);
		for ( int i = 0; i < blocksPerGrid; i++ ) {
			printf_result_and_id(i,resultsBlocks[i]);
			//results[0][k] += resultsBlocks[i]; //TODO : integration over waves : sum_j^N { A_j*dom_j}
		}

	}


}

