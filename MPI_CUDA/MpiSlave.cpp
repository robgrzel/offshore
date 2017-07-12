//
// Created by robgrzel on 02.06.17.
//
#include "MpiSlave.h"

#define DEBUG

#define PRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)

#ifdef DEBUG
	#define DPRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)
#else
	#define DPRINTF( format, ... )
#endif


template MPI_SLAVE_TPACK MpiSlave<MPI_SLAVE_T>::MpiSlave MPI_SLAVE_ARGS : MPI_SLAVE_INST {
	DPRINTF( "SLAVE (%d) BEGIN @@@ @@@\n", myrank );


	init();

	recv_const();

	recv_idem();

	irecv_idem();

	run_while();

	return;

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::init( ) {

	DPRINTF( "...%d. slave (%d) init 0\n", ++cnt, myrank );

	requestSend    = ( MPI_Request* ) malloc( 2 * sizeof( MPI_Request ) );
	requestReceive = ( MPI_Request* ) malloc( 2 * sizeof( MPI_Request ) );


	for ( int i = 0; i < 2; i++ ) {
		requestSend[i] = MPI_REQUEST_NULL;
	}

	DPRINTF( "...%d. slave (%d) init e\n", ++cnt, myrank );

}


template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::recv_const( ) {


	DPRINTF( "...%d. slave (%d) recv structs const data 0 : sizeofWavesMpi=%d \n", ++cnt, myrank,oWAVES.sizeofWavesMpi );

	MPI_Recv( &oWAVES, oWAVES.sizeofWavesMpi, MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	DPRINTF( "...%d. slave (%d) recv structs const data 1 : sizeofWaveNum=%d\n", ++cnt, myrank,oWAVES.sizeofWaveNum );
	MPI_Recv( &(oWAVES.w[0]), oWAVES.sizeofWaveNum, MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	DPRINTF( "...%d. slave (%d) recv structs const data 2 \n", ++cnt, myrank );
	MPI_Recv( &(oSTRUCT), oSTRUCT.sizeof_1, MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	DPRINTF( "...%d. slave (%d) recv structs const data e\n", ++cnt, myrank );

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::recv_idem( ) {
	DPRINTF( "...%d. slave (%d) MPI_Recv(0:oELEM) 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );

	MPI_Recv( &(oELEM_[0]), sizeof( TE ), MPI_BYTE, 0, DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	DPRINTF( "...%d. slave (%d) MPI_Recv(e:oELEM, id=%d) 1 (k,m=%d,%d)\n", ++cnt, myrank, oELEM_[0].idElem, k, m );

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::irecv_idem( ) {

	DPRINTF( "...%d. slave (%d) MPI_Irecv(0:oELEM) 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );

	MPI_Irecv( &oELEM_[1], sizeof( TE ), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &requestReceive[0] );

	DPRINTF( "...%d. slave (%d) MPI_Irecv(e:oELEM) 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );


}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::run_while( ) {

	do {
		m = k;
		k = oELEM_[0].idElem;


		if (k != m) {
			run_while_calc();
			run_while_isend();
		}

		doBreak = run_while_irecv();
		if (doBreak) {
			break;
		}


	} while ( oELEM_[0].idElem != -1 or oELEM_[1].idElem!=-1 );

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::run_while_calc( ) {
	DPRINTF( "... .../%d. slave (%d) calc (0: k,m=%d,%d)\n", ++cnt, myrank, k, m );
	//resultSlave[i] = SimpleIntegration( myBuffer[i], myBuffer[i] + RANGESIZE );
	printf("ORESULTS ::   myrank=%d,  blocksPerGrid=%d,  numWaves=%d,  "
			       "  numElems=%d,  numSteps=%d \n",
	       oRESULTS.myrank,oRESULTS.blocksPerGrid,oRESULTS.numWaves,
	       oRESULTS.numElems,oRESULTS.numSteps);
	oRESULTS.compute_cuda(oWAVES,oSTRUCT, oELEM_[0],myrank);
	DPRINTF( "... .../%d. slave (%d) calc (e: k,m=%d,%d)\n", ++cnt, myrank, k, m );

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::run_while_isend( ) {


	DPRINTF( "... .../%d. slave (%d) MPI_Wait(0:reqSend[i]) 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );
	MPI_Waitall( 1, (requestSend), MPI_STATUS_IGNORE );
	DPRINTF( "... .../%d. slave (%d) MPI_Wait(e:reqSend[i]) 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );


	DPRINTF( "... .../%d. slave (%d) MPI_Isend(0:oRESULTS) 1 oRESULTS[id=%d].F=%f, sizeof=%d\n",
	         ++cnt, myrank, oRESULTS.results[0][10].elemId,oRESULTS.results[0][10].F,oRESULTS.sizeofResultSystemStep );
	MPI_Isend( &(oRESULTS.results[0][0]), oRESULTS.sizeofResultSystemStep, MPI_BYTE, 0, RESULT, MPI_COMM_WORLD, &(requestSend[0]) );
	DPRINTF( "... .../%d. slave (%d) MPI_Isend(0:oRESULTS) 1 ... \n", ++cnt, myrank );

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::run_while_irecv_tag_data( ) {
	DPRINTF( "... .../%d. slave (%d) status.MPI_TAG == DATA (k,m=%d,%d)\n", ++cnt, myrank, k, m );
	oELEM_[0] = oELEM_[1];
	DPRINTF( "... .../%d. slave (%d) IRecv(0:oELEM) 2 (k,m=%d,%d)\n", ++cnt, myrank, k, m );
	MPI_Irecv( &oELEM_[1], sizeof( TE ), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &requestReceive[0] );
	DPRINTF( "... .../%d. slave (%d) IRecv(e:oELEM) 2 (k,m=%d,%d)\n", ++cnt, myrank, k, m );
	DPRINTF("WHILE SLAVE (%d) :: k=%d, m=%d, id=%d/%d\n",myrank,k,m,oELEM_[0].idElem,oELEM_[1].idElem);

}

template MPI_SLAVE_TPACK void MpiSlave<MPI_SLAVE_T>::run_while_irecv_tag_finish( ) {
	DPRINTF( "... .../%d. slave (%d) status.MPI_TAG == FINISH (k,m=%d,%d)\n", ++cnt, myrank, k, m );
	DPRINTF( "... .../%d. slave (%d) FINALIZE (k,m=%d,%d)\n", ++cnt, myrank, k, m );
}


template MPI_SLAVE_TPACK bool MpiSlave<MPI_SLAVE_T>::run_while_irecv( ) {

	//DPRINTF( "... .../%d. slave (%d) MPI_Test(0:flag=%d,status.tag=%d) 1 (k,m=%d,%d)\n", ++cnt, myrank, flag, status.MPI_TAG, k, m );
	MPI_Test( &requestReceive[0], &flag, &status );
	//DPRINTF( "... .../%d. slave (%d) MPI_Test(e:flag=%d,status.tag=%d) 1 (k,m=%d,%d)\n", ++cnt, myrank, flag, status.MPI_TAG, k, m );

	if (flag == 1) {//means we have a new data to receive,NoneFinishFlag means should I stop receive data
		DPRINTF( "... .../%d. slave (%d) CHECK FLAG 2 : flag == 1 (k,m=%d,%d)\n", ++cnt, myrank, k, m );
		DPRINTF( "... .../%d. slave (%d) MPI_WAIT(0:reqRecv[0]) 2 (k,m=%d,%d)\n", ++cnt, myrank, k, m );
		MPI_Wait( &requestReceive[0], MPI_STATUS_IGNORE );
		DPRINTF( "... .../%d. slave (%d) MPI_WAIT(e:reqRecv[0]) 2 (k,m=%d,%d)\n", ++cnt, myrank, k, m );

		DPRINTF( "... .../%d. slave (%d) IRecved : oELEM[%d].z0=%f\n", ++cnt, myrank, oELEM_[1].idElem, oELEM_[1].z0 );
		DPRINTF( "... .../%d. slave (%d) CHECK STATUS : status.MPI_TAG=%d (k,m=%d,%d)\n", ++cnt, myrank, status.MPI_TAG, k, m );
		if (status.MPI_TAG == DATA) {
			run_while_irecv_tag_data();
		} else if (status.MPI_TAG == FINISH) {
			run_while_irecv_tag_finish();
			return true;

		}
	}

	return false;
}


template class MpiSlave<MPI_PACK>;