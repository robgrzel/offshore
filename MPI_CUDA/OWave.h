//
// Created by robgrzel on 12.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVE_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVE_H


#include <cstdio>
#include "UtilSources/Math/const.h"
#include "UtilSources/Math/math.h"
#include "UtilSources/Math/phys.h"


class OWave {
	
	protected:
		double amp, eps, om, alpha;
		double depth;
		double Rho, Grav, oC;
		
		double V[2], K[2], Ek[2], k, kx, phi;
		double c, Cp, Cg, Cg_Cp;
		
		//boundary cond
		double dt, t;
		
		double Eta, Theta;
		
		int i;
	
	public:
		
		double plane_wavenumber(double x = 0, double y = 0);
		
		double wave_phase();
		
		double dispersion_relation();
		
		double wave_phase_speed();
		
		double wave_group_speed();
		
		double wave_group_phase_ratio();
		
		double phase_speed();
		
		double wave_elevation();
	
		
	
	public:
		
		/*
		 1. for: calc_step_wave
		 2. wave_boundary_conditions
		 3. particle_init_conds
		 4. for: set_step_particle, calc_step_particle
		 */
		
		
		
		inline double get_period() {
			return OM2T(om);
		}
		
		inline const double & get_pulsation() {
			return om;
		}
		
		void print_wave_step() {
			printf("wave    : i=%d ::: t=%.4f,amp=%.4f,om=%f, eps=%.4f,depth=%.4f,Ek=[%.4f,%.4f],"
					       "K=[%.4f,%.4f],k=%.4f,kx=%.4f,Theta=%.4f,Eta=%.4f\n",
			       i, t, amp, om, eps, depth, Ek[0], Ek[1], K[0], K[1], k, kx, Theta, Eta);
		}
		
		
		inline double set_step_wave(const int & i_) {
			i = i_;
			t = dt * i;
			return t;
		}
		
		inline void wave_init_conds(double k_, double xEk, double yEk) {
			t = 0;
			i = 0;
			
			k = k_;
			K[0] = k * xEk;
			K[1] = k * yEk;
			Ek[0] = xEk;
			Ek[1] = yEk;
			
			Eta = 0;
			Theta = 0;
			
		};
		
		inline void wave_params() {
			phase_speed();
			dispersion_relation();
			wave_phase_speed();
			wave_group_speed();
			wave_group_phase_ratio();
		}
		
		inline void
		reset_wave(double amp_ = 1, double om_ = T2OM(10), double eps_ = 0, double depth_ = 1000, double vx_ = 0,
		           double vy_ = 0, double phi_ = 0, double dt_ = 0.1, double Grav_ = 9.81, double Rho_ = 1025) {
			amp = amp_;
			om = om_;
			eps = eps_;
			depth = depth_;
			V[0] = vx_;
			V[1] = vy_;
			phi = phi_;
			Rho = Rho_;
			Grav = Grav_;
			dt = dt_;
		}
		
		OWave() {
		
		}
		
		OWave(
				double amp, double om, double eps = 0, double depth = 1000, double vx = 0,
				double vy = 0, double phi = 0, double dt = 0.1, double Grav = 9.81, double Rho = 1025
		) : amp(amp), om(om), eps(eps), depth(depth), V{vx, vy}, phi(phi), Rho(Rho), Grav(Grav), dt(dt) {

		}
};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVE_H
