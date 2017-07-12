//
// Created by robgrzel on 11.07.17.
//

#ifndef EXAM_OVEC_H
#define EXAM_OVEC_H
#include <cmath>
#include <cstdio>


class OPoint {
	
	public:
		
		double x, y, z;
		
		OPoint(const OPoint & p) : x(p.x), y(p.y), z(p.z) {
		}
		
		OPoint(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {
		
		}
		
		double mult_sum(const OPoint & p) {
			return x * p.x + y * p.y + z * p.z;
		}
		
		friend OPoint operator *(const OPoint & p, const double & s) {
			OPoint p1(p.x * s, p.y * s, p.z * s);
			return p1;
		}
		
		
		
		OPoint operator *(const OPoint & p) {
			OPoint p1(x * p.x, y * p.y, z * p.z);
			return p1;
		}
		
		
		OPoint & operator +=(const OPoint & p) {
			x += p.x;
			y += p.y;
			z += p.z;
			return * this;
		}
		
		
		OPoint operator -(const OPoint & p) {
			OPoint p1(x - p.x, y - p.y, z - p.z);
			return p1;
		}
		
		
		OPoint operator +(const OPoint & p) {
			OPoint p1(x + p.x, y + p.y, z + p.z);
			return p1;
		}
		
		OPoint operator /(const double & s) {
			OPoint p1(x / s, y / s, z / s);
			return p1;
		}
		
		double sum() {
			return x + y + z;
		}
		
		friend double sum(const OPoint & p) {
			return p.x + p.y + p.z;
		}
		
		friend OPoint operator ^(const OPoint & p, double n) {
			OPoint p1(pow(p.x, n), pow(p.y, n), pow(p.z, n));
			return p1;
		}
};


class OVec {
	
	public:
		
		OPoint a;
		OPoint b;
		OPoint c;
		
		OPoint V;
		OPoint uV;
		double mV = 0;
		
		enum vec_type{
			pts2vec_t = 0,
			vec2pts_t = 1,
		};
		
		OVec(double x0, double y0, double z0, double x1, double y1, double z1, vec_type vectype = pts2vec_t) :
				a(x0, y0, z0), b(x1, y1, z1) {
			
			switch (vectype){
				case (pts2vec_t) :{
					init_pts2vec();
					break;}
				case (vec2pts_t) :{
					init_vec2pts();
					break;}
			}
			
		};
		
		
		OVec(OPoint p1, OPoint p2, vec_type vectype = pts2vec_t) : a(p1), b(p2){
			
			switch (vectype){
				case (pts2vec_t) :{
					init_pts2vec();
					break;}
				case (vec2pts_t) :{
					init_vec2pts();
					break;}
			}
			
			
		}
		
		void init_pts2vec(){
			centroid();
			pts2vec();
			mag();
			unit();
		}
		
		
		void init_vec2pts(){
			
			centroid();
			vec2pts();
			mag();
			unit();
		}
		
		OPoint & centroid() {
			c = (a + b) / 2;
			return c;
			
		}
		
		OPoint pts2vec() {
			V = b - a;
			return V;
		}
		
		OPoint vec2pts() {
			V = b;
			return V;
		}
		
		
		
		double mag() {
			mV = sqrt(sum(V ^ 2));
			return mV;
		}
		
		OPoint unit() {
			uV = V / mV;
			return uV;
		}
		
		double cross(const OVec & v) {
			return V.mult_sum(v.V);
		}
		
		OVec projvec(const OVec & v) {
			double m2 = pow(mV, 2);
			double cr = cross(v);
			double x = cr / m2;
			
			printf("m=%f, m2=%f, cr=%f, x=%f\n",mV,m2,cr,x);
			OPoint b = v.b*x;
			OVec vt = OVec(v.a, b,vec2pts_t);
			return vt;
		}
		
		
		OVec projplane(const OVec & v) {
			OVec vt = projvec(v);
			OPoint a = V - vt.V;
			OVec vn(vt.b, a, vec2pts_t);
			return vn;
		}
};


#endif //EXAM_OVEC_H
