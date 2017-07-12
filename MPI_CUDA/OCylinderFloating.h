//
// Created by robgrzel on 12.05.17.
//

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H


#include <cmath>
#include <cstdio>
#include <vector>
#include <string>


class GCylinderFloating {

public:
	double diameter = 0;
	double height = 0;
	double draughtInit = 0;
	double areaProjected = 0;
	double areaWaterline = 0;
	double massInit = 0;
	double massDisp = 0;
	double Rho = 1025;
	double Grav = 9.81;
	double Dvc = 1.08/1e3;
	double Kvc;
	
protected:
	void geometry();
	
	void init();

public:
	
	double get_diameter();
	
	void displaced_mass();
	
	void area_of_waterline();
	
	void area_projected_to_motion_direction();
	
	void kinematic_viscosity(){
		Kvc = Dvc/Rho;
	};
	
	
	GCylinderFloating();
	
	GCylinderFloating(double diameter, double draughtInit, double height, double Rho, double Grav, double Dvc);
	
	
};



class OCylinderFloating : public GCylinderFloating {
	friend class OSubSystemWaves;

private:
	
	double coefAddmass, coefMass, coefDamp, coefDrag, coefLift;
	
	double massAdded, Re, KC, Beta;
	
	//double forces, fdrag, fdamp, frest, Fmass, Flift;
	//double ddotX, dotZi, dotZj, x, dt;
	
	std::vector<double> M, F, Fdrag, Fdamp, Frest, Faddm, Flift;
	std::vector<double> ddotX, dotX, X;
	
	//wave data
	double Ua[3], Ui[3], dotUi[3], Fwavei, dt;
	
	int i, j, numSteps;
	
	double T;


protected:
	
	void acceleration();
	
	void velocity();
	
	void displacement();
	
	void force_damping();
	
	void force_drag_part_submerged();
	
	void force_drag_full_submerged();
	
	void force_stiffness();
	
	void force_added_mass2();
	
	void forces_sum();
	
	void coef_mass();
	
	void coef_drag_part_submerged();
	
	void coef_drag_full_submerged();
	
	void force_lift();
	
	
	void keulegan_carpenter_number();
	
	void sarpkaya_beta();
	
	void reynolds_number();

public:
	
	void save_data(std::string name);
	
	
	~OCylinderFloating() {
	
	};
	
	OCylinderFloating() : GCylinderFloating(), isInitialized(false) {
	
	};
	
	OCylinderFloating(
			double coefAddmass, double coefDrag, double coefDamp, double diameter, double draughtInit,
			double height, double Rho, double Grav, double T, double Dvc
	) :
			GCylinderFloating(diameter, draughtInit, height, Rho, Grav, Dvc), isInitialized(true),
			coefAddmass(coefAddmass), coefMass(1 + coefAddmass),
			coefDrag(coefDrag), coefDamp(coefDamp), T(T) {
		
	};


private:
	
	bool isInitialized;

public:
	
	
	inline const bool &is_initialized() {
		return isInitialized;
	};
	
	
	inline const double &get_area_waterline() {
		return areaWaterline;
	}
	
	inline const double &get_draught_init() {
		return draughtInit;
	}
	
	inline const double &get_displacement(int j) {
		return X[i];
	}
	
	inline const double &get_velocity(int j) {
		return dotX[i];
	}
	
	inline const double &get_acceleration(int j) {
		return ddotX[i];
	}
	
	inline const double &get_forces(int j = 0) {
		return F[i];
	};
	
	inline const double &get_momentum(int j = 0) {
		return M[i]; //TODO
	};
	
	
	inline void update(double *data = nullptr) {
		init(data);
		
	};
	
	inline void swap_displacement(std::vector<double> &X_) {
		X.swap(X_);
	}
	
	inline void swap_velocity(std::vector<double> &dotX_) {
		dotX.swap(dotX_);
	}
	
	inline void swap_acceleration(std::vector<double> &ddotX_) {
		ddotX.swap(ddotX_);
	}
	
	inline void swap_forces(std::vector<double> &F_) {
		F.swap(F_);
	};
	
