//
// Created by robgrzel on 29.06.17.
//

#ifndef EXAM_OSYSTEMSTRUCTWAVE_H
#define EXAM_OSYSTEMSTRUCTWAVE_H


#include "OWave.h"
#include "OMorisonEquation.h"
#include "OSeas.h"

#include "UtilSources/template_utils.h"
#include "AResults.h"
#include "Cases.h"



struct AElem_t {
	double xc = 0;
	double yc = 0;
	double zc = 0;
	double dxc = 1;
	double dyc = 1;
	double dzc = 1;
};


struct AElems_t {
	
	size_t numElems;
	
	std::vector<AElem_t> E;
	
	AElem_t &operator[](int i) {
		return E[i];
	}
	
	void gen_on_z(double z0, double z1, bool doEqual = true) {
		
		if (doEqual) {
			
		}
		
		size_t numElems = E.size();
		double dz = (z1 - z0) / numElems;
		
		E[0].zc = z0 + dz / 2;
		
		for (int i = 0; i < numElems; i++) {
			E[i].dzc = dz;
		}
		
		for (int i = 1; i < numElems; i++) {
			E[i].zc = E[i - 1].zc + dz;
		}
		
		for (int i = 0; i < numElems; i++) {
			printf("dz=%f, zc=%f\n", dz, E[i].zc);
		}
		
	}
	
	AElems_t(size_t numElems) : numElems(numElems), E(numElems) {
		
	};
	
	
};

void monopile_stuck_in_seabed_on_wave(AMonopile D, AResults_t<AResultForcesMoments_t> &R);

void buoy_floating_no_wave(AFloatingCylinder A, AResults_t<AResultAll_t> &R);

void buoy_floating_on_wave(AFloatingCylinder D, AResults_t<AResultAll_t> &R);

#endif //EXAM_OSYSTEMSTRUCTWAVE_H
