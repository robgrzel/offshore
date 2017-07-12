//
// Created by robgrzel on 12.06.17.
//
#include "OWaves.h"

int main(){
	double diameter    = 2;
	double draught     = 4;
	double height      = 6;
	double depth       = 1000;
	double vx          = -1;
	double vy          = 0;
	double phi         = 0;
	double z0          = 1.;
	double coefDrag2   = 0.82;
	double damp33      = 1000;
	double coefAddmass = 0.2;
	double Grav        = 9.81;
	double Rho         = 1025.;
	double Hs          = 6.0;
	double Tp          = 11.5;
	double t0          = 0;
	double t1          = 3600;
	double dt          = 0.005;

	int numSteps = int( (t1 - t0) / dt );
	int numElems = int( 1e1 );
	int numWaves = int(2.995 * t1);


	OWavesJonswap oWaves = OWavesJonswap(t1,Hs,Tp,1/dt,"seas",OWavesJonswap::Brodtkorb);

	oWaves.save_data();

	//std::vector<AWaveTime_t> AWavesT;
	std::vector<AWavePuls_t> AWavesP;

	oWaves.swap_vec_wave_puls(AWavesP);
	//oWaves.swap_vec_wave_time(AWavesT);

	printf("AWavesP.size=%zu\n",AWavesP.size());



}