//
// Created by robgrzel on 12.05.17.
//

#include "OElemCylinder.h"


double OCylinder::get_diameter( ) {
	return diameter;
}


void OCylinder::displaced_mass( ) {
	massInit = massDisp = areaWaterline * Rho * draughtInit;
};

void OCylinder::area_of_waterline( ) {
	areaWaterline = PI / 4 * pow( diameter, 2 );
};

void OCylinder::area_projected_to_motion_direction( ) {
	areaProjected = PI / 4 * pow( diameter, 2 );
};


OCylinder::OCylinder( ) {

};

OCylinder::OCylinder( double diameter, double draughtInit, double height, double Rho, double Grav ) :
		diameter( diameter ), draughtInit( draughtInit ), height( height ),
		Rho( Rho ), Grav( Grav ) {
	init();
};

void OCylinder::geometry( ) {
	area_projected_to_motion_direction();
	area_of_waterline();
	displaced_mass();
};

void OCylinder::init( ) {

	geometry();

};

void OElemCylinder::acceleration( ) {
	ddotX[j3 + 2] = (F[i3 + 2] - Fmass[i3 + 2]) / (coefMass * massInit);
}

void OElemCylinder::velocity( ) {
	/*
	dt = time step
	dotZi = buoy velocity current step
	dotZj = buoy velocity next step
	ddotZj = buoy acceleration next step
	*/
	//dotX[i3+2] = dotX[j3+2];
	dotX[j3 + 2] = dotX[i3 + 2] + ddotX[j3 + 2] * dt; //dotZj

}

void OElemCylinder::displacement( ) {
	/*
	dt = time step
	Zi = buoy displacement current step
	Zj = buoy displacament next step
	dotZi = buoy velocity current step
	dotZj = buoy velocity next step
	*/
	X[j3 + 2] = X[i3 + 2] + 0.5 * (dotX[i3 + 2] + dotX[j3 + 2]) * dt; //Zj

}

void OElemCylinder::force_damping( ) {
	Fdamp[i3 + 2] = -coefDamp * dotX[i3 + 2];

};

void OElemCylinder::force_drag_part_submerged( ) {
	Fdrag[i3 + 2] = 0.5 * Rho * areaProjected * 0.5 * coefDrag * fabs( Ui[2] - dotX[i3 + 2] ) * (Ui[2] - dotX[i3 + 2]);

};

void OElemCylinder::force_drag_full_submerged( ) {
	Fdrag[i3 + 2] = 0.5 * Rho * areaProjected * coefDrag * fabs( Ui[2] - dotX[i3 + 2] ) * (Ui[2] - dotX[i3 + 2]);
};

void OElemCylinder::force_lift( ) {
	Flift[i3 + 2] = 0.5 * Rho * areaProjected * coefLift * fabs( Ui[2] - dotX[i3 + 2] ) * (Ui[2] - dotX[i3 + 2]);
}

void OElemCylinder::force_stiffness( ) {
	Frest[i3 + 2] = -Rho * Grav * areaWaterline * X[i3 + 2];

};

void OElemCylinder::force_added_mass2( ) {
	Fmass[i3 + 2] = -massDisp * coefMass * ddotX[i3 + 2];

};

void OElemCylinder::forces_sum( ) {
	F[i3 + 2] = Fdamp[i3 + 2] + Fdrag[i3 + 2] + Frest[i3 + 2] + Fmass[i3 + 2] + Fwavei;

};

void OElemCylinder::coef_drag_part_submerged( ) {
	coefDrag = Fdrag[i3 + 2] / (Rho * areaProjected * pow( Ui[2] - dotX[i3 + 2], 2 ));
}

void OElemCylinder::coef_drag_full_submerged( ) {
	coefDrag = Fdrag[i3 + 2] / (0.5 * Rho * areaProjected * pow( Ui[2] - dotX[i3 + 2], 2 ));
}

void OElemCylinder::coef_mass( ) {
	coefMass = 1 + coefAddmass;
}

void OElemCylinder::reynold_number( ) {
	reynoldNum = Rho * pow( Ui[2] - dotX[i3 + 2], 2 );
}


