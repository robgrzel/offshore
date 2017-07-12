//
// Created by robgrzel on 29.06.17.
//

#include "OSysObjEnv.h"
#include "OCylinderFloating.h"
#include "OWaves.h"

#define elif else if

void buoy_floating_no_wave(AFloatingCylinder A, AResults_t <AResultAll_t> & R) {
	
	
	OMorisonEquation O = OMorisonEquation(A.diameter, A.draft0, A.height, A.Rho, A.Grav, A.Dvc, A.dt);
	O.initial_conditions(0, 0, A.z0, 1, 1, A.draft0, 0, 0, A.dotz0, A.Cd2, A.Ca, 0, 0, A.b33);
	
	double * F, * Fdrag, * Fdamp, * Frest, * Faddm;
	double * x, * dotx, * ddotx;
	
	for (int i = 0; i < R.numSteps; i++) {
		
		O.set_step(i);
		
		Fdamp = O.Fdamp();
		Fdrag = O.Fdrag(0, 0, 0);
		Frest = O.Frest();
		ddotx = O.Accel();
		dotx = O.Veloc();
		x = O.Displ();
		Faddm = O.Faddm();
		F = O.Forces();
		
		R[i].ddotx(ddotx);
		R[i].dotx(dotx);
		R[i].x(x);
		R[i].F(F);
		R[i].Fdrag(Fdrag);
		R[i].Faddm(Faddm);
		R[i].Fdamp(Fdamp);
		R[i].Frest(Frest);
		
		
		if (i < 5) {
			printf("###....\n");
			O.print_step();
			
		}
	}
	
	R++;
	
	R << false << A.casename + "_Results.txt";
	
}


void buoy_floating_on_wave(AFloatingCylinder D, AResults_t <AResultAll_t> & R) {
	
	
	seawater_params(D.Rho, D.Kvc, D.oC);
	
	OMorisonEquation O = OMorisonEquation(D.diameter, D.draft0, D.height, D.Rho, D.Grav, D.dt);
	O.initial_conditions(0, 0, D.z0, 1, 1, D.draft0, 0, 0, D.dotz0, D.Cd2, D.Ca, 0, 0, D.b33);
	
	O.CDrag();
	
	OSeas S(D.Amp, D.T, 0, D.Depth, 0, 0, 0, D.Rho, D.Grav, D.dt);
	S.seas_init();
	
	
	wave_load_types waveLoadType;
	structure_volume_types structureVolumeType;
	wave_loads_regimes(D.diameter, D.H, S.get_wavelen(), waveLoadType);
	structure_volume_type(waveLoadType, structureVolumeType);
	
	if (structureVolumeType == largeVolume) {
		printf("Structure of Large Volume Type - error: not implemented (Morison Equation invalid)...\n");
		
	} elif (structureVolumeType == smallVolume) {
		
		double * dotua, * u, * dotu, * ddotu, Eta = 0, Theta = 0, Fwave = 0;
		double * M, * F, * Fdrag, * Fdamp, * Frest, * Faddm;
		double * x, * dotx, * ddotx;
		
		for (int i = 0; i < R.numSteps; i++) {
			
			S.set_step(i);
			S.plane_wavenumber(D.x, D.y);
			Theta = S.wave_phase();
			Eta = S.wave_elevation();
			
			S.excursion_amp(D.z0, D.draft0);
			S.velocity_amp();
			S.acceleration_amp();
			u = S.excursion();
			dotu = S.velocity();
			ddotu = S.acceleration();
			
			O.set_step(i);
			Fdrag = O.Fdrag(dotu[0], dotu[1], dotu[2]);
			Fwave = O.Fwave(Eta);
			Frest = O.Frest();
			Fdamp = O.Fdamp();
			ddotx = O.Accel();
			dotx = O.Veloc();
			x = O.Displ();
			Faddm = O.Faddm();
			F = O.Forces();
			
			R[i].Theta(Theta);
			R[i].Eta(Eta);
			R[i].ddotu(ddotu);
			R[i].dotu(dotu);
			R[i].u(u);
			R[i].ddotx(ddotx);
			R[i].dotx(dotx);
			R[i].x(x);
			R[i].F(F);
			R[i].Fdrag(Fdrag);
			R[i].Faddm(Faddm);
			R[i].Fdamp(Fdamp);
			R[i].Frest(Frest);
			R[i].Fwave(Fwave);
			
			if (i < 15) {
				printf("###....\n");
				S.print_wave_step();
				S.print_particle_step();
				O.print_step();
				
			}
		}
		
		R++;
		
		R << false << D.casename + "_Results.txt";
		
		/*
			
			O.initial_conditions(numSteps, A.dt, A.draft0, 0, 0, A.z0, 0, 0, A.dotz0, 0, 0, 0, A.T);
			W.wave_init_conds(A.dt, O.areaWaterline);
			
			
			W.set_step_wave(0);
			W.calc_step_wave();
			
			W.particle_init_conds(A.draft0, 0, 0, A.z0);
			W.set_step_particle_obj_floating(0, 0, 0, A.z0, W.get_wave_phase());
			W.calc_step_particle_obj_floating();
			
			W.print_wave_step();
			W.print_particle_step();
			
			double tmp[10];
			
			for (int i = 0; i < numSteps; i++) {
				W.set_step_wave(i);
				W.calc_step_wave();
				
				tmp[0] = W.get_force();
				tmp[1] = W.get_velocity(0);
				tmp[2] = W.get_velocity(1);
				tmp[3] = W.get_velocity(2);
				tmp[4] = W.get_acceleration(0);
				tmp[5] = W.get_acceleration(1);
				tmp[6] = W.get_acceleration(2);
				
				O.set_step(i, tmp);
				O.calc_step_part_submerged();
				
				tmp[7] = O.get_displacement(0);
				tmp[8] = W.get_wave_phase();
				
				W.set_step_particle_obj_floating(i, 0, 0, tmp[7], tmp[8]);
				W.calc_step_particle_obj_floating();
				
				if (i < 100) {
					W.print_wave_step();
					W.print_particle_step();
					O.print_step();
				}
			}
			
			printf("save data...\n");
			
			O.save_data(name);
		 */
	}
}

