//
// Created by robgrzel on 13.05.17.
//

#pragma once

#ifndef STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVES_H
#define STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVES_H


#include <cstdio>
#include <cmath>
#include <iomanip>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


#include <vector>
#include <complex>
#include <valarray>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include "UtilSources/Containers/std_utils.h"
#include "UtilSources/Math/math.h"
#include "UtilSources/template_utils.h"
#include "UtilSources/Math/FFT.h"

#define ref(u, a) u[a]


typedef struct AWavePulsTime_t {
	
	/*
	 * theta = kx - om*t - eps;
	 * eta = amp * cos(theta);
	 */
	
	
	double theta;
	double eta;
	
	
} AWavePulsTime;

typedef struct AWaveTime_t {
	
	/*
	 * u += eta;
	 */
	
	double x;
	double t;
	double dt;
	double zeta;
	
	
} AWaveTime;


typedef struct AWavePuls_t {
	/*
	 * amp = sqrt(2*dom*S);
	 */
	int id = -1;
	double T;
	double om;
	double dom;
	double amp;
	double eps;
	double spc;
	
	
} AWavePuls;


void fft_wave_elevation_long_crested(const std::vector<AWaveTime_t> &AWaveT,
                                     std::vector<AWavePuls_t> &AWaveP_FFT
);


void wave_elevation_long_crested(const std::vector<AWavePuls_t> &O, std::vector<AWaveTime_t> &T);

void spectrum_amplitudes(std::vector<AWavePuls_t> &O, double dom);

void waves_phase_shift(std::vector<AWavePuls_t> &O);


double pulsation_peak_mean(const std::vector<AWavePuls_t> &O);


void spectra_moments(const int n, const int *ord, double *moments, const std::vector<double> &Spc,
                     const std::vector<double> &Om, const double dom);

void spectra_parameters(const std::vector<AWavePuls_t> &O, double H[4], double T[6], double M[7]);

int douglas_scale(double Hs);

typedef struct sea_scale_t {
	
	double fp;
	double velwind;
	double fetch_;
	double fetch;
	int beaufort;
	int douglass;
	
	sea_scale_t(const double Tp, const double Hs) {
		
		fp = 1 / Tp;
		
		velwind = Tp * 0.87 * GRAV / (2 * M_PI * pow(1.95, 1. / 7.));
		
		fetch_ = pow(fp * velwind / GRAV, -3);
		fetch = fetch_ * pow(velwind, 2) / GRAV;
		
		beaufort = iround(pow(velwind / 0.836, 2. / 3.));
		
		douglass = douglas_scale(Hs);
		
		
	}
	
} sea_scale;


class OWavesJonswap {
	
	public:
		
		enum MRatioNarrow_t {
			standard = 0,
			DNV = 1,
			Brodtkorb = 2,
		};
		
		size_t get_num_steps() { return numSteps; }
		
		size_t get_num_waves() { return numWaves; }
		
		size_t get_num_waves_fft() { return numWavesFFT; }
	
	
	private:
		
		MRatioNarrow_t mrationarrow;
		
		double Hs, Tp, fp, omp;
		double dom, df, dt, tmax;
		double domFFT;
		
		double alpha, gamma;
		
		int idOmmin = 0;
		int idOmmax = 0;
		
		double ommin = 2 * M_PI * 0.005;
		double ommax = 2 * M_PI * 3.005;
		double omrng = 2 * M_PI * 3.;
		double Tmin = 1 / 3.005;
		double Tmax = 1 / 0.005;
		double Trng = 1 / 3.;
		double fmin = 0.005;
		double fmax = 3.005;
		double frng = 3.;
		double fsamp = 50;
		
		
		double H[4], H_FFT[4];
		double T[6], T_FFT[6];
		double M[7], M_FFT[7];
		
		std::string nameSeas = "North Sea";
	
	private:
		
		const size_t numSteps;
		size_t numWaves;
		size_t numWavesFFT;
		
		std::vector<AWavePuls_t> AWaveP_FFT;
		std::vector<AWavePuls_t> AWaveP;
		std::vector<AWaveTime_t> AWaveT;
	
	protected:
		
		void init();
		
		void ratio_narrowing_dnv();
		
		void ratio_narrowing_brodtkorb();
		
		void ratio_narrowing_standard();
		
		bool check_ratio_narrowing();
		
		void ratio_narrowing();
		
		void ratio_peakadnes(double a = 1, double HsM0 = 0, double tol = 1e-7);
		
		void spectrum(double alpha);
		
		void pulsation_boundary_fit();
		
		void pulsation_boundary_range(std::vector<AWavePuls_t> &AWaveP, size_t &numWaves, double &dom);
		
		void pulsation_boundary(double alpha = 1, bool isFFT = false);
	
	
	public:
		
		void swap_vec_wave_puls(std::vector<AWavePuls_t> &vec, bool getFFT = false);
		
		void swap_vec_wave_time(std::vector<AWaveTime_t> &vec);
		
		void save_data();
		
		OWavesJonswap(
				const double tmax, double dt, double Hs, double Tp,
				std::string nameSeas = "seas", MRatioNarrow_t mrationarrow = Brodtkorb
		);
		
		const AWavePuls_t &operator()(int j, int i) const {
			if (i == 0)
				return AWaveP[j];
			else if (i == 1)
				return AWaveP_FFT[j];
		}
		
		const AWaveTime_t &operator()(int i) const {
			return AWaveT[i];
		}
	
};


#endif //STABILITY_AND_DYNAMICS_OF_SHIP_AND_OFFSHORE_STRUCTURES_OWAVES_H

