//
// Created by robgrzel on 12.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_ELEMS_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_ELEMS_H

#include <map>
#include "OWave.h"
#include "OWaves.h"
#include "OTime.h"
#include "VElem.h"
#include "UtilSources/Containers/std_vector_3d.h"

#define PI 3.1415926536

#define OSUBSYSTEME (OSubSystemElems::instance())


class OSubSystemElems {


	private:

		friend OSubSystemElems operator +( OSubSystemElems& os, OTime& ot );

		friend OSubSystemElems operator +( OSubSystemElems& os, std::vector<VElem*>& vecOe );

		friend OSubSystemElems operator +( OSubSystemElems& os, OWave& ow );

		friend OSubSystemElems operator +( OTime& ot, OSubSystemElems& os );

		friend OSubSystemElems operator +( OTime& ot, std::vector<VElem*>& vecOe );

		friend OSubSystemElems operator +( OTime& ot, OWave& ow );

		friend OSubSystemElems operator +( std::vector<VElem*>& vecOe, OTime& ot );

		friend OSubSystemElems operator +( std::vector<VElem*>& vecOe, OSubSystemElems& os );

		friend OSubSystemElems operator +( std::vector<VElem*>& vecOe, OWave& ow );

		friend OSubSystemElems operator +( OWave& ow, OSubSystemElems& os );

		friend OSubSystemElems operator +( OWave& ow, OTime& ot );

		friend OSubSystemElems operator +( OWave& ow, std::vector<VElem*>& vecOe );


	public:

		void operator ()( size_t i );

		void operator +( std::vector<VElem*>& vecOe );

		void operator +( OWave& ow );

		void operator +( OTime& ot );

		void operator +=( OTime& ot );

		void operator +=( VElem& ob );

		void operator +=( OWave& ow );

	private:

		void set_otime( OTime* ot );

		void set_owave( OWave* ow );

		void del_owave( );

		void del_otime( );

		void set_vec_velem( std::vector<VElem*>& vecOe );

		void add_velem( VElem* ob );

		void pop_velem( );

	public:

		enum SYS_TYPES {
			SYS_BUOYS,
			SYS_PILLAR,
		};


		void set_elem( size_t i );

		void set_step( size_t i );

		void set_step_time( const size_t& i ) {
			ot->set_step( i );
		}

		void calc_step( );

		size_t num_steps( ) {
			return numSteps;
		}

		size_t num_elems( ) {
			return numElems;
		}

		void initial_conditions( size_t N, double dt, double x0, double y0, double z0 );

		OSubSystemElems( OTime* ot, OWave* ow, std::vector<VElem*>* vecOe );

		OSubSystemElems( OTime* ot, OWave* ow );

		OSubSystemElems( OTime* ot );

		OSubSystemElems( OWave* ow );

		OSubSystemElems( );

		~OSubSystemElems( );

		static OSubSystemElems& instance( );

	private:
		size_t i, j, k;
		double t, tmp[10];

	private:

		size_t idElem   = NULL;
		size_t idWave   = NULL;
		size_t idTime   = NULL;
		size_t numElems = 0;
		size_t numSteps = 0;
		bool   hasOTime = false;
		bool   hasOWave = false;
		bool   hasVElem = false;

		std::vector<double> vecOt;
		std::vector<VElem*> vecOe;

		SYS_TYPES systemType;

		VElem* oe;
		OTime* ot;
		OWave* ow;

};

OSubSystemElems operator +( OTime& ot );

OSubSystemElems operator +( OWave& ow );

OSubSystemElems operator +( std::vector<VElem*>& vecOe );


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_ELEMS_H
