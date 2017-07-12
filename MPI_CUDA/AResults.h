//
// Created by robgrzel on 30.06.17.
//

#ifndef EXAM_ARESULTS_H
#define EXAM_ARESULTS_H


#include <cstdio>
#include <vector>
#include <string>
#include <sstream>


struct AResultTemp_t {
	double xi[3];
	double dotxi[3];
	double ddotxi[3];
	double ui[3];
	double dotui[3];
	double ddotui[3];
	double Mi[3];
	double Fi[3];
	double Fdragi[3];
	double Fdampi[3];
	double Fineri[3];
	double Fresti[3];
	double Faddmi[3];
	double Fwavei;
	double Thetai;
	double Etai;
	
	void F(double a[3]) {
		Fi[0] = a[0];
		Fi[1] = a[1];
		Fi[2] = a[2];
	}
	
	void Fwave(double a) {
		Fwavei = a;
	}
	
	void Fdrag(double a[3]) {
		Fdragi[0] = a[0];
		Fdragi[1] = a[1];
		Fdragi[2] = a[2];
	}
	
	void Finer(double a[3]) {
		Fineri[0] = a[0];
		Fineri[1] = a[1];
		Fineri[2] = a[2];
	}
	
	void Frest(double a[3]) {
		Fresti[0] = a[0];
		Fresti[1] = a[1];
		Fresti[2] = a[2];
	}
	
	void Faddm(double a[3]) {
		Faddmi[0] = a[0];
		Faddmi[1] = a[1];
		Faddmi[2] = a[2];
	}
	
	void Fdamp(double a[3]) {
		Fdampi[0] = a[0];
		Fdampi[1] = a[1];
		Fdampi[2] = a[2];
	}
	
	void M(double a[3]) {
		Mi[0] = a[0];
		Mi[1] = a[1];
		Mi[2] = a[2];
	}
	
	void x(double a[3]) {
		xi[0] = a[0];
		xi[1] = a[1];
		xi[2] = a[2];
	}
	
	void dotx(double a[3]) {
		dotxi[0] = a[0];
		dotxi[1] = a[1];
		dotxi[2] = a[2];
	}
	
	void ddotx(double a[3]) {
		ddotxi[0] = a[0];
		ddotxi[1] = a[1];
		ddotxi[2] = a[2];
	}
	
	
	void u(double a[3]) {
		ui[0] = a[0];
		ui[1] = a[1];
		ui[2] = a[2];
	}
	
	
	void dotu(double a[3]) {
		dotui[0] = a[0];
		dotui[1] = a[1];
		dotui[2] = a[2];
	}
	
	void ddotu(double a[3]) {
		ddotui[0] = a[0];
		ddotui[1] = a[1];
		ddotui[2] = a[2];
	}
	
	
	void Theta(double theta) {
		Thetai = theta;
	}
	
	void Eta(double eta) {
		Etai = eta;
	}
	
	
	friend std::istream &operator>>(std::istream &stream, AResultTemp_t t) {
		stream >> t.Fi[0]
		       >> t.Fi[1]
		       >> t.Fi[2]
		       >> t.Mi[0]
		       >> t.Mi[1]
		       >> t.Mi[2]
		       >> t.xi[0]
		       >> t.xi[1]
		       >> t.xi[2]
		       >> t.dotxi[0]
		       >> t.dotxi[1]
		       >> t.dotxi[2]
		       >> t.ddotxi[0]
		       >> t.ddotxi[1]
		       >> t.ddotxi[2]
		       >> t.ui[0]
		       >> t.ui[1]
		       >> t.ui[2]
		       >> t.dotui[0]
		       >> t.dotui[1]
		       >> t.dotui[2]
		       >> t.Thetai
		       >> t.Etai;
		
		return stream;
	}
	
	friend std::ostream &operator<<(std::ostream &stream, AResultTemp_t t) {
		stream << t.Fi[0] << " "
		       << t.Fi[1] << " "
		       << t.Fi[2] << " "
		       << t.Mi[0] << " "
		       << t.Mi[1] << " "
		       << t.Mi[2] << " "
		       << t.xi[0] << " "
		       << t.xi[1] << " "
		       << t.xi[2] << " "
		       << t.dotxi[0] << " "
		       << t.dotxi[1] << " "
		       << t.dotxi[2] << " "
		       << t.ddotxi[0] << " "
		       << t.ddotxi[1] << " "
		       << t.ddotxi[2] << " "
		       << t.ui[1] << " "
		       << t.ui[2] << " "
		       << t.dotui[0] << " "
		       << t.dotui[1] << " "
		       << t.dotui[2] << " "
		       << t.Thetai << " "
		       << t.Etai << "\n";
		
		return stream;
	}
};

