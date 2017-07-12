//
// Created by robgrzel on 27.06.17.
//

#ifndef EXAM_OCYLINDERSTUCK_H
#define EXAM_OCYLINDERSTUCK_H


#include <cmath>
#include <cstdio>
#include <vector>
#include <string>
#include "UtilSources/Math/env.h"


/*
 *******************************************************************************************
 * Lecture 15A.1, Offshore Structures: General Introduction
    2.2.3 Wave forces on structural members
 
 *******************************************************************************************
 * Water Wave Kinematics.pdf - 651
 * MORISON TYPE WAVE LOADING
	Professor Geir Moe
	The Norwegian Institute of Technology
	Division of Structural Engineering
	N-7D34 Trondheim - NTH
	Norway
 
 *******************************************************************************************
 * WAVE FORCES ON CYLINDERS - ANTHONY GRAHAM DIXON
 *
 *******************************************************************************************
 * OffshoreHydromechanics_Journee_Massie
 *
 *
 *  How can this be generalized in the light of the above information on currents for a cylinder
	having a non-vertical orientation? The following steps are suggested in which the inertia
	force and the drag force are considered to be separate entities until the end. The following
	sequence of steps must be carried out sequentially and at each time step for which results
	are desired::
	1. Determine the instantaneous water kinematics: velocityand acceleration (magnitudes
	as well as directions) in a …xed x; y; z axis system. Relate their phase to that of a
	reference such as the wave pro…le.
	2. Knowing the cylinder axis orientation (in that same x; y; z axis system), determine the
	instantaneous cone angles, ·I and ·D for the acceleration and velocity respectively.
	3. Determine the instantaneous perpendicular components of acceleration and velocity
	- u_
	p and up - as well as their directions. Use the results from the two previous steps
	to do this. These two vectors will not generally be co-linear; they are both in the
	plane perpendicular to the cylinder axis, however.
	4. Evaluate the inertia and drag force components at each instant. Don’t forget that
	the drag is quadratic! The direction of each force component will correspond to that
	of its associated kinematics found in the previous step.
	5. If the force on an entire member is needed, then now is the time to integrate these
	separate force components over the length of the member in order to determine each
	resultant at that time. The member support forces - and thus the equivalent loads
	to be applied at the structure nodes - can be found by treating each member as a
	simple beam.12-24 CHAPTER 12. WAVE FORCES ON SLENDER CYLINDERS
	6. Since the inertia and drag force components are not generally colinear, they can
	be combined (if desired) via vector addition to yield the resulting force magnitude
	and its direction (still in a plane perpendicular to the cylinder axis). This step is
	not absolutely necessary for the computation of resulting forces on a large structure,
	however.
	Note that these …ve or six steps must be repeated at each instant in time. These steps are
	not di¢cult in principle, but very careful bookkeeping is essential!
	In many simple cases, each of the quantities needed for this methodology will be expressible in terms of nicely behaved and convenient functions so that the resulting force can be
	described as one or another continuous time function. On the other hand, if the wave is
	irregular and thus composed of many frequency and direction components, then the necessary bookkeeping becomes too cumbersome for a hand calculation. The only requirement
	for the force computation is that the water acceleration and velocity be known at any time.
	
    * Special Orientations
	One can check his or her understanding of the above by evaluating the forces acting on
	two special cases of a horizontal cylinder in a regular wave. These are in addition to the
	vertical cylinder used during the Morison equation derivation.
	If the horizontal cylinder segment is oriented with its axis parallel to the direction
	of wave propagation (and thus perpendicular to the wave crests), then it will experience
	a vertical force which has a time trace which looks much like that for a vertical cylinder -
	see …gure 12.2. This force record will be shifted 90± in phase relative to a similar record
	for a vertical cylinder, however. The relative phases of the resulting drag and inertial force
	components on consecutive segments of the cylinder will correspond - with some constant
	shift - to that of the wave pro…le on the sea surface.
	The second case has the horizontal cylinder turned parallel to the wave crests (and
	thus perpendicular to the direction of wave propagation). If the cylinder is situated in a
	deep water wave (in which the horizontal and vertical kinematic components have the same
	magnitudes) then one will …nd a resultant force of constant magnitude which sweeps around
	the cylinder once per wave period. It may seem strange, but the horizontal component of
	this force will have a purely sinusoidal form (except for a possible phase shift) independent
	of the fact that quadratic drag is involved. Force components on consecutive segments of
	this cylinder will have identical phases in this case as compared to the previous one.
 *
 *
 */

