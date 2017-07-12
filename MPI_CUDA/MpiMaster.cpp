//
// Created by robgrzel on 02.06.17.
//

#include "MpiMaster.h"

#define DEBUG

#define PRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)

#ifdef DEBUG
	#define DPRINTF( format, ... ) fprintf (stdout, format, ##__VA_ARGS__)
#else
	#define DPRINTF( format, ... )
#endif

template MPI_MASTER_TPACK MpiMaster<MPI_MASTER_T>::~MpiMaster( ){
		delete oELEMS.e;
		delete oWAVES.w;
		//oRESULTS=NULL;
		//oSTRUCT=NULL;
}


template MPI_MASTER_TPACK MpiMaster<MPI_MASTER_T>::MpiMaster MPI_MASTER_ARGS :
		oRESULTS( myrank, oSTRUCT.numElems, numSteps, numWaves, t0, t1  ),
		oWAVES( myrank, numWaves ),
		oELEMS( myrank,oSTRUCT.numElems ),
		oSTRUCT( oSTRUCT),
		myrank( myrank ),
		proccount( proccount ){


	DPRINTF( "MASTER START ### ###\n" );

	if (!oRESULTS.results) {
		DPRINTF( "\nNot enough memory" );
		MPI_Finalize();
		return;

	} else {

		init();

		send_const();
		send_idem();
		isend_idem();

		irecv_results();

		run_while();

		after_while();

		check_results();

		kill_slaves();

		DPRINTF( "MASTER FINALIZE...\n" );
		check_results();
		recv_orphans();

		check_results();

		DPRINTF( "MASTER END...\n" );

		return;

	}

	return;
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::init( ) {

	DPRINTF( "--- --->%d. master init 0\n", ++cnt );


	requestSend = ( MPI_Request* ) malloc( (proccount - 1) * sizeof( MPI_Request ) );

	requestReceive = ( MPI_Request* ) malloc( (proccount - 1) * sizeof( MPI_Request ) );

	for ( int i = 0; i < (proccount - 1); i++ ) {
		requestReceive[i] = MPI_REQUEST_NULL;
	}


	oSTRUCT.gen( oWAVES );
	oSTRUCT.gen( oELEMS );

	DPRINTF( "--- --->%d. master init e\n", ++cnt );

}


template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::send_const( ) {
	DPRINTF( "--- --->%d. master send const 0\n", ++cnt );
	for ( int i = 1; i < proccount; i++ ) {
		DPRINTF( "--- --->%d. master send const 1 : sizeofWavesMpi=%d\n", ++cnt, oWAVES.sizeofWavesMpi );

		MPI_Send( &(oWAVES), oWAVES.sizeofWavesMpi, MPI_BYTE, i, DATA, MPI_COMM_WORLD );
		DPRINTF( "--- --->%d. master send const 2 : sizeofWaveNum=%d\n", ++cnt, oWAVES.sizeofWaveNum );
		MPI_Send( &(oWAVES.w[0]), oWAVES.sizeofWaveNum, MPI_BYTE, i, DATA, MPI_COMM_WORLD );
		DPRINTF( "--- --->%d. master send const 3\n", ++cnt );
		MPI_Send( &(oSTRUCT), sizeof( TS ), MPI_BYTE, i, DATA, MPI_COMM_WORLD );
	}
	DPRINTF( "--- --->%d. master send const e\n", ++cnt );
}


template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::send_idem( ) {
	for ( i = 1; i < proccount; i++ ) {

		oELEM = oELEMS.e[i - 1];

		DPRINTF( "--- --->%d. master send 0 ::: to = %d, oELEM[idElem=%d]=%f\n", ++cnt, i, oELEM.idElem, oELEM.z0 );

		MPI_Send( &(oELEM), oELEMS.sizeof_2, MPI_BYTE, i, DATA, MPI_COMM_WORLD );

		sentcount++;
	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::isend_idem( ) {
	DPRINTF( "--->%d. master isend 1\n", ++cnt );

	for ( int i = 1; i < proccount; i++ ) {//Process from 1~~proccount-1
		oELEM = oELEMS.e[sentcount];
		DPRINTF( "--- --->%d. master isend 1 ::: to = %d, oELEM[idElem=%d]=%f\n", ++cnt, i, oELEM.idElem, oELEM.z0 );

		MPI_Isend( &oELEM, oELEMS.sizeof_2, MPI_BYTE, i, DATA, MPI_COMM_WORLD, &(requestSend[i - 1]) );
		sentcount++;
	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::irecv_results( ) {

	for ( int i = 1; i < proccount; i++ ) {//Process from 1~~proccount-1

		DPRINTF( "--- ---<%d. master irecv 1 ::: from=%d, sizeof=%d\n", ++cnt, i, oRESULTS.sizeofResultSystemStep );

		MPI_Irecv( &(oRESULTS.results[i - 1][0]), oRESULTS.sizeofResultSystemStep, MPI_BYTE, i, RESULT, MPI_COMM_WORLD, &(requestReceive[i - 1]) );

	}

}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::run_while_waits( ) {

	DPRINTF( "--- ---%d. master MPI_Waitany(0)\n", ++cnt );
	MPI_Waitany( proccount - 1, requestReceive, &requestIndex, MPI_STATUS_IGNORE );
	DPRINTF( "--- ---%d. master MPI_Waitany(e: requestReceive from=%d)\n", ++cnt, requestIndex + 1 );

	//oRESULTS.integrate(oELEMS,requestIndex);//todo

	DPRINTF( "--- ---%d. master MPI_Wait(0:request send : sentcnt=%d, to=%d)\n", ++cnt, sentcount, requestIndex + 1 );
	MPI_Wait( &requestSend[requestIndex], MPI_STATUS_IGNORE );
	DPRINTF( "--- ---%d. master MPI_Wait(e:request send : sentcnt=%d, to=%d)\n", ++cnt, sentcount, requestIndex + 1 );



	//result += resultTemp[requestIndex];
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::run_while_isend_check_anylast( ) {
	DPRINTF( "WHILE MASTER : sentcount == oELEMS.numElems / sentcount, oELEMS.numElems\n", sentcount == oELEMS.numElems, sentcount, oELEMS.numElems );
	if (sentcount == oELEMS.numElems) {
		oELEM.idElem = -1;
	} else {
		oELEM = oELEMS.e[sentcount];
		sentcount++;

	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::run_while_icomm( ) {


	DPRINTF( "--- --->>>%d. master MPI_Isend(0:oELEM oELEM[idElem=%d]=%f)\n", ++cnt, oELEM.idElem, oELEM.z0 );
	MPI_Isend( &oELEM, oELEMS.sizeof_2, MPI_BYTE, requestIndex + 1, DATA, MPI_COMM_WORLD, &requestSend[requestIndex] );
	DPRINTF( "--- --->>>%d. master MPI_Isend(e:oELEM)\n", ++cnt );

	DPRINTF( "--- ---<<<%d. master 2 MPI_Irecv(o : recvcnt=%d, prev orss.id = %d) 2\n",
	         ++cnt, proccount - 1 + recvcnt, oRESULTS.results[proccount - 1 + recvcnt][0].elemId );
	MPI_Irecv( &(oRESULTS.results[proccount - 1 + recvcnt][0]), oRESULTS.sizeofResultSystemStep,
	           MPI_BYTE, requestIndex + 1, RESULT, MPI_COMM_WORLD,
	           &(requestReceive[requestIndex])
	         );
	DPRINTF( "--- ---<<<%d. master 2 MPI_Irecv(e) 2\n", ++cnt );
	recvcnt++;
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::run_while( ) {

	do {
		DPRINTF( "---%d. MASTER BEGIN WHILE LOOP 1 (oELEM.idElem %d <= %d NUM_OF_ELEMENTS - 1) --- --->\n",
		         ++cnt, oELEM.idElem, oELEMS.numElems - 1 );

		run_while_waits();
		run_while_isend_check_anylast();
		run_while_icomm();


	} while ( oELEM.idElem != -1 );

	check_results();

	DPRINTF( "---%d. MASTER WHILE END...\n", ++cnt );


}
#define MPI_MASTER_TPACK <class TRs, class TR, class TWs, class TW,  class TEs, class TE, class TS>

#define MPI_MASTER_T TRs,  TR,  TWs,  TW,   TEs,  TE, TS
template  MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T> ::after_while( ) {

	MPI_Waitall( 1 * (proccount - 1), requestReceive, MPI_STATUS_IGNORE );


	for ( int i = 0; i < proccount - 1; i++ ) {
		recvcnt++;
		//result += resultTemp[i];
	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::kill_slaves( ) {
	//shut down the slave
	//Maybe I should send special number

	oELEM.idElem = -1;

	for ( i = 1; i < proccount; i++ ) {
		MPI_Send( &oELEM, oELEMS.sizeof_2, MPI_BYTE, i, FINISH, MPI_COMM_WORLD );
		MPI_Send( NULL, 0, MPI_DOUBLE, i, FINISH, MPI_COMM_WORLD );
	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::check_results( ) {
	PRINTF( "MASTER cnt: recv/sent=%d/%d//%d\n", recvcnt, sentcount, sentcount - recvcnt );
	for ( int i = 0; i < sentcount; i++ ) {
		PRINTF( "<<<<<%d. master 1 MPI_Irecv(0:oRESULTS.results[%d] : myrank=%d, idElem=%d Fe=%f) 1\n",
		        ++cnt, i, oRESULTS.results[i][10].myrank,
		        oRESULTS.results[i][10].elemId,
		        oRESULTS.results[i][10].F );

	}
}

template MPI_MASTER_TPACK void MpiMaster<MPI_MASTER_T>::recv_orphans( ) {
	while ( recvcnt < sentcount ) {
		DPRINTF( "--- ---<%d. master 3 MPI_Recv(0) 3 : recv=%d, sent=%d\n", ++cnt, recvcnt, sentcount );
		MPI_Recv( &(oRESULTS.results[recvcnt][0]), oRESULTS.sizeofResultSystemStep, MPI_BYTE, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status );
		DPRINTF( "--- ---<%d. master 3 MPI_Recv(e: oRESULTS.results[%d] : id=%d z0=%f) 3\n", ++cnt, recvcnt, oRESULTS.results[recvcnt][0].elemId,
		         oRESULTS.results[recvcnt][0].myrank );
		recvcnt++;
	}

}

template class MpiMaster<MPI_PACK>;
//template class MpiMaster<OResultSystem_t, OResultSystemStep_t, OWavesData_t, OWaveData_t, OCylinderFloatingData_t, OElems_t, OElem_t>;