struct AResultForcesMoments_t {
	double t;
	double M[3];
	double F[3];
	
	AResultForcesMoments_t &operator+=(const AResultTemp_t &r) {
		F[0] += r.Fi[0];
		F[1] += r.Fi[1];
		F[2] += r.Fi[2];
		M[0] += r.Mi[0];
		M[1] += r.Mi[1];
		M[2] += r.Mi[2];
		return *this;
	}
	
	friend std::ostream &operator<<(std::ostream &stream, const AResultForcesMoments_t &t) {
		stream << t.t << " "
		       << t.F[0] << " "
		       << t.F[1] << " "
		       << t.F[2] << " "
		       << t.M[0] << " "
		       << t.M[1] << " "
		       << t.M[2] << "\n";
		return stream;
	}
	
	friend std::istream &operator>>(std::istream &stream, AResultForcesMoments_t &t) {
		stream >> t.t >> t.F[0] >> t.F[1] >> t.F[2] >> t.M[0] >> t.M[1] >> t.M[2];
		return stream;
	}
	
};

struct AResultAll_t {
	//13*3 + 4 = 39 + 4 = 43
	double t;
	double M[3];
	double F[3];
	double Finer[3];
	double Fdrag[3];
	double Faddm[3];
	double Frest[3];
	double Fdamp[3];
	double Fwave;
	double x[3];
	double dotx[3];
	double ddotx[3];
	double u[3];
	double dotu[3];
	double ddotu[3];
	double Theta;
	double Eta;
	
	AResultAll_t &operator+=(const AResultTemp_t &r) {
		x[0] += r.xi[0];
		x[1] += r.xi[1];
		x[2] += r.xi[2];
		
		dotx[0] += r.dotxi[0];
		dotx[1] += r.dotxi[1];
		dotx[2] += r.dotxi[2];
		
		ddotx[0] += r.ddotxi[0];
		ddotx[1] += r.ddotxi[1];
		ddotx[2] += r.ddotxi[2];
		
		u[0] += r.ui[0];
		u[1] += r.ui[1];
		u[2] += r.ui[2];
		
		dotu[0] += r.dotui[0];
		dotu[1] += r.dotui[1];
		dotu[2] += r.dotui[2];
		
		ddotu[0] += r.ddotui[0];
		ddotu[1] += r.ddotui[1];
		ddotu[2] += r.ddotui[2];
		
		M[0] += r.Mi[0];
		M[1] += r.Mi[1];
		M[2] += r.Mi[2];
		
		F[0] += r.Fi[0];
		F[1] += r.Fi[1];
		F[2] += r.Fi[2];
		
		Fdrag[0] += r.Fdragi[0];
		Fdrag[1] += r.Fdragi[1];
		Fdrag[2] += r.Fdragi[2];
		
		Finer[0] += r.Fineri[0];
		Finer[1] += r.Fineri[1];
		Finer[2] += r.Fineri[2];
		
		Fdamp[0] += r.Fdampi[0];
		Fdamp[1] += r.Fdampi[1];
		Fdamp[2] += r.Fdampi[2];
		
		Frest[0] += r.Fresti[0];
		Frest[1] += r.Fresti[1];
		Frest[2] += r.Fresti[2];
		
		Faddm[0] += r.Faddmi[0];
		Faddm[1] += r.Faddmi[1];
		Faddm[2] += r.Faddmi[2];
		
		Fwave += r.Fwavei;
		Theta += r.Thetai;
		Eta += r.Etai;
		
		
		return *this;
	}
	
	friend std::ostream &operator<<(std::ostream &stream, const AResultAll_t &t) {
		//43
		stream << t.t << " "
		       << t.Theta << " "
		       << t.Eta << " "
		       << t.M[0] << " "
		       << t.M[1] << " "
		       << t.M[2] << " "
		       << t.F[0] << " "
		       << t.F[1] << " "
		       << t.F[2] << " "
		       << t.Fdrag[0] << " "
		       << t.Fdrag[1] << " "
		       << t.Fdrag[2] << " "
		       << t.Finer[0] << " "
		       << t.Finer[1] << " "
		       << t.Finer[2] << " "
		       << t.Faddm[0] << " "
		       << t.Faddm[1] << " "
		       << t.Faddm[2] << " "
		       << t.Fdamp[0] << " "
		       << t.Fdamp[1] << " "
		       << t.Fdamp[2] << " "
		       << t.Frest[0] << " "
		       << t.Frest[1] << " "
		       << t.Frest[2] << " "
		       << t.Fwave << " "
		       << t.x[0] << " "
		       << t.x[1] << " "
		       << t.x[2] << " "
		       << t.dotx[0] << " "
		       << t.dotx[1] << " "
		       << t.dotx[2] << " "
		       << t.ddotx[0] << " "
		       << t.ddotx[1] << " "
		       << t.ddotx[2] << " "
		       << t.u[0] << " "
		       << t.u[1] << " "
		       << t.u[2] << " "
		       << t.dotu[0] << " "
		       << t.dotu[1] << " "
		       << t.dotu[2] << " "
		       << t.ddotu[0] << " "
		       << t.ddotu[1] << " "
		       << t.ddotu[2] << "\n";
		return stream;
	}
	
