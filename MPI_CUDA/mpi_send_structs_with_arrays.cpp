#include <mpi.h>

typedef struct bar {
	int c;
	int d;
	int i, j;
};

typedef struct BAR {
	int a;
	int b;

	struct bar** pp_bar;
};

void bar_free( auto pp, int N ) {

	for ( int i = 0; i < N; i++ ) {
		delete[] pp[i];
	}
	delete[] pp;

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

template<typename T>
T adder(T v) {
	std::cout << __PRETTY_FUNCTION__ << "\n";
	return v;
}

template<typename T, typename... Args>
T adder(T first, Args... args) {
	std::cout << __PRETTY_FUNCTION__ << "\n";
	return first + adder(args...);
}

int main( int argc, char* argv[] ) {

	//adder(1, 2, 3, 8, 7);

	MPI_Init( &argc, &argv );
	int rank, p;
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &p );

	MPI_Request req[2];
	req[0] = req[1] = MPI_REQUEST_NULL;

	int cLen = 10;


	struct BAR MSG;
	struct bar msg;
	struct bar* p_msg;
	struct bar** pp_msg;

	if (rank == 0) {
		printf( "rank 0\n" );
		MSG.a = 1;
		MSG.b = -1;

		malloc2d_( &MSG.pp_bar, cLen, cLen );

		for ( int i = 0; i < cLen; i++ ) {
			for ( int j = 0; j < cLen; j++ ) {
				msg.c = i * j;
				msg.d = -1;
				msg.i = i;
				msg.j = j;
				MSG.pp_bar[i][j] = msg;
			}

		}

		printf( "Global array is:\n" );
		for ( int i = 0; i < cLen; i++ ) {
			for ( int j = 0; j < cLen; j++ ) {
				printf("%.5d, ", MSG.pp_bar[i][j].c );
			}
			printf( "\n" );
		}

		MPI_Isend( &(MSG.pp_bar[4][0]), sizeof( msg ) * cLen, MPI_BYTE, 1, 1, MPI_COMM_WORLD, &req[0] );

		free2d(&MSG.pp_bar);


	} else if (rank == 1) {
		p_msg = new bar[cLen];

		MPI_Status status;
		// Probe for a message coming from rank 0 with any tag.
		int flag = 0;
		while ( p_msg[0].d != -1 ) {
			MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
			printf("flag=%d, status.MPI_TAG=%d\n",flag,status.MPI_TAG);
			// Find out what type of message is ready to be received by accessing the MPI_TAG variable in the status struct
			if (status.MPI_TAG == 1) {
				// Receive messages of this type. Maybe you sent an array of ints?
				MPI_Irecv( &p_msg[0], sizeof( msg ) * cLen, MPI_BYTE, 0, 1, MPI_COMM_WORLD, &req[0] );
				MPI_Waitall( 1, req, MPI_STATUS_IGNORE );
				printf( "waited...\n" );
				if (p_msg[0].d == -1) {
					for ( int i = 0; i < cLen; i++ ) {
						printf( "p_msg[%d] : c=%d, d=%d\n", i, p_msg[i].c, p_msg[i].d );
					}
				}
			}


		}


		free( p_msg );

	}

	MPI_Finalize();

	return 0;
}