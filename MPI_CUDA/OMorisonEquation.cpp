//
// Created by robgrzel on 27.06.17.
//

#include "OMorisonEquation.h"

//
// Created by robgrzel on 12.05.17.
//

#include "OMorisonEquation.h"
#include "UtilSources/Math/const.h"
#include "UtilSources/template_utils.h"
#include "UtilSources/Math/math.h"

//#define DEBUG


double OMorisonEquation::displaced_vol() {
	volume = areaWaterline * draft;
	return volume;
}

double OMorisonEquation::Draft(double zeta) {
	draft = zeta + draft0 - x[2];
	draft = draft >= height ? height : draft;
	
	
	return draft;
}

double OMorisonEquation::displaced_mass() {
	mass = volume * Rho;
	return mass;
};

double OMorisonEquation::area_of_waterline() {
	areaWaterline = PI / 4 * pow(diameter, 2);
	return areaWaterline;
};

double OMorisonEquation::line_of_waterline() {
	lineWaterline = PI / 4 * diameter;
	return lineWaterline;
};

double * OMorisonEquation::line_projected() {
	lineProjected[0] = PI / 4 * diameter;
	lineProjected[1] = diameter;
	return lineProjected;
}

double * OMorisonEquation::area_projected() {
	areaProjected[0] = PI / 4 * pow(diameter, 2);
	areaProjected[1] = draft * diameter;
	return areaProjected;
};

double * OMorisonEquation::dmass_projected() {
	dmassProjected[0] = Rho * lineProjected[0];
	dmassProjected[1] = Rho * lineProjected[1];
	return dmassProjected;
}

double * OMorisonEquation::mass_projected() {
	massProjected[0] = Rho * areaProjected[0];
	massProjected[1] = Rho * areaProjected[1];
	return massProjected;
}


double OMorisonEquation::Bas_sub() {
	nsubs = height - draft0 > 0 ? 1 : 2;
	return nsubs;
}


void OMorisonEquation::Params() {
	area_projected();
	mass_projected();
	area_of_waterline();
	displaced_mass();
};


double * OMorisonEquation::Accel() {
	ddotx[0] = (fdamp[0] + fdrag[0] + finer[0] + frest[0]) / (Cm * mass);
	ddotx[1] = (fdamp[1] + fdrag[1] + finer[1] + frest[1]) / (Cm * mass);
	ddotx[2] = (fdamp[2] + fdrag[2] + finer[2] + frest[2] + fwavez) / (Cm * mass);
	#ifdef DEBUG
	if (i<5) printf("ddotx=[%f,%f,%f], forces[%f,%f,%f],  faddm[%f,%f,%f], Cm[%f], mass0[%f]\n",
					ddotx[0],ddotx[1],ddotx[2],forces[0],forces[1],forces[2],faddm[0],faddm[1],faddm[2],Cm,mass0);
	#endif
	return ddotx;
}

double * OMorisonEquation::Veloc() {
	/*
	dt = time step
	dotZi = buoy velocity current step
	dotZj = buoy Veloc next step
	ddotZj = buoy Accel next step
	*/
	//dotX[i3+2] = dotX[j3+2];
	dotxj[0] = dotx[0];
	dotxj[1] = dotx[1];
	dotxj[2] = dotx[2];
	
	dotx[0] += ddotx[0] * dt; //dotZj
	dotx[1] += ddotx[1] * dt; //dotZj
	dotx[2] += ddotx[2] * dt; //dotZj
	#ifdef DEBUG
	if (i<5)
	printf("dotx=[%f,%f,%f], dotxj=[%f,%f,%f], ddotx=[%f,%f,%f], dt=%f\n",
		   dotx[0], dotx[1], dotx[2], dotxj[0], dotxj[1], dotxj[2],
		   ddotx[0], ddotx[1], ddotx[2], dt);
	#endif
	
	return dotx;
}


double * OMorisonEquation::Displ() {
	/*
	dt = time step
	Zi = buoy Displ current step
	Zj = buoy displacament next step
	dotZi = buoy velocity current step
	dotZj = buoy Veloc next step
	*/
	
	x[0] += 0.5 * (dotx[0] + dotxj[0]) * dt; //Zj
	x[1] += 0.5 * (dotx[1] + dotxj[1]) * dt; //Zj
	x[2] += 0.5 * (dotx[2] + dotxj[2]) * dt; //Zj
	
	return x;
}