void monopile_stuck_in_seabed_on_wave(AMonopile D, AResults_t <AResultForcesMoments_t> & R) {
	
	OMorisonEquation O(D.diameter, D.draft0, 0, D.Rho, D.Grav, D.dt);
	
	OSeas S(D.Amp, D.Tp, 0, D.Depth, 0, 0, 0, D.Rho, D.Grav, D.dt);
	
	AElems_t E(D.numElems);
	
	E.gen_on_z(-D.draft0, 0);
	
	double * dotua, * dotu, * ddotu, Theta = 0, x[3], dx[3];
	double * M, * F, * Fdrag, * Finer, *Ffrkr;
	
	
	wave_load_types waveLoadType;
	structure_volume_types structureVolumeType;
	wave_loads_regimes(D.diameter, D.H, S.get_wavelen(), waveLoadType);
	structure_volume_type(waveLoadType, structureVolumeType);
	
	if (structureVolumeType == largeVolume) {
		printf("Structure of Large Volume Type - error: not implemented (Morison Equation invalid)...\n");
		
	} elif (structureVolumeType == smallVolume) {
		
		for (int i = 0; i < D.numElems; i++) {
			
			x[0] = E[i].xc;
			x[1] = E[i].yc;
			x[2] = E[i].zc;
			dx[0] = E[i].dxc;
			dx[1] = E[i].dyc;
			dx[2] = E[i].dzc;
			
			{
				S.seas_init();
				
				S.excursion_amp(x[2]);
				dotua = S.velocity_amp();
				S.acceleration_amp();
				S.excursion();
				S.velocity();
				S.acceleration();
				
				
				O.initial_conditions(x[0], x[1], x[2], dx[0], dx[1], dx[2]);
				O.Reynold(dotua[0]);
				O.Keulegan_Carpenter(D.Tp, dotua[0]);
				O.Sarpkaya();
				O.CDrag();
				O.CInertia();
				
			}
			
			S.print_wave_step();
			S.print_particle_step();
			O.print_step();
			
			{
				size_t numSteps = R.size();
				
				
				for (int j = 0; j < numSteps; j++) {
					S.set_step(j);
					S.plane_wavenumber(x[0], x[1]);
					Theta = S.wave_phase();
					S.wave_elevation();
					
					dotu = S.velocity();
					ddotu = S.acceleration();
					
					O.set_step(j);
					
					Fdrag = O.Fdrag(dotu[0], dotu[1], dotu[2]);
					Ffrkr = O.Ffrkr(ddotu[0], ddotu[1], ddotu[2]);
					Finer = O.Finer();
					F = O.Forces();
					M = O.Fmoments();
					
					R[j].dotu(dotu);
					R[j].ddotu(ddotu);
					R[j].Theta(Theta);
					R[j].Fdrag(Fdrag);
					R[j].Finer(Finer);
					R[j].F(F);
					R[j].M(M);
					
					
					if (j < 5) {
						printf("###....\n");
						S.print_wave_step();
						S.print_particle_step();
						O.print_step();
						
					}
				}
			}
			
			R++;
			
			printf("save data...\n");
			
			//R << true << D.casename + "_ResultsTemp.txt";
			
		}
		
		
		R << false << D.casename + "_Results.txt";
		
	}
}

