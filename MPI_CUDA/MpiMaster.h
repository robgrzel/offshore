//
// Created by robgrzel on 02.06.17.
//

#ifndef UTILS_MPIMASTER_H
#define UTILS_MPIMASTER_H


#include <mpi.h>
#include "structs.cuh"
//#include "UtilSources/mem_utils.h"

#define ID -1
#define DATA 0
#define RESULT 1
#define FINISH 2


#define MPI_MASTER_TPACK  <class TRs, class TR, class TWs, class TW,  class TEs, class TE, class TS>

#define MPI_MASTER_T TRs,  TR,  TWs,  TW,   TEs,  TE, TS

template MPI_MASTER_TPACK class MpiMaster {
	private:



		MPI_Request* requestSend;
		MPI_Request* requestReceive;
		MPI_Status status;


		int cnt       = -1;

		int requestIndex;
		int sentcount = 0;
		int recvcnt   = 0;

		int i;

		TE  oELEM;
		TEs oELEMS = NULL;

		TWs oWAVES = NULL;

		TS oSTRUCT = NULL;

		TRs oRESULTS = NULL;

		int myrank, proccount;


	public:

		#define MPI_MASTER_INC

		#define MPI_MASTER_ARGS (TS oSTRUCT, int numWaves, int numSteps, double t0, double t1, int myrank, int proccount )

		MpiMaster (TS oSTRUCT, int numWaves, int numSteps, double t0, double t1, int myrank, int proccount );

		~MpiMaster( );

	private:

		void init( );

		void send_const( );

		void send_idem( );

		void isend_idem( );

		void irecv_results( );

		void run_while_waits( );

		void run_while_isend_check_anylast( );

		void run_while_icomm( );

		void run_while( );

		void after_while( );

		void kill_slaves( );

		void check_results( );

		void recv_orphans( );

};


#endif //UTILS_MPIMASTER_H
