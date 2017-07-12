//
// Created by robgrzel on 03.07.17.
//

#ifndef EXAM_PHYS_H_H
#define EXAM_PHYS_H_H

#include "math.h"

double OC[7] = {0, 5, 10, 15, 20, 35, 30};

double RHO[3][7] = {
		{999.8, 1000,   999.7,  999.1,  998.2,  997,    995.6},
		{1028., 1027.6, 1026.9, 1025.9, 1024.7, 1023.2, 1021.7},
		{1.293, 1.270,  1.247,  1.226,  1.205,  1.184,  1.165}
};

double KVC[3][7] = {
		{1.79, 1.52, 1.31, 1.14, 1,    0.89, 0.8},
		{1.83, 1.56, 1.35, 1.19, 1.05, 0.94, 0.85},
		{1.32, 1.36, 1.41, 1.45, 1.50, 1.55, 1.6}
};

enum env_type {
	freshWater = 0,
	saltWater = 1,
	dryAir = 2,
};

int err;

double a[10];


double oC_by_Rho(double Rho, env_type e) {
	//Recommended Practice DNV-RP-H103, April 2011
	//APPENDIX C Physical Constants – Page 149
	//Table C-1 Density and viscosity of fresh water, sea water and dry air
	double oC;
	
	
	if (Rho < RHO[e][0]) {
		oC = OC[0];
		
	} else if (Rho > RHO[e][6]) {
		oC = OC[6];
		
	} else {
		const int o = 4;
		err = polyfit(RHO[e], OC, 7, o, a);
		
		oC = dround(calc_poly(o + 1, Rho, a), 2);
	}
	return oC;
	
};

double Kvc_by_Rho(double Rho, env_type e) {
	//Recommended Practice DNV-RP-H103, April 2011
	//APPENDIX C Physical Constants – Page 149
	//Table C-1 Density and viscosity of fresh water, sea water and dry air
	double Kvc;
	
	if (Rho < RHO[e][0]) {
		Kvc = KVC[e][0];
		
	} else if (Rho > RHO[e][6]) {
		Kvc = KVC[e][6];
		
	} else {
		const int o = 6;
		err = polyfit(RHO[e], KVC[e], 7, o, a);
		Kvc = dround(calc_poly(o + 1, Rho, a), 2) / 1e6;
	}
	
	return Kvc;
	
};

double Rho_by_oC(double oC, env_type e) {
	//Recommended Practice DNV-RP-H103, April 2011
	//APPENDIX C Physical Constants – Page 149
	//Table C-1 Density and viscosity of fresh water, sea water and dry air
	double Rho;
	
	if (oC < OC[0]) {
		Rho = RHO[e][0];
		
	} else if (oC > OC[6]) {
		Rho = RHO[e][6];
		
	} else {
		const int o = 5;
		err = polyfit(OC, RHO[e], 7, o, a);
		
		Rho = dround(calc_poly(o + 1, oC, a), 2);
	}
	return Rho;
	
};

double Kvc_by_oC(double oC, env_type e) {
	//Recommended Practice DNV-RP-H103, April 2011
	//APPENDIX C Physical Constants – Page 149
	//Table C-1 Density and viscosity of fresh water, sea water and dry air
	double Kvc;
	
	if (oC < OC[0]) {
		Kvc = KVC[e][0];
		
	} else if (oC > OC[6]) {
		Kvc = KVC[e][6];
		
	} else {
		const int o = 5;
		err = polyfit(OC, KVC[e], 7, o, a);
		
		Kvc = dround(calc_poly(o + 1, oC, a), 2) / 1e6;
	}
	
	return Kvc;
	
};

double seawater_density(double & Rho, double & oC){
	if (Rho == -1 and oC == -1) {
		oC = 20;
		Rho = Rho_by_oC(oC, env_type::saltWater);
	} else if (Rho == -1 and oC != -1) {
		Rho = Rho_by_oC(oC, env_type::saltWater);
	} else if (Rho != -1 and oC == -1) {
		oC = oC_by_Rho(Rho, env_type::saltWater);
	}
	
	return Rho;
}

double seawater_kinematic_viscosity(double & Kvc, double & oC){
	if (Kvc == -1 and oC == -1) {
		oC = 20;
		Kvc = Kvc_by_oC(oC, env_type::saltWater);
	} else if (Kvc == -1 and oC != -1) {
		Kvc = Kvc_by_oC(oC, env_type::saltWater);
	} else if (Kvc != -1 and oC == -1) {
		oC = oC_by_Rho(Kvc, env_type::saltWater);
	}
	
	return Kvc;
}

void seawater_params(double & Rho, double & Kvc, double & oC){
	
	seawater_density(Rho,oC);
	seawater_kinematic_viscosity(Kvc,oC);
	
}



#endif //EXAM_PHYS_H_H