class OMorisonEquation {
		template<class Data_t, class TResult_t> friend
		class OSysObjEnv;
		
		/*
		 * Lecture 15A.1, Offshore Structures: General Introduction
		 In this form the equation is valid for fixed tubular cylinders. For the analysis of the motion
		response of a structure it has to be modified to account for the motion of the cylinder [10]. The
		values of CD and CM depend on the wave theory used, surface roughness and the flow
		parameters. According to API-RP2A, CD ≈ 0,6 to 1,2 and CM ≈ 1,3 to 2,0. Additional
		information can be found in the DNV rules [4].
		
		* WAVE FORCES ON CYLINDERS - ANTHONY GRAHAM DIXON - 48
		 Chakrabarti/(1975) has shown that for many years the
		wrong expression has been used to predict the forces on a
		horizontal cylinder. Most workers had used the following
		equations,
		Fx = Cm p V üx +Cd rho A ux |ux|, (2.3a)
		Fy = CM P V uy + Cd rho A uy |uy| (2.3b)
		Chakrabarti demonstrates that this is incorrect and that
		the forces should be written,
		Fx = Cm p V üx +Cd rho A ux |_w|, (2.4a)
		Fy = CM P V uy + Cd rho A uy |_w| (2.4b)
		As w involves both u and u it can be seen that the force
		in the x direction involves the velocity in the y direction
		and vice versa. t is the drag term only that is affected,
		the inertial one remaining the same. The values of Cm and
		C will therefore be slightly modified, and care must be
		taken when interpreting other work to check which set of
		equations (2.3 or 2.4) have been employed. For a vertical
		cylinder = ju, and so equation (2.3a) is correct.
		 */
	
	protected:
		
		double draft0 = 0;
		double draft;
		double volume;
		
		double diameter = 0;
		double height = 0;
		
		double lineWaterline = 0;
		double areaWaterline = 0;
		double mass = 0;
		double massAdded;
		
		double Rho = 1025;
		double Grav = 9.81;
		
		double lineProjected[2] = {0, 0};
		double areaProjected[2] = {0, 0};
		double massProjected[2] = {0, 0};
		double dmassProjected[2] = {0, 0};
		
		
		double x[3], dx[3];
		double dotx[3], dotxj[3];
		double ddotx[3];
		
		int nsubs = 2;
		
		double re, KC, Beta;
		double Ca, Cm, Cd;
		double b11, b22, b33;
		
		double fslam[3], qlift[3], ffrkr[3];
		double fmoments[3], forces[3], faddm[3];
		double fdamp[3], frest[3];
		double fdrag[3], finer[3];
		double fwavez;
		
		double dt;
		
		int i, j;
		
		wave_load_types waveLoadType;
		structure_volume_types structureVolumeType;
	
	public:
		double line_of_waterline();
		
		double * line_projected();
		
		double * dmass_projected();
		
		double CDrag();
		
		double CInertia();
		
		double displaced_vol();
		
		double Draft(double zeta);
		
		double Fwave(double Eta);
		
		double * Flift(double dotu = 0, double dotv = 0, double dotw = 0);
		
		double * Fslam(double dotu = 0, double dotv = 0, double dotw = 0);
		
		double * Fdrag(double dotu = 0, double dotv = 0, double dotw = 0);
		
		double * Ffrkr(double ddotu = 0, double ddotv = 0, double ddotw = 0);
		
		double * Finer();
		
		double * Faddm();
		
		double * Forces();
		
		double * Fmoments();
		
		double Keulegan_Carpenter(double T, double dotuax);
		
		double Sarpkaya();
		
		double Reynold(double dotuax, double Kvc);
		
		double * Accel();
		
		double * Veloc();
		
		double * Displ();
		
		double * Frest();
		
		double * Fdamp();
		
		
		void intg_forces(double F[3], double q[3]) {
			F[0] += q[0] * dx[0];
			F[1] += q[1] * dx[1];
			F[2] += q[2] * dx[2];
		}
		
