//
// Created by robgrzel on 12.05.17.
//

#include <cmath>
#include "OWave.h"



double  OWave::dispersion_relation( ) {
	c = sqrt( Grav * k * tanh( k * depth ) );\
	return c;
};

double  OWave::wave_phase_speed( ) {
	Cp = sqrt( Grav / k * tanh( k * depth ) );
	return Cp;
};

double  OWave::wave_group_speed( ) {
	Cg = 0.5 * Cp * (1 + k * depth * (1 - pow( tanh( k * depth ), 2 )) / tanh( k * depth ));
	return Cg;
};

double  OWave::wave_group_phase_ratio( ) {
	Cg_Cp = Cg / Cp;
	return Cg_Cp;
};



double  OWave::phase_speed( ) {
	Cp = sqrt( Grav / k * tanh( k * depth ) );
	return Cp;
};

double  OWave::wave_elevation( ) {
	Eta = amp * cos( Theta );
	return Eta;
}

double  OWave::plane_wavenumber(double x, double y){
	kx = k * x * cos(alpha) - k * y*sin(alpha);
	return kx;
}

double OWave::wave_phase( ) {
	Theta = kx - om * t - eps;
	if (i<10) printf("i=%d, Theta=%f, kx=%f, om=%f, t=%f, eps=%f\n",i,Theta,kx,om,t,eps);
	return Theta;
}


