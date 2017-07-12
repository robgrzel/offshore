//
// Created by robgrzel on 12.05.17.
//

#include "OSubSystemWaves.h"


OSubSystemWaves operator +( OSubSystemWaves& os, OTime& ot ) {
	os.set_otime( &ot );
	return os;

};

OSubSystemWaves operator +( OSubSystemWaves& os, std::vector<OWave*>& vecOw ) {
	os.set_vec_owave( vecOw );
	return os;

};

OSubSystemWaves operator +( OSubSystemWaves& os, VElem& oe ) {
	os.set_velem( &oe );
	return os;
};

OSubSystemWaves operator +( OTime& ot, OSubSystemWaves& os ) {
	os.set_otime( &ot );
	return os;
};

OSubSystemWaves operator +( OTime& ot, std::vector<OWave*>& vecOw ) {
	OSubSystemWaves os( &ot, { }, &vecOw );
	return os;
};

OSubSystemWaves operator +( OTime& ot, VElem& oe ) {
	OSubSystemWaves os( &ot, &oe, { } );
	return os;
};

OSubSystemWaves operator +( std::vector<OWave*>& vecOw, OTime& ot ) {
	OSubSystemWaves os( &ot, { }, &vecOw );
	return os;
};

OSubSystemWaves operator +( std::vector<OWave*>& vecOw, OSubSystemWaves& os ) {
	os.set_vec_owave( vecOw );
	//os.reset_vecs();
	return os;
};

OSubSystemWaves operator +( std::vector<OWave*>& vecOw, VElem& oe ) {
	OSubSystemWaves os( { }, &oe, &vecOw );
};

OSubSystemWaves operator +( VElem& oe, OSubSystemWaves& os ) {
	os.set_velem( &oe );
	//os.reset_vecs();

	return os;
};

OSubSystemWaves operator +( VElem& oe, OTime& ot ) {
	OSubSystemWaves os( &ot, &oe, { } );
	return os;
};

OSubSystemWaves operator +( VElem& oe, std::vector<OWave*>& vecOw ) {
	OSubSystemWaves os( { }, &oe, &vecOw );
	return os;
};

void OSubSystemWaves::operator +( std::vector<OWave*>& vecOw ) {
	set_vec_owave( vecOw );
	//reset_vecs();

};

void OSubSystemWaves::operator +( VElem& oe ) {
	set_velem( &oe );
	//reset_vecs();
};

void OSubSystemWaves::operator +( OTime& ot ) { //outside the class
	set_otime( &ot );
	//reset_vecs();

};

void OSubSystemWaves::operator +=( OTime& ot ) { //outside the class
	set_otime( &ot );
	//reset_vecs();

};


void OSubSystemWaves::operator +=( OWave& ow ) { //outside the class
	add_owave( &ow );
};


void OSubSystemWaves::operator +=( VElem& oe ) { //outside the class
	set_velem( &oe );
};


void OSubSystemWaves::operator ()( const int i, const int j ) {
	set_wave( i );
	set_step( j );

}

void OSubSystemWaves::set_otime( OTime* ot ) {
	this->ot       = ot;
	this->numSteps = ot->numSteps;


}

void OSubSystemWaves::del_otime( ) {
	this->ot = nullptr;
	hasOTime = false;
	this->numSteps = 0;


}

void OSubSystemWaves::set_vec_owave( std::vector<OWave*>& vecOw ) {
	this->vecOw = vecOw;
	numWaves = vecOw.size();
	if (!vecOw.empty()) {
		hasOWave = true;
	}

}

void OSubSystemWaves::set_velem( VElem* oe ) {
	this->oe = oe;
	hasVElem = true;
}

void OSubSystemWaves::del_velem( ) {
	this->oe = nullptr;
	hasVElem = false;


}


void OSubSystemWaves::add_owave( OWave* ow ) {
	this->ow = ow;
	vecOw.push_back( ow );
	hasOWave = true;
	numWaves++;
	idWave++;
}

void OSubSystemWaves::pop_owave( ) {
	vecOw.pop_back();
	numWaves--;
	if (vecOw.empty()) {
		hasOWave = false;
	} else {
		this->ow = vecOw.back();
	}
}

void OSubSystemWaves::set_wave( const int i ) {
	ow = vecOw[i];
}


void OSubSystemWaves::set_step( const int i_ ) {
	i = i_;
	j = i + 1;
	t = ot->set_step( i );

	ow->set_step_wave( i );
}

void OSubSystemWaves::print_wave( const int i, const int j ) {

	operator ()( i, j );

	ow->print_wave();
}

void OSubSystemWaves::calc( ) {

	const int numBuffer = 10;//2160000000/sizeof(struct)/numSteps;
	//TODO
	//struct tmp[numBuffer][numSteps] //max size of buffer should be : numBuffer= 2160000000/sizeof(struct)/numSteps; 2gb = 2160000000
	//struct resultWaves[numSteps]
	for ( int i = 0, b = 0; i < numWaves; i++, b++ ) {
		for ( int j = 0; j < numSteps; j++ ) {
			calc_step( i, j );
		}

		if (b == numBuffer) {
			for ( int j = 0; j < numBuffer; j++ ) {
				//resultWaves += tmp[j]
			}
			b = 0;

		} else {
			//tmp[b]= ....
		}
	}

}

