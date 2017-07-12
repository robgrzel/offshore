//
// Created by robgrzel on 30.06.17.
//

#ifndef EXAM_CASEMONOPILEINSEABED_H
#define EXAM_CASEMONOPILEINSEABED_H


#include "UtilSources/Math/env.h"

struct ACylinderCase{
	
	struct_case_types caseType = verticalMonopile;
	wave_types waveType = regularWaves;
	
	double diameter;
	double draft0;
	double depth;
	double dzc;
	
	double Rho;
	double Grav;
	double oC;
	
	
	
};

struct AMonopile {
	
	double normal[3] = {1, 0, 0};
	
	double diameter = 6.5;
	double draft0 = 40;
	double Depth = 40;
	double dzc = 10;
	double ddraft0 = 5;
	
	double Rho = 1025;
	double Grav = 9.81;
	
	double Amp = 4.5;
	double Tp = 11.3;
	double Hs = 9;
	
	double Dvc = 1.08 / 1e3;
	
	double t0 = 0;
	double t1 = 3600;
	double dt = 0.005;
	double x = 0;
	double y = 0;
	
	size_t numElems = 10;
	
	std::string casename = "MonopileStuckSeabed";
	
	
};

struct AFloatingCylinder {
	
	double diameter = 2;
	double draft0 = 4;
	double height = 6;
	double Ca = 0.2;
	double Cd2 = 0.82;
	double b33 = 0;
	double Rho = 1025;
	double Grav = 9.81;
	double Dvc = 1.08 / 1e3;
	double dt = 0.01;
	double t1 = 50;
	double t0 = 0;
	double z0 = 0;
	double dotz0 = 0;
	
	double Depth = 1000;
	double H = 1;
	double Amp = 0.5;
	double T = 3;
	double Om = 2 * PI / T;
	double k = pow(Om, 2) / Grav;
	
	double x = 0;
	double y = 0;
	
	double Ucurrent[2] = {0, 0};
	double Uwind[2] = {0, 0};
	
	std::string casename = "FloatingCylinder";
	
};


#endif //EXAM_CASEMONOPILEINSEABED_H
