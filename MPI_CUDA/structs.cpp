//
// Created by robgrzel on 04.06.17.
//
#include <vector>
#include "structs.h"



AWaves_t::~AWaves_t( ) {
	PRINTF( "del AWaves_t (0,rank=%d, numWaves=%d, empty=%d) \n", myrank, numWaves, w );
	if (w != nullptr) {
		free1d( &w ); //segefault when master do : return;
		w = nullptr;
	}

	PRINTF( "del AWaves_t (e,rank=%d, numWaves=%d, empty=%d) \n", myrank, numWaves, 0 );

}

AWaves_t::AWaves_t( int myrank, int numWaves ) : myrank( myrank ), numWaves( numWaves ) {
	sizeofWaveNum *= numWaves;
	PRINTF( "AWaves_t (0,rank=%d, numWaves=%d, empty = %d) \n", myrank, numWaves, w );
	if (w == nullptr) {
		malloc1d( &w, numWaves );

	}
	PRINTF( "AWaves_t (1,rank=%d, numWaves=%d, empty = %d) \n", myrank, numWaves, w );

}


AElems_t::~AElems_t( ) {
	PRINTF( "del AElems_t (0,rank=%d, numElems=%d, empty=%d) \n", myrank, numElems, e );
	if (e != nullptr) {
		free( &e );
		e = nullptr;
	}
	PRINTF( "del AElems_t (e,rank=%d, numElems=%d, empty=%d) \n", myrank, numElems, e );

}

AElems_t::AElems_t( int myrank, int numElems ) : myrank( myrank ), numElems( numElems ) {
	PRINTF( "AElems_t (rank=%d, numElems=%d, empty=%d) \n", myrank, numElems, e );
	if (e == nullptr) {
		malloc1d( &e, numElems );
	}
	PRINTF( "AElems_t (rank=%d, numElems=%d, empty=%d) \n", myrank, numElems, e );
}

void ACylinderFloating_t::gen( AWaves_t& W ) {
	PRINTF( "AWaves_t.gen : numWaves=%d\n", W.numWaves );

	printf("tmax=%f,Hs=%f,Tp=%f,fsamp=%f,seas=%s,OWavesJonswap::Brodtkorb=%d\n",
	       tmax,Hs,Tp,fsamp,"seas",OWavesJonswap::Brodtkorb);
	
	OWavesJonswap oWaves = OWavesJonswap(tmax,Hs,Tp,fsamp,"seas",OWavesJonswap::Brodtkorb);

	oWaves.save_data();

	//std::vector<AWaveTime_t> AWavesT;
	std::vector<AWavePuls_t> AWavesP;

	oWaves.swap_vec_wave_puls(AWavesP);
	//oWaves.swap_vec_wave_time(AWavesT);

	printf("AWavesP.size=%zu\n",AWavesP.size());


	W.w = AWavesP.data();

}

void ACylinderFloating_t::gen( AElems_t& E ) {
	PRINTF( "ACylinderFloating_t.gen\n" );

	E.e[0].z0 = z0 + dz0 / 2.;
	for ( int i = 1; i < numElems; i++ ) {
		E.e[i].z0 = E.e[i - 1].z0 + dz0;
	}

	for ( int i = 0; i < numElems; i++ ) {
		E.e[i].h      = dz0;
		E.e[i].m      = massInit / numElems;
		E.e[i].idElem = i;
		E.e[i].dotz0  = 0;

		if (E.e[i].z0 < dz0 / 2) {
			E.e[i].d = dz0 / 2.;
		} else { E.e[i].d = 0; }
	}


}

ACylinderFloating_t::ACylinderFloating_t( int myrank, int numElems ) : myrank( myrank ), numElems( numElems ) {
	PRINTF( "ACylinderFloating_t (rank=%d) \n", myrank );

}

ACylinderFloating_t::ACylinderFloating_t(
		int myrank, int numElems, double dt, double diameter,
		double draught, double height,
		double depth, double vx, double vy, double phi,
		double z0, double coefDrag, double damp33,
		double coefAddmass, double Grav, double Rho,
		double tmax, double Hs, double Tp

) : myrank( myrank ), numElems( numElems ), dt( dt ), draught( draught ), height( height ), diameter( diameter ),
    depth( depth ), vx( vx ), vy( vy ), phi( phi ), z0( z0 ), coefDrag( coefDrag ),
    damp33( damp33 ), coefMass( 1 + coefAddmass ), coefAddmass( coefAddmass ), Grav( Grav ), Rho( Rho ),
	tmax(tmax), Hs(Hs), Tp(Tp), fsamp(1/dt)
{

	dz0 = height / numElems;

	areaWaterline = PI / 4 * pow( diameter, 2 );

	areaProjected = areaWaterline;

	massInit = massDisp = areaWaterline * Rho * draught;

}


