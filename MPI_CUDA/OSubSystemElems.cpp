//
// Created by robgrzel on 12.05.17.
//

#include "OSubSystemElems.h"


OSubSystemElems operator +( OSubSystemElems& os, OTime& ot ) {
	os.set_otime( &ot );
	return os;

};

OSubSystemElems operator +( OSubSystemElems& os, std::vector<VElem*>& vecOe ) {
	os.set_vec_velem( vecOe );
	return os;

};

OSubSystemElems operator +( OSubSystemElems& os, OWave & ow ) {
	os.set_owave( &ow );
	return os;
};

OSubSystemElems operator +( OTime& ot, OSubSystemElems& os ) {
	os.set_otime( &ot );
	return os;
};

OSubSystemElems operator +( OTime& ot, std::vector<VElem*>& vecOe ) {
	OSubSystemElems os( &ot,  { },&vecOe);
	return os;
};

OSubSystemElems operator +( OTime& ot, OWave & ow ) {
	OSubSystemElems os( &ot, &ow, { }  );
	return os;
};

OSubSystemElems operator +( std::vector<VElem*>& vecOe, OTime& ot ) {
	OSubSystemElems os( &ot, {}, &vecOe );
	return os;
};

OSubSystemElems operator +( std::vector<VElem*>& vecOe, OSubSystemElems& os ) {
	os.set_vec_velem( vecOe );
	//os.reset_vecs();
	return os;
};

OSubSystemElems operator +( std::vector<VElem*>& vecOe, OWave & ow ) {
	OSubSystemElems os( { }, &ow, &vecOe );
};

OSubSystemElems operator +( OWave & ow, OSubSystemElems& os ) {
	os.set_owave( &ow );
	//os.reset_vecs();

	return os;
};

OSubSystemElems operator +( OWave & ow, OTime& ot ) {
	OSubSystemElems os( &ot, &ow,{} );
	return os;
};

OSubSystemElems operator +( OWave & ow, std::vector<VElem*>& vecOe ) {
	OSubSystemElems os( { }, &ow, &vecOe );
	return os;
};

void OSubSystemElems::operator +( std::vector<VElem*>& vecOe ) {
	set_vec_velem( vecOe );
	//reset_vecs();

};

void OSubSystemElems::operator +( OWave & ow ) {
	set_owave( &ow );
	//reset_vecs();
};

void OSubSystemElems::operator +( OTime& ot ) { //outside the class
	set_otime( &ot );
	//reset_vecs();

};
void OSubSystemElems::operator +=( OTime& ot ) { //outside the class
	set_otime( &ot );
	//reset_vecs();

};


void OSubSystemElems::operator +=( VElem& oe ) { //outside the class
	add_velem( &oe );
};


void OSubSystemElems::operator +=( OWave& ow ) { //outside the class
	set_owave( &ow );
};


void OSubSystemElems::operator ()( size_t i) {
	set_elem(i);
}

void OSubSystemElems::set_otime( OTime* ot ) {
	this->ot       = ot;
	this->numSteps = ot->numSteps;


}

void OSubSystemElems::del_otime( ) {
	this->ot = nullptr;
	hasOTime = false;
	this->numSteps = 0;


}

void OSubSystemElems::set_vec_velem( std::vector<VElem*>& vecOe ) {
	this->vecOe = vecOe;
	numElems = vecOe.size();
	if (!vecOe.empty()) {
		hasVElem = true;
	}

}

void OSubSystemElems::set_owave( OWave * ow ) {
	this->ow = ow;
	hasOWave = true;
}

void OSubSystemElems::del_owave( ) {
	this->ow = nullptr;
	hasOWave = false;


}


void OSubSystemElems::add_velem( VElem* oe ) {
	vecOe.push_back( oe );
	hasVElem = true;
	numElems++;
	idElem++;
}

void OSubSystemElems::pop_velem( ) {
	vecOe.pop_back();
	numElems--;
	if (vecOe.empty()) {
		hasVElem = false;
	}
}


void OSubSystemElems::set_elem(size_t i) {
	oe = vecOe[i];
}

void OSubSystemElems::set_step(size_t i_){

	i  = i_;
	j  = i_ + 1;
	ow->set_step_wave(i);
	ot->set_step(i);

}

void OSubSystemElems::calc_step(  ) {

	ow->calc_step_particle();

	tmp[0]=ow->get_force();
	tmp[1]=ow->get_velocity(0);
	tmp[2]=ow->get_velocity(1);
	tmp[3]=ow->get_velocity(2);
	tmp[4]=ow->get_acceleration(0);
	tmp[5]=ow->get_acceleration(1);
	tmp[6]=ow->get_acceleration(2);

	oe->set_step(i,tmp);
	oe->calc_step_part_submerged();

	tmp[0] = oe->get_displacement(0);
	tmp[1] = oe->get_displacement(1);
	tmp[2] = oe->get_displacement(2);

	ow->set_step_particle(i,tmp[0],tmp[1],tmp[2]);


}

void OSubSystemElems::initial_conditions(size_t N, double dt, double x0, double y0, double z0){

	tmp[0]=z0;
	tmp[1]=dt;
	oe->initial_conditions(N,tmp);

	tmp[0]=oe->get_area_waterline();
	tmp[1]=oe->get_draught_init();
	ow->wave_boundary_conditions(N,dt,tmp[0]);
	ow->particle_boundary_conditions(tmp[1],x0,y0,z0);

};



OSubSystemElems::OSubSystemElems( ) : oe( nullptr ) {


}

OSubSystemElems::OSubSystemElems( OTime* ot) {

	set_otime( ot );
	//reset_vecs();

}


OSubSystemElems::OSubSystemElems( OWave* ow) {

	set_owave( ow);
	//reset_vecs();

}

OSubSystemElems::OSubSystemElems( OTime* ot, OWave * ow ) {

	set_owave(ow);
	set_otime( ot );
	//reset_vecs();

}

OSubSystemElems::OSubSystemElems( OTime* ot, OWave * ow, std::vector<VElem*>* vecOe ) {

	set_otime( ot );
	set_owave( ow );
	set_vec_velem(* vecOe);

	//reset_vecs();

}


OSubSystemElems::~OSubSystemElems( ) {
	//std::cout << "OSubSystemElems::OSubSystemanager\n";

}

OSubSystemElems& OSubSystemElems::instance( ) {
	//std::cout << "OSubSystemElems& OSubSystemElems::instance( )\n";

	static OSubSystemElems os;
	return os;
}