		void intg_moments(double M[3], double q[3]) {
			M[0] += q[0] * dx[0];
			M[1] += q[1] * dx[1];
			M[2] += q[2] * dx[2];
		}
	
	
	protected:
		void Params();
		
		double displaced_mass();
		
		double area_of_waterline();
		
		double * area_projected();
		
		double * mass_projected();
		
		double Bas_sub();
	
	public:
		
		void check_small_structure(double H, double lambda);
		
		inline void print_step() {
			printf("element  : i=%d ::: [..."
					       "t=%.2f, z=%.6f, D=%f, d0=%.5f,  nsubs=%d, Kvc=%.5e, re=%.5f, KC=%.5f, Beta=%.5f, Cm=%.5f, Cd=%.5f,\n"
					       " Fsum=[%.1f,%.1f,%1f], finer=[%.4f,%.4f,%.4f], faddm=[%.4f,%.4f,%.4f], fdrag=[%.6f,%.6f,%.6f],\n"
					       " frest=[%.4f,%.4f,%.4f], fdamp=[%.4f,%.4f,%.4f], fwavez=%4.f, x=[%.4f,%.4f,%.4f], dotx=[%.4f,%.4f,%.4f], \n"
					       " ddotx=[%.4f,%.4f,%.4f] ...]\n",
			       i, i * dt, x[2], diameter, draft0, nsubs, Kvc, re, KC, Beta, Cm, Cd, forces[0], forces[1], forces[2],
			       finer[0], finer[1], finer[2], faddm[0], faddm[1], faddm[2], fdrag[0], fdrag[1], fdrag[2],
			       frest[0], frest[1], frest[2], fdamp[0], fdamp[1], fdamp[2], fwavez, x[0], x[1], x[2], dotx[0],
			       dotx[1], dotx[2],
			       ddotx[0], ddotx[1], ddotx[2]
			);
		}
		
		inline void set_step(int i_) {
			
			i = i_;
			j = i + 1;
			
		}
		
		
		inline double get_d_xyz(int i) {
			return dx[i];
		}
		
		inline void set_x(double a) {
			x[0] = a;
		}
		
		inline void set_y(double a) {
			x[1] = a;
		}
		
		inline void set_z(double a) {
			x[2] = a;
		}
		
		inline void set_xyz(double a, double b, double c) {
			x[0] = a;
			x[1] = b;
			x[2] = c;
		}
		
		inline void set_d_xyz(double a, double b, double c) {
			dx[0] = a;
			dx[1] = b;
			dx[2] = c;
		}
		
		inline void set_dot_xyz(double a, double b, double c) {
			dotx[0] = a;
			dotx[1] = b;
			dotx[2] = c;
		}
		
		inline void set_damping(double a, double b, double c) {
			b11 = a;
			b22 = b;
			b33 = c;
		}
		
		inline void set_morison_coefs(double a, double b) {
			Cd = a;
			Ca = b;
			Cm = Ca != -1 ? b + 1 : -1;
		}
		
		inline void
		initial_conditions(double x = 0, double y = 0, double z = 0, double dx = 1, double dy = 1, double dz = 1,
		                   double dotx = 0, double doty = 0, double dotz = 0, double Cd = -1, double Ca = -1,
		                   double b11 = 0, double b22 = 0, double b33 = 0) {
			
			for (int i = 0; i < 3; i++) {
				fmoments[i] = 0;
				forces[i] = 0;
				faddm[i] = 0;
				fdamp[i] = 0;
				frest[i] = 0;
				fdrag[i] = 0;
				finer[i] = 0;
			}
			
			Bas_sub();
			set_morison_coefs(Cd, Ca);
			set_damping(b11, b22, b33);
			set_dot_xyz(dotx, doty, dotz);
			set_d_xyz(dx, dy, dz);
			set_xyz(x, y, z);
			
			Params();
			
			
			i = 0;
		}
	
	
	public:
		
		~OMorisonEquation() {
		
		};
		
		OMorisonEquation() {
		
		};
		
		OMorisonEquation(
				double diameter, double draft0, double height, double Rho, double Grav, double dt
		) :
				diameter(diameter), draft0(draft0), height(height), Rho(Rho), Grav(Grav), dt(dt) {
			
			
		};
	
	
};


#endif //EXAM_OCYLINDERSTUCK_H
