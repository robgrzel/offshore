//
// Created by robgrzel on 11.04.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_WAVENUMBER_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_WAVENUMBER_H


#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

class OWavenumber {
	
	protected:
		//temporary values
		double xi, yi, k0, tanhkh;
		double f, fp, ome;
		
		//numberical values
		double minK = 0, maxK = 1e6, prec;
		
		//checking values
		bool doIter, isSolved, isBlocking;
		
		//output values: wavelenght, wavenum, current speed (current Veloc)
		double lambda, k, v, K[2], Ek[2];
	
	private:
		//initial values
		double om, phi; //wave rake angle
		double depth, Grav;
		double vx, vy;// current Veloc x, y directions, sign: (-) against current, (+) with current
		double tol;
	
	public:
		
		OWavenumber(double om, double vx = -1, double vy = 0, double phi = 0,
		            double depth = 1000, double Grav = 9.81, double tol = 1e-6) :
				om(om), vx(vx), vy(vy), phi(phi), depth(fabs(depth)), Grav(Grav), tol(tol) {
			
		};
		
		OWavenumber(){
		
		}
		
		inline void reset_wavenumber(double om = 1, double vx = 0, double vy = 0, double phi = 0, double depth = 1000,
		                             double Grav = 9.81, double tol = 1e-6) {
			this->om = om;
			this->vx = vx;
			this->vy = vy;
			this->phi = phi;
			this->depth = fabs(depth);
			this->Grav = Grav;
			this->tol = tol;
			
			
		}
		
		double get_speed_current(void) {
			return v;
		};
		
		double get_wavelen(void) {
			return lambda;
		};
		
		double get_wavenum(void) {
			return k;
		};
		
		double get_wavenum_vector(int i) {
			return K[i];
		};
		
		void wavenumber_calc();
	
	private:
		void init_guess_without_currents();
		
		void init_guess_with_current_velocity();
		
		void wavenumber_to_wavelenght();
		
		void wavenumber_to_vector();
		
		inline void solve_by_newton_raphson();
		
		inline void check_solved();
		
		inline void check_solution();
		
		
		void wave_propagation_direction();
	
};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_WAVENUMBER_H
