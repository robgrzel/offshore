//
// Created by robgrzel on 21.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_VELEM_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_VELEM_H

#include <cstdio>
#include <vector>

class VElem {
		//friend class OSystem;
		friend class OSubSystem;

	public:

		virtual inline const bool& is_initialized( ) = 0;

		virtual inline const double& get_area_waterline( ) = 0;

		virtual inline const double& get_draught_init( ) = 0;

		virtual inline const double& get_displacement( int j = 0 ) = 0;

		virtual inline const double& get_velocity( int j = 0 ) = 0;

		virtual inline const double& get_acceleration( int j = 0 ) = 0;

		virtual inline const double& get_forces( int j = 0 ) = 0;

		virtual inline const double& get_momentum( int j = 0 ) = 0;


		virtual inline void swap_displacement(std::vector<double> & X) = 0;

		virtual inline void swap_velocity( std::vector<double> & dotX) = 0;

		virtual inline void swap_acceleration(std::vector<double>& ddotX ) = 0;

		virtual inline void swap_forces(std::vector<double>& F ) = 0;

		virtual inline void swap_momentum(std::vector<double>& M ) = 0;


		virtual inline void init( double* d = nullptr ) = 0;

		virtual inline void update( double* data = nullptr ) = 0;

		virtual inline void initial_conditions( int N = 0, double* p = nullptr ) = 0;

		virtual inline void set_step( int i_ = 0, double* p = nullptr ) = 0;

		virtual inline void calc_step_part_submerged( ) = 0;


	protected:
		VElem( );

		virtual ~VElem( );
};

#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_VELEM_H
