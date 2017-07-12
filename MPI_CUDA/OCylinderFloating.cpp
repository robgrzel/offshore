//
// Created by robgrzel on 12.05.17.
//

#include "OCylinderFloating.h"
#include "UtilSources/Math/const.h"
#include "UtilSources/template_utils.h"


double GCylinderFloating::get_diameter() {
	return diameter;
}


void GCylinderFloating::displaced_mass() {
	massInit = massDisp = areaWaterline * Rho * draughtInit;
};

void GCylinderFloating::area_of_waterline() {
	areaWaterline = PI / 4 * pow(diameter, 2);
};

void GCylinderFloating::area_projected_to_motion_direction() {
	areaProjected = PI / 4 * pow(diameter, 2);
};


GCylinderFloating::GCylinderFloating() {

};

GCylinderFloating::GCylinderFloating(double diameter, double draughtInit, double height,
                                     double Rho, double Grav, double Dvc) :
		diameter(diameter), draughtInit(draughtInit), height(height),
		Rho(Rho), Grav(Grav), Dvc(Dvc) {
	init();
};

void GCylinderFloating::geometry() {
	area_projected_to_motion_direction();
	area_of_waterline();
	displaced_mass();
};

void GCylinderFloating::init() {
	
	geometry();
	
};

void OCylinderFloating::acceleration() {
	ddotX[i] = F[i] / (coefMass * massInit);
}

void OCylinderFloating::velocity() {
	/*
	dt = time step
	dotZi = buoy velocity current step
	dotZj = buoy Veloc next step
	ddotZj = buoy Accel next step
	*/
	//dotX[i3+2] = dotX[j3+2];
	dotX[j] = dotX[i] + ddotX[i] * dt; //dotZj
	
}

void OCylinderFloating::displacement() {
	/*
	dt = time step
	Zi = buoy Displ current step
	Zj = buoy displacament next step
	dotZi = buoy velocity current step
	dotZj = buoy Veloc next step
	*/
	X[j] = X[i] + 0.5 * (dotX[i] + dotX[j]) * dt; //Zj
	
}


void OCylinderFloating::force_drag_part_submerged() {
	Fdrag[i] = 0.5 * Rho * areaProjected * 0.5 * coefDrag * fabs(Ui[2] - dotX[i]) * (Ui[2] - dotX[i]);
	
};

void OCylinderFloating::force_drag_full_submerged() {
	Fdrag[i] = 0.5 * Rho * areaProjected * coefDrag * fabs(Ui[2] - dotX[i]) * (Ui[2] - dotX[i]);
};

void OCylinderFloating::force_lift() {
	Flift[i] = 0.5 * Rho * areaProjected * coefLift * fabs(Ui[2] - dotX[i]) * (Ui[2] - dotX[i]);
}


void OCylinderFloating::force_damping() {
	Fdamp[i] = -coefDamp * dotX[i];
	
};

void OCylinderFloating::force_stiffness() {
	Frest[i] = -Rho * Grav * areaWaterline * X[i];
	
};

void OCylinderFloating::force_added_mass2() {
	Faddm[i] = -massDisp * coefAddmass * ddotX[i];
	
};

void OCylinderFloating::forces_sum() {
	F[i] = Fdamp[i] + Fdrag[i] + Frest[i] + Fwavei;
	
};

void OCylinderFloating::coef_drag_part_submerged() {
	coefDrag = Fdrag[i] / (Rho * areaProjected * pow(Ui[2] - dotX[i], 2));
}

void OCylinderFloating::coef_drag_full_submerged() {
	coefDrag = Fdrag[i] / (0.5 * Rho * areaProjected * pow(Ui[2] - dotX[i], 2));
}

void OCylinderFloating::coef_mass() {
	coefMass = 1 + coefAddmass;
}

void OCylinderFloating::reynolds_number() {
	//re = Rho * pow( Ua[9] - dotX[i], 2 );
	Re = Rho * pow(Ua[0], 2);
}

void OCylinderFloating::keulegan_carpenter_number() {
	KC = Ua[0] * T / diameter;
}

void OCylinderFloating::sarpkaya_beta() {
	Beta = pow(diameter, 2) / (Kvc * T);
}

void OCylinderFloating::save_data(std::string name) {
	
	std::vector<double> t(numSteps);
	
	for (int i=0;i<numSteps;i++){
		t[i]=dt*i;
	}
	
	vec2file(name+"_FloatingCylinder_t.txt", t);
	vec2file(name+"_FloatingCylinder_z.txt", X);
	vec2file(name+"_FloatingCylinder_dotz.txt", dotX);
	vec2file(name+"_FloatingCylinder_ddotz.txt", ddotX);

	vec2file(name+"_FloatingCylinder_Fsum.txt", F);
	vec2file(name+"_FloatingCylinder_Faddm.txt", Faddm);
	vec2file(name+"_FloatingCylinder_Frest.txt", Frest);
	vec2file(name+"_FloatingCylinder_Fdamp.txt", Fdamp);
	vec2file(name+"_FloatingCylinder_Fdrag.txt", Fdrag);
	vec2file(name+"_FloatingCylinder_Flift.txt", Flift);
	
}