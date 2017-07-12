#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <random>

#define MaxProcesses 16
#define ArrayLength 1
#define DATA 0
#define RESULT 1
#define FINISH 2
#define DEBUG

#define PI 3.14

#define NUM_OF_WAVES size_t(1e1)
#define NUM_OF_ELEMENTS size_t(3e1)
#define NUM_OF_STEPS size_t(1e4)


#define PRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)

#ifdef DEBUG
	#define DPRINTF( format, ... ) fprintf (stderr, format, ##__VA_ARGS__)
#else
	#define DPRINTF(format, ...)
#endif


template <typename T>
const char* BytesToSize( T Bytes ) {

	DPRINTF( "BytesToSize : Bytes=%d,%f\n", Bytes, float(Bytes) );

	float tb = 1099511627776;
	float gb = 1073741824;
	float mb = 1048576;
	float kb = 1024;


	char* returnSize;


	if (Bytes >= tb) {
		sprintf( returnSize, "%.2f TB", Bytes / tb );
	} else if (Bytes >= gb && Bytes < tb) {
		sprintf( returnSize, "%.2f GB", Bytes / gb );
	} else if (Bytes >= mb && Bytes < gb) {
		sprintf( returnSize, "%.2f MB", Bytes / mb );
	} else if (Bytes >= kb && Bytes < mb) {
		sprintf( returnSize, "%.2f KB", Bytes / kb );
	} else if (Bytes < kb) {
		sprintf( returnSize, "%.2f Bytes", Bytes );
	} else {
		sprintf( returnSize, "%.2f Bytes", Bytes );
	}

	DPRINTF( "BytesToSize e\n" );

	return returnSize;
}


template <typename T>
int malloc1d( T** array, int n) {

	/* allocate the row pointers into the memory */
	(*array) = ( T* ) malloc( n * sizeof( T ) );
	if (!(*array)) {
		return -1;
	}

	return 0;
}

template <typename T>
int malloc1d_p( T*** array, int n ) {
	(*array) = ( T** ) malloc( n * sizeof( T* ) );
	if (!(*array)) {
		return -1;
	}
	return 0;
}
template <typename T>
int alloc_1d_in_1dp(T *** array, T * row, int n, int m){
	/* set up the pointers into the contiguous memory */
	for ( int i = 0; i < n; i++ ) {
		(*array)[i] = &(row[i * m]);
	}
	return 0;
}

template <typename T>
int malloc2d_( T*** array, int n, int m ) {

	int o = 0;
	/* allocate the n*m contiguous items */
	T * p;

	o = malloc1d(&p,n*m);
	if (o) return -1;

	/* allocate the row pointers into the memory */
	o = malloc1d_p(array,n);

	if (o) {
		free(p);
		return -1;
	}

	/* set up the pointers into the contiguous memory */
	alloc_1d_in_1dp(array,p,n,m);

	return 0;
}
template <typename T>
int malloc2d( T*** array, int n, int m ) {

	int o = 0;
	/* allocate the n*m contiguous items */
	T * p;

	malloc1d(&p,n*m);
	if (!p) return -1;

	/* allocate the row pointers into the memory */
	(*array) = ( T** ) malloc( n * sizeof( T* ) );
	if (!(*array)) {
		free( p );
		return -1;
	}

	/* set up the pointers into the contiguous memory */
	for ( int i = 0; i < n; i++ ) {
		(*array)[i] = &(p[i * m]);
	}

	return 0;
}
template <typename T>
int free2d( T*** array ) {
	/* free the memory - the first element of the array is at the start */
	free( &((*array)[0][0]) );

	/* free the pointers into the memory */
	free( *array );

	return 0;
}

typedef struct OElem_t {
	int    idElem = 0;
	double dZ0;
	double Z0;
	double dotZ0;

} OElem;

typedef struct OElems_t {
	const int numElems = NUM_OF_ELEMENTS;
	OElem     e[NUM_OF_ELEMENTS];
} OElems;

typedef struct OSeabedMonopileData_t {

	double Grav;
	double Rho;

	double massB;
	double massInit;
	double massTotal;

	double coefDrag;
	double damp33;
	double coefAddmass;

	double diameter;
	double areaWaterline;
	double areaProjected;

	double z0 = -40;
	double z1 = 10;
	double H  = z1 - z0;
	double dz = H / NUM_OF_ELEMENTS;

} OSeabedMonopileData;

typedef struct OWaveData_t {
	int    id  = 0;
	double om  = 0.5;
	double amp = 1;
	double eps = 0;
} OWaveData;

