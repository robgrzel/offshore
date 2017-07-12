//
// Created by robgrzel on 19.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OPARTICLE_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OPARTICLE_H


#include <vector>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "UtilSources/Math/phys.h"

class OParticle {
	
	protected:
		//wave data
		double om, amp, depth, k, Rho, Grav, oC, K[2], Ek[2];
		
		//particle data
		double Theta, Ps;
		
		int i;
		
		double PHI, PHIa;
		double P, Pd, Pda;
		double dotu[3], ddotu[3], u[3];
		double dotua[2], ddotua[2], ua[2];
	
	public:
		
		
		double *excursion_amp(double z, double draft0 = 0);
		
		double *velocity_amp();
		
		double *acceleration_amp();
		
		double dynamical_pressure_amplitude();
		
		double potential_amplitude();
		
		double *velocity(double Theta);
		
		double *excursion(double Theta);
		
		double *acceleration(double Theta);
		
		double potential(double Theta);
		
		double pressure();
		
		double statical_pressure(double zc, double dz2b = 0);
		
		double dynamical_pressure(double Theta);
		
		
	
	
	public:
		
		
		inline void print_particle_step() {
			printf("particle: i=%d ::: [...om=%.4f, amp=%.4f, Rho=%.4f, Grav=%.4f, k=%.4f, depth=%.4f,"
					       " Theta=%.4f, Pa=%.4f, P=%.4f, \nu=[%.4f,%.4f,%.4f],"
					       " ddotu=[%.4f,%.4f,%.4f], ddotu=[%.4f,%.4f,%.4f]...]\n",
			       i,
			       om, amp, Rho, Grav, k, depth,
			       Theta, Pda, Pd,
			       u[0], u[1], u[2],
			       dotu[0], dotu[1], dotu[2],
			       ddotu[0], ddotu[1], ddotu[2]
			);
		}
		
		
		inline void particle_init_conds(double k_, double xEk, double yEk) {
			
			
			P = 0;
			Pd = 0;
			PHI = 0;
			u[0] = 0;
			u[1] = 0;
			u[2] = 0;
			dotu[0] = 0;
			dotu[1] = 0;
			dotu[2] = 0;
			ddotu[0] = 0;
			ddotu[1] = 0;
			ddotu[2] = 0;
			
			Pda = 0;
			PHIa = 0;
			
			ua[0] = 0;
			ua[1] = 0;
			dotua[0] = 0;
			dotua[1] = 0;
			ddotua[0] = 0;
			ddotua[1] = 0;
			
			Theta = 0;
			K[0] = k * xEk;
			K[1] = k * yEk;
			Ek[0] = xEk;
			Ek[1] = yEk;
			
			i = 0;
			
			k = k_;
			depth = fabs(depth);
			
			
			
		};
		
		
		inline void reset_particle(double om_, double amp_, double depth_, double Grav_, double Rho_) {
			om = om_;
			amp = amp_;
			depth = depth_;
			Rho = Rho_;
			Grav = Grav_;
		};
		
		OParticle(){
		
		}
		
		OParticle(double om, double amp, double depth, double Grav, double Rho) :
				om(om), amp(amp), depth(fabs(depth)), Rho(Rho), Grav(Grav) {
			
		}
	
};

#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OPARTICLE_H