double OMorisonEquation::Fwave(double Eta) {
	fwavez = Rho * Grav * areaWaterline * Eta;
	return fwavez;
}


double * OMorisonEquation::Fdamp() {
	fdamp[0] = -b11 * dotx[0];
	fdamp[1] = -b22 * dotx[1];
	fdamp[2] = -b33 * dotx[2];
	return fdamp;
};

double * OMorisonEquation::Frest() {
	//Buoyancy force
	frest[0] = Grav * massProjected[1] * (-x[0]);
	frest[1] = Grav * massProjected[1] * (-x[1]);
	frest[2] = Grav * massProjected[0] * draft;
	#ifdef DEBUG
	if (i<5)
	printf("frest=%f,%f,%f, massP=%f,%f, x=%f,%f,%f\n",frest[0],frest[1],frest[2],massProjected[0],massProjected[1],x[0],x[1],x[2]);
	#endif
	
	return frest;
};


double * OMorisonEquation::Flift(double dotu, double dotv, double dotw) {
	//Cl = 1.3*Cd;
	qlift[0] = 0.5 * massProjected[1] * 1.3 * Cd * (dotu - dotx[0]) * fabs(dotu - dotx[0]);
	qlift[1] = 0.5 * massProjected[1] * 1.3 * Cd * (dotv - dotx[1]) * fabs(dotv - dotx[1]);
	qlift[2] = 0.5 * massProjected[0] * 1.3 * Cd * (dotw - dotx[2]) * fabs(dotw - dotx[2]);
	
};

double * OMorisonEquation::Fslam(double dotu, double dotv, double dotw) {
	//Cs = PI;
	fslam[0] = 0.5 * massProjected[1] * PI * (dotu - dotx[0]) * fabs(dotu - dotx[0]);
	fslam[1] = 0.5 * massProjected[1] * PI * (dotv - dotx[1]) * fabs(dotv - dotx[1]);
	fslam[2] = 0.5 * massProjected[0] * PI * (dotw - dotx[2]) * fabs(dotw - dotx[2]);
	
};

double *
OMorisonEquation::Fdrag(double dotu, double dotv, double dotw) {
	//n = 0.5 if part submerged, = 1 if all submerged
	
	fdrag[0] = 0.5 * massProjected[1] * Cd * (dotu - dotx[0]) * fabs(dotu - dotx[0]);
	fdrag[1] = 0.5 * massProjected[1] * Cd * (dotv - dotx[1]) * fabs(dotv - dotx[1]);
	fdrag[2] = 0.5 * massProjected[0] * Cd * (dotw - dotx[2]) * fabs(dotw - dotx[2]);
	return fdrag;
};

double * OMorisonEquation::Ffrkr(double ddotu, double ddotv, double ddotw) {
	ffrkr[0] = mass * ddotu;
	ffrkr[1] = mass * ddotv;
	ffrkr[2] = mass * ddotw;
	return ffrkr;
}

double * OMorisonEquation::Finer() {
	
	finer[0] = ffrkr[0] * Cm;
	finer[1] = ffrkr[1] * Cm;
	finer[2] = ffrkr[2] * Cm;
	return finer;
};

double * OMorisonEquation::Faddm() {
	faddm[0] = mass * Ca * (-ddotx[0]);
	faddm[1] = mass * Ca * (-ddotx[1]);
	faddm[2] = mass * Ca * (-ddotx[2]);
	return faddm;
};


double * OMorisonEquation::Forces() {
	forces[0] = fdamp[0] + fdrag[0] + finer[0] + faddm[0] + frest[0] + 0;
	forces[1] = fdamp[1] + fdrag[1] + finer[1] + faddm[1] + frest[1] + 0;
	forces[2] = fdamp[2] + fdrag[2] + finer[2] + faddm[2] + frest[2] + fwavez;
	#ifdef DEBUG
	if (i<5)
	printf("Fsum=[%.1f,%.1f,%1f], finer=[%.4f,%.4f,%.4f], faddm=[%.4f,%.4f,%.4f], fdrag=[%.6f,%.6f,%.6f],\n"
				   " frest=[%.4f,%.4f,%.4f], fdamp=[%.4f,%.4f,%.4f], Fwavei=%4.f\n",
		   forces[0], forces[1], forces[2],
		   finer[0], finer[1], finer[2], faddm[0],faddm[1],faddm[2], fdrag[0], fdrag[1], fdrag[2],
		   frest[0],frest[1],frest[2], fdamp[0],fdamp[1],fdamp[2], Fwavei);
	#endif
	
	return forces;
};