typedef struct OWavesData_t {
	const int numWaves = NUM_OF_WAVES;

	OWaveData w[NUM_OF_WAVES];

} OWavesData;

/*
//TODO:
CREATE 2D ORESULTSUBSYSTEM OR[NUM_OF_ELEMENTS][NUM_OF_STEPS] PASS IT
*/
typedef struct OResultSystemStep_t {


	int id = -1;

	double Forces;
	double Displacement;
	double Velocity;
	double Acceleration;


} OResultSystemStep;


typedef struct OResultSystem_t {

	int sizeof_1 = sizeof( OResultSystem_t ) - sizeof( OResultSystemStep_t** );
	int sizeof_2 = sizeof( OResultSystemStep_t ) * NUM_OF_ELEMENTS;
	int sizeof_3 = sizeof( OResultSystemStep_t ) * NUM_OF_STEPS;

	const int numElems = NUM_OF_ELEMENTS;
	const int numSteps = NUM_OF_STEPS;
	const int size     = NUM_OF_STEPS * NUM_OF_ELEMENTS;

	int idRecv   = -111;
	int cntElems = 0;
	int cntWaves = 0;

	OResultSystemStep_t** s;

} OResultSystem;

void number_of_elements( OSeabedMonopileData& o ) {
	//o.numElems = int( (o.z1 - o.z0) / o.dz );

}

void generate_slices( OSeabedMonopileData& o, OElems& E ) {

	//double Z[o.numElems];

	E.e[0].Z0     = o.z0 + o.dz / 2.;
	E.e[0].dZ0    = o.dz;
	E.e[0].dotZ0  = 0;
	E.e[0].idElem = 0;

	for ( int i = 1; i < E.numElems; i++ ) {
		E.e[i].dotZ0  = 0;
		E.e[i].dZ0    = o.dz;
		E.e[i].Z0     = E.e[i - 1].Z0 + o.dz;
		E.e[i].idElem = i;
	}
	//o.Z = Z;

}

void standard_normal_2( const int N, double* A ) {
	std::default_random_engine       de( 0 ); //seed
	std::normal_distribution<double> nd( 0, 1 ); //mean followed by stdiv

	for ( int i = 0; i < N; ++i ) {
		A[i] = 2 * nd( de ); //Generate numbers;
	}

}

void generate_irregular_wave_data( OWavesData& o ) {


	double EPS[o.numWaves];

	standard_normal_2( o.numWaves, EPS );

	for ( int i = 0; i < o.numWaves; i++ ) {
		o.w[i].eps = EPS[i];
		o.w[i].id  = i;
	}

}

void cylinder_geometry( const double diameter, double& area ) {
	area = PI * pow( diameter / 2, 2 );
}