void monopile_stuck_in_seabed_on_waves(AMonopile D, AResults_t <AResultForcesMoments_t> & R) {
	
	double * dotua, * dotu, * ddotu, Theta = 0, x[3], dx[3];
	double * M, * F, * Fdrag, * Finer, *Frkr;
	
	OMorisonEquation O(D.diameter, D.draft0, 0, D.Rho, D.Grav, D.dt);
	
	OWavesJonswap W(D.t1, D.dt, D.Hs, D.Tp, "North Sea");
	
	OSeas S;
	
	AWavePuls_t w;
	
	AElems_t E(D.numElems);
	
	E.gen_on_z(-D.draft0, 0);
	
	size_t numSteps = R.size();
	
	size_t numWaves = W.get_num_waves();
	
	
	wave_load_types waveLoadType;
	structure_volume_types structureVolumeType;
	wave_loads_regimes(D.diameter, D.H, S.get_wavelen(), waveLoadType);
	structure_volume_type(waveLoadType, structureVolumeType);
	
	if (structureVolumeType == largeVolume) {
		printf("Structure of Large Volume Type - error: not implemented (Morison Equation invalid)...\n");
		
	} elif (structureVolumeType == smallVolume) {
		
		for (int i = 0; i < D.numElems; i++) {
			
			x[0] = E[i].xc;
			x[1] = E[i].yc;
			x[2] = E[i].zc;
			dx[0] = E[i].dxc;
			dx[1] = E[i].dyc;
			dx[2] = E[i].dzc;
			
			for (int j = 0; j < numWaves; j++) {
				
				w = W(j, 0);
				
				S.reset_seas(w.amp, w.T, w.eps, D.Depth, 0, 0, 0, D.Rho, D.Grav, D.dt);
				
				S.excursion_amp(x[2]);
				dotua = S.velocity_amp();
				S.acceleration_amp();
				S.excursion();
				S.velocity();
				S.acceleration();
				
				O.initial_conditions(x[0], x[1], x[2], dx[0], dx[1], dx[2]);
				O.Reynold(dotua[0], );
				O.Keulegan_Carpenter(D.Tp, dotua[0]);
				O.Sarpkaya();
				O.CDrag();
				O.CInertia();
				
				S.print_wave_step();
				S.print_particle_step();
				O.print_step();
				
				
				for (int k = 0; k < numSteps; k++) {
					S.set_step(k);
					S.plane_wavenumber(x[0], x[1]);
					Theta = S.wave_phase();
					S.wave_elevation();
					
					dotu = S.velocity();
					ddotu = S.acceleration();
					
					O.set_step(k);
					
					Fdrag = O.Fdrag(dotu[0], dotu[1], dotu[2]);
					Ffrkr = O.Ffrkr(ddotu[0], ddotu[1], ddotu[2]);
					Finer = O.Finer();
					F = O.Forces();
					M = O.Fmoments();
					
					R[k].dotu(dotu);
					R[k].ddotu(ddotu);
					R[k].Theta(Theta);
					R[k].Fdrag(Fdrag);
					R[k].Finer(Finer);
					R[k].F(F);
					R[k].M(M);
					
					
					if (k < 5) {
						printf("###....\n");
						S.print_wave_step();
						S.print_particle_step();
						O.print_step();
						
					}
				}
				
				R++;
				
				printf("save data...\n");
				
				//R << true << D.casename + "_ResultsTemp.txt";
			}
			
		}
		
		R << false << D.casename + "_Results.txt";
		
	}
}