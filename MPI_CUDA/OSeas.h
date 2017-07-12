//
// Created by robgrzel on 29.06.17.
//

#ifndef EXAM_OSEAS_H
#define EXAM_OSEAS_H

#include "OParticle.h"
#include "OWavenumber.h"
#include "OWave.h"


class OSeas : public OWavenumber, public OParticle, public OWave {
		template <class Data_t, class TResult_t>  friend class OSysObjEnv;
	

	public:
		
		
		void set_step(int i){
			OWave::set_step_wave(i);
		}
		
		inline double * excursion() {
			return OParticle::excursion(OWave::Theta);
		}
		
		inline double * velocity() {
			return OParticle::velocity(OWave::Theta);
		}
		
		inline double * acceleration() {
			return OParticle::acceleration(OWave::Theta);
		}
		
		inline double potential() {
			return OParticle::potential(OWave::Theta);
		}
		
		inline double dynamical_pressure() {
			return OParticle::dynamical_pressure(OWave::Theta);
		}
		
		inline void wave_init_conds() {
			OWave::wave_init_conds(OWavenumber::k,OWavenumber::Ek[0],OWavenumber::Ek[1]);
		}
		
		inline void particle_init_conds() {
			OParticle::particle_init_conds(OWavenumber::k,OWavenumber::Ek[0],OWavenumber::Ek[1]);
		}
		
		inline void seas_init() {
			set_step_wave(0);
			wavenumber_calc();
			wave_init_conds();
			wave_params();
			particle_init_conds();
			
			
			
		}
		
		inline void reset_seas(double amp = 1, double T = 10, double eps = 0, double depth = 1000, double vx = 0,
		                       double vy = 0, double phi = 0, double dt = 0.1, double Grav = 9.81, double Rho = -1) {
			
			OWavenumber::reset_wavenumber(T2OM(T), vx, vy, phi, depth, Grav, 1e-6);
			OWave::reset_wave(amp, T, eps, depth, vx, vy, phi, dt,Grav, Rho);
			OParticle::reset_particle(T2OM(T), amp, depth, Grav, Rho);
			
			seas_init();
			
		}
		
		OSeas() {
		
		}
		
		OSeas(double amp, double T, double eps, double depth = 1000, double vx = 0,
		      double vy = 0, double phi = 0, double dt = 0.1, double Grav = 9.81, double Rho = -1 ) :
		// vx,vy : current Veloc x, y directions, sign: (-) wave against current, (+) wave with current
				OWavenumber(T2OM(T), vx, vy, phi, depth, Grav, 1e-6),
				OWave(amp, T2OM(T), eps, depth, vx, vy, phi, dt, Grav, Rho),
				OParticle(T2OM(T), amp, depth, Grav, Rho) {
			seas_init();
			
		}
	
};


#endif //EXAM_OSEAS_H
