//
// Created by robgrzel on 19.05.17.
//

#include <iostream>
#include "OParticle.h"


double * OParticle::excursion_amp(double z, double draft0) {
	//if z = xc : draft0 = 0, else draft0>0
	ua[0] = amp * cosh(k * (z -draft0 + depth)) / sinh(k*depth); //hor
	ua[1] = amp * sinh(k * (z -draft0 + depth)) / sinh(k*depth); //vert
	return ua;
}


double * OParticle::velocity_amp() {
	dotua[0] = om * ua[0];//hor
	dotua[1] = om * ua[1];//ver
	return dotua;
}

double * OParticle::acceleration_amp() {
	ddotua[0] = om * dotua[0];//h
	ddotua[1] = om * dotua[1];//v
	return ddotua;
}

double OParticle::dynamical_pressure_amplitude() {
	
	Pda = -Rho * ddotua[1];
	return Pda;
}

double OParticle::potential_amplitude() {
	
	PHIa = ua[0] / k;
	return PHIa;
}

double * OParticle::excursion(double Theta) {
	
	u[0] = -Ek[0] * ua[0] * sin(Theta);//x
	u[1] = -Ek[1] * ua[0] * sin(Theta);//y
	u[2] = ua[1] * cos(Theta);//v
	
	return u;
};

double * OParticle::velocity(double Theta) {
	dotu[0] = Ek[0] * dotua[0] * cos(Theta);//x
	dotu[1] = Ek[1] * dotua[0] * cos(Theta);//y
	dotu[2] = dotua[1] * sin(Theta);//z
	return dotu;
};

double * OParticle::acceleration(double Theta) {
	ddotu[0] = Ek[0] * ddotua[0] * sin(Theta);//x
	ddotu[1] = Ek[1] * ddotua[0] * sin(Theta);//y
	ddotu[2] = -ddotua[1] * cos(Theta);//z
	return ddotu;
	
};

double OParticle::statical_pressure(double z, double dz2b) {
	//dz2b = dist from z to bottom
	Ps = -Rho * Grav * (z - dz2b);
	return Ps;
}

double OParticle::dynamical_pressure(double Theta) {
	//printf("Pd=%f,Pda=%f,cTheta=%f\n",Pd,Pda,cTheta);
	Pd = Pda * cos(Theta);
	return Pd;
};

double OParticle::pressure() {
	P = Pd + Ps;
	//printf("P=%f,Pd=%f,Ps=%f\n");
	return P;
}

double OParticle::potential(double Theta) {
	PHI = -PHIa * sin(Theta);
	return PHI;
}