	inline void swap_momentum(std::vector<double> &M_) {
		M.swap(M_);
	};


protected:
	virtual inline void init(double *data = nullptr) {
		//GCylinderFloating( diameter, draught, height, Rho, Grav ),
		
		if (data != nullptr) {
			
			coefAddmass = data[1];
			coefDrag = data[2];
			coefDamp = data[3];
			diameter = data[4];
			draughtInit = data[5];
			height = data[6];
			Rho = data[7];
			Grav = data[8];
		}
		
		GCylinderFloating::init();
		
		coef_mass();
		
	};
	
	
	virtual inline void initial_conditions(int N = 0, double *data = nullptr) {
		
		/*
			 data[0] = dt;
			data[1] = d;
			data[2] = x0;
			data[3] = y0;
			data[4] = xc;
			data[5] = dotx0;
			data[6] = doty0;
			data[7] = dotz0;
		 */
		
		if (data != nullptr) {
			initial_conditions(N, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
			return;
		}
		
		numSteps = N;
		
		
	}
	
	
	


public:
	
	inline void set_step(int i_ = 0, double *data = nullptr) {
		
		i = i_;
		j = i + 1;
		
		
		Fwavei = data[0];
		Ui[0] = data[1];
		Ui[1] = data[2];
		Ui[2] = data[3];
		dotUi[0] = data[4];
		dotUi[1] = data[5];
		dotUi[2] = data[6];
		
	}
	
	inline void set_step(int i_ = 0, double Fwavei_ = 0, double Uix = 0, double Uiy=0, double Uiz = 0, double dotUix=0, double dotUiy = 0, double dotUiz=0) {
		
		i = i_;
		j = i + 1;
		
		Fwavei = Fwavei_;
		Ui[0] = Uix;
		Ui[1] = Uiy;
		Ui[2] = Uiz;
		dotUi[0] = dotUix;
		dotUi[1] = dotUiy;
		dotUi[2] = dotUiz;
		
	}
	
	inline void calc_step_part_submerged() {
		force_damping();
		force_drag_part_submerged();
		force_stiffness();
		forces_sum();
		acceleration();
		velocity();
		displacement();
		force_added_mass2();
	}
	
	inline void print_step(){
		printf("element  : i=%d, t=%.2f ::: z=%.6f, dotz=%.5f, ddotz=%.5f, uz=%.5f,"
				       "Fsum=%.1f, Fadd2=%.4f, fdamp=%.4f, "
				       "fdrag=%.6f, Fres=%.1f, Fwave=%.2f\n",
		       i,i*dt,X[i],dotX[i],ddotX[i],Ui[2],F[i],Faddm[i],
		       Fdamp[i],Fdrag[i],Frest[i],Fwavei);
	}
	
	inline void initial_conditions(
			int N = 0, double dt_ = 0, double d0_ = 0,
			double x0_ = 0, double y0_ = 0, double z0_ = 0,
			double dotx0_ = 0, double doty0_ = 0, double dotz0_ = 0,
	        double Uax=0, double Uay=0, double Uaz=0, double T_=0
	) {
		
		numSteps = N;
		
		Flift = std::vector<double>(numSteps, 0);
		Fdrag = std::vector<double>(numSteps, 0);
		Fdamp = std::vector<double>(numSteps, 0);
		Faddm = std::vector<double>(numSteps, 0);
		Frest = std::vector<double>(numSteps, 0);
		F = std::vector<double>(numSteps, 0);
		M = std::vector<double>(numSteps, 0);
		
		ddotX = std::vector<double>(numSteps, 0);
		dotX = std::vector<double>(numSteps, 0);
		X = std::vector<double>(numSteps, 0);
		
		draughtInit = d0_;
		dt = dt_;
		
		X[0] = z0_;
		dotX[0] = dotz0_;
		dotX[1] = 0;
		ddotX[0] = 0;
		
		Ua[0]=Uax;
		Ua[1]=Uay;
		Ua[2]=Uaz;
		
		T = T_;
		
		
	}
	
};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OELEMCYLINDER_H
