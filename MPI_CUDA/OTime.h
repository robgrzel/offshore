//
// Created by robgrzel on 17.05.17.
//
#pragma once
#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OTIME_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OTIME_H


#include <cstdio>
#include <iostream>
#include <vector>
#include "../UtilSources/Containers/std_utils.h"

class OTime {
	private:
		void init( ) {
			if (t1 != t0 and dt > 0) {
				numSteps = size_t( (t1 - t0) / dt );
				TIME     = arange( t0, t1, dt );
			} else {
				numSteps = 1;
				TIME     = std::vector<double>( 1, 0 );
			}
			isInit = true;

			std::cout << "numSteps\n";
		}

	public:

		bool               isInit = false;
		double              t0, t1, dt, t;
		size_t             numSteps;
		std::vector<double> TIME;

		inline void time_bounadry_conditions( double t0, double t1, double dt ) {

			this->t0 = t0;
			this->t1 = t1;
			this->dt = dt;

			init();

		}

		inline double set_step( int i ) {
			t = dt * i;
			return t;
		}

		inline double up_time_step( ) {
			t += dt;
			return t;
		}

		inline size_t get_num_steps( ) {
			return numSteps;
		}


		OTime( const OTime* o ) :
				t0( o->t0 ), t1( o->t1 ), dt( o->dt ), t( o->t0 ) {
			init();

		};

		OTime( double t0 = 0, double t1 = 0, double dt = 0 ) :
				t0( t0 ), t1( t1 ), dt( dt ), t( t0 ) {
			init();
		};


};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OTIME_H
