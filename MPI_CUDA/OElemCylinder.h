//
// Created by robgrzel on 12.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H


#include <cmath>
#include <cstdio>
#include <vector>
#include "OSubSystemWaves.h"


class OCylinder {

	public:
		double diameter      = 0;
		double height        = 0;
		double draughtInit   = 0;
		double areaProjected = 0;
		double areaWaterline = 0;
		double massInit      = 0;
		double massDisp      = 0;
		double Rho           = 1025;
		double Grav          = 9.81;

	protected:
		void geometry( );

		void init( );

	public:

		double get_diameter( );

		void displaced_mass( );

		void area_of_waterline( );

		void area_projected_to_motion_direction( );

		OCylinder( );

		OCylinder( double diameter, double draughtInit, double height, double Rho, double Grav );


};

typedef struct AElemCylinder_t{

};

class OElemCylinder : public VElem, public OCylinder {
		friend class OSubSystemWaves;

	private:

		double coefAddmass, coefMass, coefDamp, coefDrag, coefLift;

		double massAdded, reynoldNum;

		//double F, Fdrag, Fdamp, Frest, Fmass, Flift;
		//double ddotX, dotZi, dotZj, X, dt;

		std::vector<double> M, F, Fdrag, Fdamp, Frest, Fmass, Flift;
		std::vector<double> ddotX, dotX, X;

		//wave data
		double Ui[3], dotUi[3], Fwavei, dt;

		int i, j, i3, j3, numSteps;


	protected:

		void acceleration( );

		void velocity( );

		void displacement( );

		void force_damping( );

		void force_drag_part_submerged( );

		void force_drag_full_submerged( );

		void force_stiffness( );

		void force_added_mass2( );

		void forces_sum( );

		void coef_mass( );

		void coef_drag_part_submerged( );

		void coef_drag_full_submerged( );

		void force_lift( );

		void reynold_number( );

	public:

		~OElemCylinder( ) {

		};

		OElemCylinder( ) : OCylinder(), isInitialized( false ) {

		};

		OElemCylinder(
				double coefAddmass, double coefDrag, double coefDamp, double diameter, double draughtInit,
				double height, double Rho, double Grav
		) :
				OCylinder( diameter, draughtInit, height, Rho, Grav ), isInitialized( true ),
				coefAddmass( coefAddmass ), coefMass( 1 + coefAddmass ),
				coefDrag( coefDrag ), coefDamp( coefDamp ) {

		};


	private:

		bool isInitialized;


	protected:


		virtual inline const bool& is_initialized( ) {
			return isInitialized;
		};


		virtual inline const double& get_area_waterline( ) {
			return areaWaterline;
		}

		virtual inline const double& get_draught_init( ) {
			return draughtInit;
		}

		virtual inline const double& get_displacement( int j ) {
			return X[i3 + j];
		}

		virtual inline const double& get_velocity( int j ) {
			return dotX[i3 + j];
		}

		virtual inline const double& get_acceleration( int j ) {
			return ddotX[i3 + j];
		}

		virtual inline const double& get_forces( int j = 0 ) {
			return F[i3 + j];
		};

		virtual inline const double& get_momentum( int j = 0 ) {
			return M[i3 + j]; //TODO
		};


		virtual inline void update( double* data = nullptr ) {
			init( data );

		};

		virtual inline void swap_displacement( std::vector<double>& X_ ) {
			X.swap( X_ );
		}

		virtual inline void swap_velocity( std::vector<double>& dotX_ ) {
			dotX.swap( dotX_ );
		}

		virtual inline void swap_acceleration( std::vector<double>& ddotX_ ) {
			ddotX.swap( ddotX_ );
		}

		virtual inline void swap_forces( std::vector<double>& F_ ) {
			F.swap( F_ );
		};

		virtual inline void swap_momentum( std::vector<double>& M_ ) {
			M.swap( M_ );
		};


		virtual inline void init( double* data = nullptr ) {
			//OCylinder( diameter, draught, height, Rho, Grav ),

			if (data != nullptr) {

				coefAddmass = data[1];
				coefDrag    = data[2];
				coefDamp    = data[3];
				diameter    = data[4];
				draughtInit = data[5];
				height      = data[6];
				Rho         = data[7];
				Grav        = data[8];
			}

			OCylinder::init();

			coef_mass();

		};


		virtual inline void initial_conditions( int N = 0, double* data = nullptr ) {

			/*
			 	data[0] = dt;
				data[1] = d;
				data[2] = x0;
				data[3] = y0;
				data[4] = z0;
				data[5] = dotx0;
				data[6] = doty0;
				data[7] = dotz0;
			 */

			if (data != nullptr) {
				initial_conditions( N, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] );
				return;
			}

			numSteps = N;


		}

		virtual inline void set_step( int i_ = 0, double* data = nullptr ) {

			i  = i_;
			j  = i + 1;
			i3 = i * 3;
			j3 = j * 3;


			Fwavei = data[0];
			Ui[0]    = data[1];
			Ui[1]    = data[2];
			Ui[2]    = data[3];
			dotUi[0] = data[4];
			dotUi[1] = data[5];
			dotUi[2] = data[6];

		}

		virtual inline void calc_step_part_submerged( ) {
			force_damping();
			force_drag_part_submerged();
			force_stiffness();
			force_added_mass2();
			forces_sum();
			acceleration();
			velocity();
			displacement();
		}


	public:
		inline void initial_conditions(
				int N = 0, double dt_ = 0, double d0_ = 0,
				double x0_ = 0, double y0_ = 0, double z0_ = 0,
				double dotx0_ = 0, double doty0_ = 0, double dotz0_ = 0
		) {

			numSteps = N;

			Flift = std::vector<double>( numSteps * 3, 0 );
			Fdrag = std::vector<double>( numSteps * 3, 0 );
			Fdamp = std::vector<double>( numSteps * 3, 0 );
			Fmass = std::vector<double>( numSteps * 3, 0 );
			Frest = std::vector<double>( numSteps * 3, 0 );
			F     = std::vector<double>( numSteps * 3, 0 );
			M     = std::vector<double>( numSteps * 3, 0 );

			ddotX = std::vector<double>( numSteps * 3, 0 );
			dotX  = std::vector<double>( numSteps * 3, 0 );
			X     = std::vector<double>( numSteps * 3, 0 );

			draughtInit = d0_;
			dt          = dt_;

			X[0]     = x0_;
			X[1]     = y0_;
			X[2]     = z0_;
			dotX[0]  = dotx0_;
			dotX[1]  = doty0_;
			dotX[2]  = dotz0_;
			dotX[3]  = 0;
			dotX[4]  = 0;
			dotX[5]  = 0;
			ddotX[0] = 0;
			ddotX[1] = 0;
			ddotX[2] = 0;


		}

};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H