void OSubSystemWaves::calc_step( const int i, const int j ) {

	operator ()( i, j );

	tmp[0] = ow->get_force();
	tmp[1] = ow->get_velocity( 0 );
	tmp[2] = ow->get_velocity( 1 );
	tmp[3] = ow->get_velocity( 2 );
	tmp[4] = ow->get_acceleration( 0 );
	tmp[5] = ow->get_acceleration( 1 );
	tmp[6] = ow->get_acceleration( 2 );

	oe->set_step( j, tmp );
	oe->calc_step_part_submerged();

	tmp[7] = oe->get_displacement( 0 );
	tmp[8] = oe->get_displacement( 1 );
	tmp[9] = oe->get_displacement( 2 );


	ow->set_step_particle( j, tmp[7], tmp[8], tmp[9] );

	ow->calc_step_particle();


}

void OSubSystemWaves::get_step_current_elem( double data[12] ) {

	tmp[10]  = oe->get_velocity( 0 );
	tmp[11]  = oe->get_velocity( 1 );
	tmp[12] = oe->get_velocity( 2 );

	tmp[13] = oe->get_acceleration( 0 );
	tmp[14] = oe->get_acceleration( 1 );
	tmp[15] = oe->get_acceleration( 2 );


	tmp[16] = oe->get_forces( 0 );
	tmp[17] = oe->get_forces( 1 );
	tmp[18] = oe->get_forces( 2 );


	//tmp[19] = oe->get_momentum(0);
	//tmp[20] = oe->get_momentum(1);
	//tmp[21] = oe->get_momentum(2);


	for ( int i = 0, j = 7; i < 12; i++, j++ ) {
		data[i] = tmp[j];
	}


};


void OSubSystemWaves::get_step_current( double data[22] ) {


	tmp[10]  = oe->get_velocity( 0 );
	tmp[11]  = oe->get_velocity( 1 );
	tmp[12] = oe->get_velocity( 2 );

	tmp[13] = oe->get_acceleration( 0 );
	tmp[14] = oe->get_acceleration( 1 );
	tmp[15] = oe->get_acceleration( 2 );


	tmp[16] = oe->get_forces( 0 );
	tmp[17] = oe->get_forces( 1 );
	tmp[18] = oe->get_forces( 2 );


	tmp[19] = oe->get_momentum(0);
	tmp[20] = oe->get_momentum(1);
	tmp[21] = oe->get_momentum(2);


	for ( int i = 0; i < 22; i++ ) {
		data[i] = tmp[i];
	}

}

void OSubSystemWaves::print_current_step( ) {
	get_step_current( tmp );

	printf( "SYSTEM : i=%d, t=%f, OW(Fu=%f, Vu=[%f,%f,%f], Au=[%f,%f,%f]), "
			        "OE(F=[%f,%f,%f], U=[%f,%f,%f], V=[%f,%f,%f], A=[%f,%f,%f]\n)\n",
	        i, t, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6],
	        tmp[10], tmp[11], tmp[12], tmp[7], tmp[8], tmp[9],
	        tmp[13], tmp[14], tmp[15], tmp[16], tmp[17], tmp[18]

	      );


}

void OSubSystemWaves::initial_conditions( int N, double dt, double d0, double x0, double y0, double z0, double dotx0, double doty0, double dotz0 ) {

	numWaves = N;
	tmp[0] = dt;
	tmp[1] = d0;
	tmp[2] = x0;
	tmp[3] = y0;
	tmp[4] = z0;
	tmp[5] = dotx0;
	tmp[6] = doty0;
	tmp[7] = dotz0;


	oe->initial_conditions( N, tmp );

	tmp[0] = oe->get_area_waterline();

	ow->wave_boundary_conditions( N, dt, tmp[0] );

	calc_wave();

	//oe->calc_step_part_submerged();

	ow->particle_boundary_conditions( d0, x0, y0, z0 );

	ow->calc_step_particle();

};

void OSubSystemWaves::calc_wave( ) {

	for ( int i = 0; i < numSteps; i++ ) {
		set_step( i );

		ow->calc_step_wave();

	}
	set_step( 0 );

}

OSubSystemWaves::OSubSystemWaves( ) : oe( nullptr ) {


}

OSubSystemWaves::OSubSystemWaves( OTime* ot ) {

	set_otime( ot );
	//reset_vecs();

}


OSubSystemWaves::OSubSystemWaves( VElem* oe ) {

	set_velem( oe );
	//reset_vecs();

}

OSubSystemWaves::OSubSystemWaves( OTime* ot, VElem* oe ) {

	set_velem( oe );
	set_otime( ot );
	//reset_vecs();

}

OSubSystemWaves::OSubSystemWaves( OTime* ot, VElem* oe, std::vector<OWave*>* vecOw ) {

	set_otime( ot );
	set_velem( oe );
	set_vec_owave( *vecOw );

	//reset_vecs();

}


OSubSystemWaves::~OSubSystemWaves( ) {
	//std::cout << "OSubSystemWaves::OSubSystemanager\n";

}

OSubSystemWaves& OSubSystemWaves::instance( ) {
	//std::cout << "OSubSystemWaves& OSubSystemWaves::instance( )\n";

	static OSubSystemWaves os;
	return os;
}