	friend std::istream &operator>>(std::istream &stream, AResultAll_t &t) {
		//43
		stream >> t.t
		       >> t.Theta
		       >> t.Eta
		       >> t.M[0]
		       >> t.M[1]
		       >> t.M[2]
		       >> t.F[0]
		       >> t.F[1]
		       >> t.F[2]
		       >> t.Fdrag[0]
		       >> t.Fdrag[1]
		       >> t.Fdrag[2]
		       >> t.Finer[0]
		       >> t.Finer[1]
		       >> t.Finer[2]
		       >> t.Faddm[0]
		       >> t.Faddm[1]
		       >> t.Faddm[2]
		       >> t.Fdamp[0]
		       >> t.Fdamp[1]
		       >> t.Fdamp[2]
		       >> t.Frest[0]
		       >> t.Frest[1]
		       >> t.Frest[2]
		       >> t.Fwave
		       >> t.x[0]
		       >> t.x[1]
		       >> t.x[2]
		       >> t.dotx[0]
		       >> t.dotx[1]
		       >> t.dotx[2]
		       >> t.ddotx[0]
		       >> t.ddotx[1]
		       >> t.ddotx[2]
		       >> t.u[0]
		       >> t.u[1]
		       >> t.u[2]
		       >> t.dotu[0]
		       >> t.dotu[1]
		       >> t.dotu[2]
		       >> t.ddotu[0]
		       >> t.ddotu[1]
		       >> t.ddotu[2];
		return stream;
	}
	
};

template<class TResult_t>
struct AResults_t {
	
	enum result_type {
		temp = 0,
		fin = 1,
	};
	
	bool doSaveTemp;
	
	std::string name;
	size_t numSteps;
	size_t numElems;
	
	
	std::vector<TResult_t> r;
	std::vector<AResultTemp_t> r_;
	
	size_t size() {
		return numSteps;
	}
	
	TResult_t &operator()(int i) {
		return r[i];
	}
	
	const TResult_t &operator()(int i) const {
		return r[i];
	}
	
	AResultTemp_t &operator[](int i) {
		return r_[i];
	}
	
	const AResultTemp_t &operator[](int i) const {
		return r_[i];
	}
	
	AResults_t<TResult_t> &operator++() {
		
		for (int i = 0; i < numSteps; i++) {
			r[i] += r_[i];
		}
		
		return *this;
	}
	
	AResults_t<TResult_t> operator++(int) {
		AResults_t<TResult_t> R(*this);
		++(*this);
		return R;
	}
	
	AResults_t<TResult_t> &operator<<(int opt) {
		doSaveTemp = bool(opt);
		return *this;
	}
	
	AResults_t<TResult_t> &operator<<(std::string fname) {
		
		obj2txt(fname, *this);
		return *this;
		
	}
	
	
	friend std::ostream &operator<<(std::ostream &stream, const AResults_t<TResult_t> &t) {
		if (t.doSaveTemp) {
			for (int i = 0; i < t.numSteps; i++)
				stream << t(i).t << " " << t[i];
		} else {
			for (int i = 0; i < t.numSteps; i++)
				stream << t(i);
		}
		
		return stream;
	}
	
	friend std::istream &operator>>(std::istream &stream, const AResults_t<TResult_t> &t) {
		TResult_t r;
		AResultTemp_t r_;
		
		while (stream >> r.t >> r_) {
			printf("r.t=%f\n", r.t);
		}
		
		return stream;
	}
	
	AResults_t(double dt, double t0, double t1, std::string name) :
			name(name) {
		
		numSteps = size_t((t1 - t0) / dt);
		
		r.resize(numSteps);
		r_.resize(numSteps);
		
		for (int i = 0; i < numSteps; i++) {
			r[i].t = dt * i;
		}
		
	}
	
};


#endif //EXAM_ARESULTS_H