double * OMorisonEquation::Fmoments() {
	fmoments[0] = forces[0] * (x[2] + draft0);
	fmoments[1] = forces[1] * (x[2] + draft0);
	fmoments[2] = 0;
	return fmoments;
}

double OMorisonEquation::Reynold(double dotuax, double Kvc) {
	//re = Rho * pow( Ua[9] - dotX[i], 2 );
	re = dotuax * diameter / Kvc;
	return re;
}

double OMorisonEquation::Keulegan_Carpenter(double T, double dotuax) {
	KC = dotuax * T / diameter;
	return KC;
}

double OMorisonEquation::Sarpkaya() {
	Beta = re / KC;
	return Beta;
	
	
}

double OMorisonEquation::CDrag() {
	/*
	 a = { 6.72782514e+00  -8.72350144e+00   5.57177264e+00  -1.98687227e+00
   4.46687331e-01  -6.70196564e-02   6.98421355e-03  -5.20518632e-04
   2.82514610e-05  -1.11937647e-06   3.14212818e-08  -5.50903153e-10
   2.60118968e-12   1.24050978e-13  -2.04211513e-15  -2.95909475e-17
   7.30357705e-19   1.15781837e-20  -2.01429722e-22  -5.47146273e-24
   2.51911248e-26   2.21953809e-27   1.44109758e-29  -6.60291287e-31
  -1.26112813e-32   1.23699642e-34   6.03012573e-36  -1.60158880e-38
  -2.63628950e-39   3.87305300e-41  -1.67676135e-43}
	 
	 Cd = SUM_0_30 {a_i*Kc^i}
	 
	 */
	
	
	double a[31] = {
			6.72782514e+00, -8.72350144e+00, 5.57177264e+00, -1.98687227e+00,
			4.46687331e-01, -6.70196564e-02, 6.98421355e-03, -5.20518632e-04,
			2.82514610e-05, -1.11937647e-06, 3.14212818e-08, -5.50903153e-10,
			2.60118968e-12, 1.24050978e-13, -2.04211513e-15, -2.95909475e-17,
			7.30357705e-19, 1.15781837e-20, -2.01429722e-22, -5.47146273e-24,
			2.51911248e-26, 2.21953809e-27, 1.44109758e-29, -6.60291287e-31,
			-1.26112813e-32, 1.23699642e-34, 6.03012573e-36, -1.60158880e-38,
			-2.63628950e-39, 3.87305300e-41, -1.67676135e-43};
	
	if (Cd == -1) {
		
		Cd = calc_poly(31, KC, a);
		
	}
	
	Cd *= nsubs * 0.5;
	
	return Cd;
	
}

double OMorisonEquation::CInertia() {
	
	double a[18] = {1.36741509e+01, -8.42861085e+01, 2.64036089e+02, -4.77831612e+02,
	                5.61871443e+02, -4.57394697e+02, 2.68110607e+02, -1.16137971e+02,
	                3.78175431e+01, -9.35265180e+00, 1.76423150e+00, -2.53388070e-01,
	                2.74679857e-02, -2.20736173e-03, 1.27362928e-04, -4.98508748e-06,
	                1.18461756e-07, -1.28959671e-09};
	
	double b[13] = {-2.94244141e+02, 1.09857912e+02, -1.83362700e+01, 1.81417151e+00,
	                -1.18448186e-01, 5.37606571e-03, -1.73958299e-04, 4.04478076e-06,
	                -6.71033926e-08, 7.75122027e-10, -5.92174081e-12, 2.68868435e-14,
	                -5.49128236e-17};
	
	if (Cm == -1) {
		double cm = 0;
		if (KC <= 10.5) {
			cm = calc_poly(18, KC, a);
		} else if (KC > 10.5 and KC < 15) {
			if (Beta < 2400) {
				cm = 1.2;
			} else if (Beta > 14000) {
				cm = 0.6;
			} else {
				double k = log10(1.2 / 0.6) / log10(2400 / 14000);
				cm = 1.2 * pow(Beta / 2400, k);
			}
		} else if (KC >= 15) {
			cm = calc_poly(13, KC, b);
		};
		
		Cm = cm;
		Ca = cm - 1;
	}
	
	return Cm;
}

