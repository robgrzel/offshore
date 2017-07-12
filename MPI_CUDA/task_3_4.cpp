//
// Created by robgrzel on 23.06.17.
//



#include "OWave.h"
#include "OWaves.h"
#include "OWavenumber.h"
#include "OCylinderFloating.h"
#include "OMorisonEquation.h"
#include "OSeas.h"
#include "OSysObjEnv.h"
#include "Cases.h"

typedef std::numeric_limits<double> maxdbl;


int cases_buoy_no_wave() {
	AFloatingCylinder b1;
	AFloatingCylinder b2;
	AFloatingCylinder b3;
	AFloatingCylinder b4;
	
	b1.z0 = 0;
	b2.z0 = 1;
	b3.z0 = 0;
	b3.b33 = 1000;
	b4.z0 = 1;
	b4.b33 = 1000;
	
	
	b1.casename += "_inEquilibrium_noWave_noDamp";
	b2.casename += "_noEquilibrium_noWave_noDamp";
	b3.casename += "_inEquilibrium_noWave_onDamp";
	b4.casename += "_noEquilibrium_noWave_onDamp";
	
	AResults_t<AResultAll_t> R1(b1.dt, b1.t0, b1.t1, b1.casename);
	AResults_t<AResultAll_t> R2(b2.dt, b2.t0, b2.t1, b2.casename);
	AResults_t<AResultAll_t> R3(b3.dt, b3.t0, b3.t1, b3.casename);
	AResults_t<AResultAll_t> R4(b4.dt, b4.t0, b4.t1, b4.casename);
	
	std::cout << "################################\n";
	buoy_floating_no_wave(b1, R1);
	std::cout << "################################\n";
	buoy_floating_no_wave(b2, R2);
	std::cout << "################################\n";
	buoy_floating_no_wave(b3, R3);
	std::cout << "################################\n";
	buoy_floating_no_wave(b4, R4);
}

int cases_buoy_on_wave() {
	AFloatingCylinder b1;
	AFloatingCylinder b2;
	AFloatingCylinder b3;
	AFloatingCylinder b4;
	
	b1.z0 = 0;
	b1.dt = 0.005;
	b2.z0 = 1;
	b2.dt = 0.005;
	b2.t1 = 500;
	b3.z0 = 0;
	b3.dt = 0.005;
	b3.b33 = 1000;
	b4.z0 = 1;
	b4.b33 = 1000;
	b4.dt = 0.005;
	
	b1.casename += "_inEquilibrium_onWave_noDamp";
	b2.casename += "_noEquilibrium_onWave_noDamp";
	b3.casename += "_inEquilibrium_onWave_onDamp";
	b4.casename += "_noEquilibrium_onWave_onDamp";
	
	AResults_t<AResultAll_t> R1(b1.dt, b1.t0, b1.t1, b1.casename);
	AResults_t<AResultAll_t> R2(b2.dt, b2.t0, b2.t1, b2.casename);
	AResults_t<AResultAll_t> R3(b3.dt, b3.t0, b3.t1, b3.casename);
	AResults_t<AResultAll_t> R4(b4.dt, b4.t0, b4.t1, b4.casename);
	
	std::cout << "################################\n";
	buoy_floating_on_wave(b1, R1);
	std::cout << "################################\n";
	buoy_floating_on_wave(b2, R2);
	std::cout << "################################\n";
	buoy_floating_on_wave(b3, R3);
	std::cout<<"################################\n";
	buoy_floating_on_wave(b4, R4);
}



struct AElem {
	double xc = 0;
	double yc = 0;
	double zc = 0;
	double dxc = 1;
	double dyc = 1;
	double dzc = 1;
};


void cases_monopile_stuck_in_seabed() {
	AMonopile A;
	
	AResults_t<AResultForcesMoments_t> R(A.dt, A.t0, A.t1, A.casename);
	
	monopile_stuck_in_seabed_on_wave(A, R);
	
}


int task_3_4() {
	
	cases_monopile_stuck_in_seabed();
	//cases_buoy_on_wave();
}