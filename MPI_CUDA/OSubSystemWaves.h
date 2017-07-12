//
// Created by robgrzel on 12.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_WAVES_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_WAVES_H

#include <map>
#include "OWave.h"
#include "OTime.h"
#include "VElem.h"
#include "../UtilSources/Containers/std_vector_3d.h"


#define OSUBSYSTEMW (OSubSystemWaves::instance())


class OSubSystemWaves {


	private:

		friend OSubSystemWaves operator +( OSubSystemWaves& os, OTime& ot );

		friend OSubSystemWaves operator +( OSubSystemWaves& os, std::vector<OWave*>& vecOw );

		friend OSubSystemWaves operator +( OSubSystemWaves& os, VElem& oe );

		friend OSubSystemWaves operator +( OTime& ot, OSubSystemWaves& os );

		friend OSubSystemWaves operator +( OTime& ot, std::vector<OWave*>& vecOw );

		friend OSubSystemWaves operator +( OTime& ot, VElem& oe );

		friend OSubSystemWaves operator +( std::vector<OWave*>& vecOw, OTime& ot );

		friend OSubSystemWaves operator +( std::vector<OWave*>& vecOw, OSubSystemWaves& os );

		friend OSubSystemWaves operator +( std::vector<OWave*>& vecOw, VElem& oe );

		friend OSubSystemWaves operator +( VElem& oe, OSubSystemWaves& os );

		friend OSubSystemWaves operator +( VElem& oe, OTime& ot );

		friend OSubSystemWaves operator +( VElem& oe, std::vector<OWave*>& vecOw );


	public:

		void operator ()( const int i, const int j = 0 );

		void operator +( std::vector<OWave*>& vecOw );

		void operator +( VElem& oe );

		void operator +( OTime& ot );

		void operator +=( OTime& ot );

		void operator +=( VElem& ob );

		void operator +=( OWave& ow );

	private:

		void set_otime( OTime* ot );

		void set_velem( VElem* oe );

		void del_velem( );

		void del_otime( );

		void set_vec_owave( std::vector<OWave*>& vecOw );

		void add_owave( OWave* ow );

		void pop_owave( );

	public:

		enum SYS_TYPES {
			SYS_BUOYS,
			SYS_PILLAR,
		};

		void print_wave( const int i, const int j );

		void set_wave( const int i );

		void get_step_current( double data[22] );

		void get_step_current_elem( double data[12]);

		void set_step( const int i );

		void print_current_step( );

		void calc_wave( );

		void calc_step( const int i, const int j );

		void calc();


		void initial_conditions( int N, double dt, double d0, double x0, double y0, double z0, double dotx0, double doty0, double dotz0 );

		OSubSystemWaves( OTime* ot, VElem* oe, std::vector<OWave*>* vecOw );

		OSubSystemWaves( OTime* ot, VElem* oe );

		OSubSystemWaves( OTime* ot );

		OSubSystemWaves( VElem* oe );

		OSubSystemWaves( );

		~OSubSystemWaves( );

		static OSubSystemWaves& instance( );

	public:
		inline void set_step_time( const int& i ) {
			ot->set_step( i );
		}
		inline int num_steps( ) {
			return numSteps;
		}

		inline int num_waves( ) {
			return numWaves;
		}


	private:
		int    i, j, k;
		double t, tmp[22];

	private:

		int idElem   = 0;
		int idWave   = 0;
		int idTime   = 0;
		int numElems = 0;
		int numSteps = 0;
		int numWaves = 0;

		int tmpSize1;
		int tmpSize2;

		bool hasOTime = false;
		bool hasOWave = false;
		bool hasVElem = false;

		std::vector<double> vecOt;
		std::vector<VElem*> vecOe;
		std::vector<OWave*> vecOw;

		SYS_TYPES systemType;

	public:
		VElem* oe;
		OTime* ot;
		OWave* ow;

};

OSubSystemWaves operator +( OTime& ot );

OSubSystemWaves operator +( VElem& oe );

OSubSystemWaves operator +( std::vector<OWave*>& vecOw );


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OSUBSYSTEM_WAVES_H
