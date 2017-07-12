//
// Created by robgrzel on 01.06.17.
//

#ifndef UTILS_MPI_SLAVE_H
#define UTILS_MPI_SLAVE_H

#include <mpi.h>
#include "structs.cuh"

#define ID -1
#define DATA 0
#define RESULT 1
#define FINISH 2


#define MPI_SLAVE_TPACK  <class TRs, class TR, class TWs, class TW,  class TEs, class TE, class TS>

#define MPI_SLAVE_T TRs,  TR,  TWs,  TW,   TEs,  TE, TS


template MPI_SLAVE_TPACK class MpiSlave {
	private:

		MPI_Request* requestSend;
		MPI_Request* requestReceive;
		MPI_Status status;


		int myrank;
		int proccount;

		int flag;
		int NonFinishFlag = 1;

		int m   = 1;
		int k   = -1;
		int cnt = -1;

		bool doBreak;


	private:

		TS oSTRUCT;

		TE oELEM_[2];

		TW  oWAVE;
		TWs oWAVES;

		AResultSystem_t oRESULTS;

	public:


		#define MPI_SLAVE_ARGS (int numElems, int numWaves, int numSteps, double t0, double t1, int myrank, int proccount)
		#define MPI_SLAVE_INST oSTRUCT(myrank,numElems),oRESULTS(myrank,1,numSteps, numWaves, t0, t1), \
								oWAVES(myrank,numWaves), myrank( myrank ), proccount( proccount )


		MpiSlave MPI_SLAVE_ARGS;

	private:


		void init( );

		void recv_results( );

		void recv_const( );

		void recv_idem( );

		void irecv_idem( );

		void run_while( );

		void run_while_calc( );

		void run_while_isend( );

		void run_while_irecv_tag_data( );

		void run_while_irecv_tag_finish( );


		bool run_while_irecv( );


};


#endif //UTILS_MPI_SLAVE_H