int main( int argc, char** argv ) {
	MPI_Request* requestSend, * requestReceive;

	DPRINTF( "ARRAYlEN=%d,%d\n", ArrayLength );

	int myrank, proccount;

	int sentcount = 0, recvcnt = 0;
	int i;

	MPI_Status status;
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
	MPI_Comm_size( MPI_COMM_WORLD, &proccount );

	if (proccount < 2) {
		DPRINTF( "Run with at least 2 processes\n" );
		MPI_Finalize();
		return -1;
	}
	if (NUM_OF_ELEMENTS < 2 * (proccount - 1)) {
		DPRINTF( "More subranges needed\n" );
		MPI_Finalize();
		return -1;
	}

	OElems              oes;
	OWavesData          ows;
	OSeabedMonopileData osmd;

	OElem         oe;
	OWaveData     ow;
	OResultSystem ors;


	if (myrank == 0) {

		int cnt = -1;

		DPRINTF( "MASTER START ### ###\n" );

		requestSend = ( MPI_Request* ) malloc( (proccount - 1) * sizeof( MPI_Request ) );

		requestReceive = ( MPI_Request* ) malloc( (proccount - 1) * sizeof( MPI_Request ) );

		DPRINTF( "---\n" );
		DPRINTF( "---%d. master init structs\n", ++cnt );


		for ( int i = 0; i < (proccount - 1); i++ ) {
			requestReceive[i] = MPI_REQUEST_NULL;
		}
		DPRINTF( "---%d. master prep structs\n", ++cnt );

		generate_slices( osmd, oes );
		DPRINTF( "---%d. master prep structs\n", ++cnt );

		generate_irregular_wave_data( ows );
		malloc2d( &ors.s, ors.numElems, ors.numSteps );


		if (!ors.s) {
			DPRINTF( "\nNot enough memory" );
			MPI_Finalize();
			return -1;
		}

		OResultSystemStep * orss;

		for ( i = 1; i < proccount; i++ ) {
			oe = oes.e[i - 1];
			DPRINTF( "--- --->%d. master isend 0 ::: to = %d, oe[idElem=%d]=%f\n", ++cnt, i, oe.idElem, oe.Z0 );
			MPI_Send( &(ows), sizeof( OWavesData ), MPI_BYTE, i, DATA, MPI_COMM_WORLD );
			MPI_Send( &(osmd), sizeof( OSeabedMonopileData ), MPI_BYTE, i, DATA, MPI_COMM_WORLD );
			MPI_Send( &(oe), sizeof( OElem ), MPI_BYTE, i, DATA, MPI_COMM_WORLD );
			MPI_Send( &(ors), ors.sizeof_1, MPI_BYTE, i, DATA, MPI_COMM_WORLD );
			sentcount++;
		}

		int requestIndex;
		DPRINTF( "--->%d. master isend 1\n", ++cnt );

		for ( int i = 1; i < proccount; i++ ) {//Process from 1~~proccount-1
			oe = oes.e[sentcount];
			DPRINTF( "--- --->%d. master isend 1 ::: to = %d, oe[idElem=%d]=%f\n", ++cnt, i, oe.idElem, oe.Z0 );

			MPI_Isend( &oe, sizeof( OElem ), MPI_BYTE, i, DATA, MPI_COMM_WORLD, &(requestSend[i - 1]) );
			sentcount++;
		}

		for ( int i = 1; i < proccount; i++ ) {//Process from 1~~proccount-1

			DPRINTF( "--- ---<%d. master irecv 1 ::: from=%d\n", ++cnt, i );
			MPI_Irecv( &(ors.s[i - 1][0]), ors.sizeof_3, MPI_BYTE, i, RESULT, MPI_COMM_WORLD, &(requestReceive[i - 1]) );

		}

		do {
			DPRINTF( "---%d. MASTER BEGIN WHILE LOOP 1 (oe.idElem %d <= %d NUM_OF_ELEMENTS - 1) --- --->\n", ++cnt, oe.idElem, NUM_OF_ELEMENTS - 1 );

			DPRINTF( "--- ---%d. master MPI_Waitany(0)\n", ++cnt );
			MPI_Waitany( proccount - 1, requestReceive, &requestIndex, MPI_STATUS_IGNORE );
			DPRINTF( "--- ---%d. master MPI_Waitany(e: requestReceive from=%d)\n", ++cnt, requestIndex + 1 );

			orss = ors.s[proccount - 1 + recvcnt];
			DPRINTF( "--- ---<<<%d. master 2 MPI_Irecv(o : recvcnt=%d, prev orss.id = %d) 2\n", ++cnt, proccount - 1 + recvcnt, orss[0].id );
			MPI_Irecv( &(ors.s[proccount - 1 + recvcnt][0]), ors.sizeof_3, MPI_BYTE, requestIndex + 1, RESULT, MPI_COMM_WORLD,
			           &(requestReceive[requestIndex])
			         );
			DPRINTF( "--- ---<<<%d. master 2 MPI_Irecv(e) 2\n", ++cnt );
			recvcnt++;

			//result += resultTemp[requestIndex];

			DPRINTF( "--- ---%d. master MPI_Wait(0:request send : sentcnt=%d, to=%d)\n", ++cnt, sentcount, requestIndex + 1 );
			MPI_Wait( &requestSend[requestIndex], MPI_STATUS_IGNORE );
			DPRINTF( "--- ---%d. master MPI_Wait(e:request send : sentcnt=%d, to=%d)\n", ++cnt, sentcount, requestIndex + 1 );

			if (sentcount == NUM_OF_ELEMENTS) {
				oe = {0};
				oe.idElem = -1;
			} else {
				oe = oes.e[sentcount];
				sentcount++;

			}

			DPRINTF( "--- --->>>%d. master MPI_Isend(0:oe oe[idElem=%d]=%f)\n", ++cnt, oe.idElem, oe.Z0 );
			MPI_Isend( &oe, sizeof( OElem ), MPI_BYTE, requestIndex + 1, DATA, MPI_COMM_WORLD, &requestSend[requestIndex] );
			DPRINTF( "--- --->>>%d. master MPI_Isend(e:oe)\n", ++cnt );


		} while ( oe.idElem < NUM_OF_ELEMENTS );

		DPRINTF( "---%d. MASTER WHILE END...\n", ++cnt );

		MPI_Waitall( 1 * (proccount - 1), requestReceive, MPI_STATUS_IGNORE );

		for ( int i = 0; i < proccount - 1; i++ ) {
			recvcnt++;
			//result += resultTemp[i];
		}

		//shut down the slave
		//Maybe I should send special number
		for ( i = 1; i < proccount; i++ ) {
			MPI_Send( NULL, 0, MPI_DOUBLE, i, FINISH, MPI_COMM_WORLD );
		}

		DPRINTF( "MASTER cnt: recv/sent=%d/%d//%d\n", recvcnt, sentcount, sentcount - recvcnt );
		for ( int i = 0; i < sentcount; i++ ) {
			DPRINTF( "<<<<<%d. master 1 MPI_Irecv(0:ors.s[%d] : id=%d Acceleration=%f) 1\n", ++cnt, i, ors.s[i][0].id, ors.s[i][0].Forces );

		}
		while ( recvcnt < sentcount ) {
			//DOUBLT ,If I use IRecv will it be too bog
			DPRINTF( "--- ---<%d. master 3 MPI_Recv(0) 3 : recv=%d, sent=%d\n", ++cnt, recvcnt, sentcount );
			MPI_Recv( &(ors.s[recvcnt][0]), ors.sizeof_3, MPI_BYTE, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status );

			//MPI_Recv( &(A), NUM_OF_STEPS, MPI_DOUBLE, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status );
			DPRINTF( "--- ---<%d. master 3 MPI_Recv(e: ors.s[%d] : id=%d Z0=%f) 3\n", ++cnt, recvcnt, ors.s[recvcnt][0].id,
			         ors.s[recvcnt][0].Forces );
			recvcnt++;
		}
		DPRINTF( "MASTER FINALIZE...\n" );


		for ( int i = 0; i < sentcount; i++ ) {
			DPRINTF( "<<<<<%d. master 1 MPI_Irecv(0:ors.s[%d] : id=%d Acceleration=%f) 1\n", ++cnt, i, ors.s[i][0].id, ors.s[i][0].Forces );

		}
		DPRINTF( "MASTER cnt: recv/sent=%d/%d\n", recvcnt, sentcount );

		// now receive results from the processes
		DPRINTF( "MASTER END...\n" );

	} else {

		int cnt = -1;
		DPRINTF( "SLAVE (%d) BEGIN @@@ @@@\n", myrank );

		DPRINTF( "...%d. slave (%d)\n", ++cnt, myrank );
		DPRINTF( "...%d. slave (%d) prepare structs\n", ++cnt, myrank );

		DPRINTF( "...%d. slave (%d) prepare requests\n", ++cnt, myrank );

		requestSend    = ( MPI_Request* ) malloc( 2 * sizeof( MPI_Request ) );
		requestReceive = ( MPI_Request* ) malloc( 2 * sizeof( MPI_Request ) );


		for ( int i = 0; i < 2; i++ ) {
			requestSend[i] = MPI_REQUEST_NULL;
		}


		OElem oe_[2];
		OResultSystemStep * orss_p ;
		OResultSystemStep ** orss_pp ;


		DPRINTF( "...%d. slave (%d) recv structs\n", ++cnt, myrank );

		MPI_Recv( &ows, sizeof( OWavesData ), MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		MPI_Recv( &(osmd), sizeof( OSeabedMonopileData ), MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		MPI_Recv( &(oe_[0]), sizeof( OElem ), MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		MPI_Recv( &(ors), ors.sizeof_1, MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

		malloc1d(&orss_p,ors.numSteps);
		malloc2d(&orss_pp,2,ors.numSteps);

		int i             = 0;
		int j             = 0;
		int flag;
		int NonFinishFlag = 1;

		int k = -1;
		int m = 1;

		DPRINTF( "...%d. slave (%d) MPI_Irecv(0:oe) 1 (i,j=%d,%d)\n", ++cnt, myrank, i, j );

		MPI_Irecv( &oe, sizeof( OElem ), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &requestReceive[0] );
		DPRINTF( "...%d. slave (%d) MPI_Irecv(e:oe) 1 (i,j=%d,%d)\n", ++cnt, myrank, i, j );

		do {
			m = k;
			k = oe_[0].idElem;
			DPRINTF( "...%d. slave (%d) BEGIN WHILE LOOP (i,j=%d,%d)... oe.id=%d, k=%d, m=%d.../\n", ++cnt, myrank, i, j, oe_[i].idElem, k, m );


			if (k != m) {
				DPRINTF( "... .../%d. slave (%d) calc (i,j=%d,%d)\n", ++cnt, myrank, i, j );
				//resultSlave[i] = SimpleIntegration( myBuffer[i], myBuffer[i] + RANGESIZE );


				for ( int k = 0; k < ors.numSteps; k++ ) {
					orss_pp[0][k].id = oe_[0].idElem;
					orss_pp[0][k].Forces    = oe_[0].Z0;
					orss_pp[0][k].Displacement = oe_[0].Z0;
					orss_pp[0][k].Velocity    = oe_[0].Z0;
					orss_pp[0][k].Acceleration = oe_[0].Z0;
				}

				DPRINTF( "... .../%d. slave (%d) MPI_Wait(0:reqSend[i]) 1 (i,j=%d,%d)\n", ++cnt, myrank, i, j );
				MPI_Waitall( 1, (requestSend), MPI_STATUS_IGNORE );
				DPRINTF( "... .../%d. slave (%d) MPI_Wait(e:reqSend[i]) 1 (i,j=%d,%d)\n", ++cnt, myrank, i, j );

				DPRINTF( "... .../%d. slave (%d) MPI_Isend(0:ors) 1 ors[id=%d]=%f\n", ++cnt, myrank, orss_pp[0][0].id, orss_pp[0][0].Acceleration );
				MPI_Isend( &(orss_pp[0][0]), ors.sizeof_3, MPI_BYTE, 0, RESULT, MPI_COMM_WORLD, &(requestSend[0]) );
				DPRINTF( "... .../%d. slave (%d) MPI_Isend(0:ors) 1 ... \n", ++cnt, myrank );

			}

			DPRINTF( "... .../%d. slave (%d) CHECK FLAG 1 : NonFinishingFlag=%d (i,j=%d,%d)\n", ++cnt, myrank, NonFinishFlag, i, j );
			if (NonFinishFlag == 1) {

				DPRINTF( "... .../%d. slave (%d) MPI_Test(0:flag=%d,status.tag=%d) 1 (i,j=%d,%d)\n", ++cnt, myrank, flag, status.MPI_TAG, i, j );
				MPI_Test( &requestReceive[0], &flag, &status );
				DPRINTF( "... .../%d. slave (%d) MPI_Test(e:flag=%d,status.tag=%d) 1 (i,j=%d,%d)\n", ++cnt, myrank, flag, status.MPI_TAG, i, j );

				if (flag == 1) {//means we have a new data to receive,NoneFinishFlag means should I stop receive data
					DPRINTF( "... .../%d. slave (%d) CHECK FLAG 2 : flag == 1 (i,j=%d,%d)\n", ++cnt, myrank, i, j );
					DPRINTF( "... .../%d. slave (%d) MPI_WAIT(0:reqRecv[0]) 2 (i,j=%d,%d)\n", ++cnt, myrank, i, j );
					MPI_Wait( &requestReceive[0], MPI_STATUS_IGNORE );
					DPRINTF( "... .../%d. slave (%d) MPI_WAIT(e:reqRecv[0]) 2 (i,j=%d,%d)\n", ++cnt, myrank, i, j );
					DPRINTF( "... .../%d. slave (%d) IRecved : oe[%d].Z0=%f\n", ++cnt, myrank, oe.idElem, oe.Z0 );
					DPRINTF( "... .../%d. slave (%d) CHECK STATUS : status.MPI_TAG=%d (i,j=%d,%d)\n", ++cnt, myrank, status.MPI_TAG, i, j );

					if (status.MPI_TAG == DATA) {
						DPRINTF( "... .../%d. slave (%d) status.MPI_TAG == DATA (i,j=%d,%d)\n", ++cnt, myrank, i, j );
						oe_[0] = oe;
						DPRINTF( "... .../%d. slave (%d) IRecv(0:oe) 2 (i,j=%d,%d)\n", ++cnt, myrank, i, j );
						MPI_Irecv( &oe, sizeof( OElem ), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &requestReceive[0] );
						DPRINTF( "... .../%d. slave (%d) IRecv(e:oe) 2 (i,j=%d,%d)\n", ++cnt, myrank, i, j );


					} else if (status.MPI_TAG == FINISH) {
						DPRINTF( "... .../%d. slave (%d) status.MPI_TAG == FINISH (i,j=%d,%d)\n", ++cnt, myrank, i, j );
						NonFinishFlag = 0;//it means I don't have any new data to receive, so I even don't have to test anymore
						DPRINTF( "... .../%d. slave (%d) FINALIZE (i,j=%d,%d)\n", ++cnt, myrank, i, j );
						break;
					}
				}
			}
		} while ( oe.idElem != -1 );

	}
	MPI_Finalize();
	return 0;
}