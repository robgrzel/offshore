//
// Created by robgrzel on 13.05.17.
//

#include "OWaves.h"


void fft_wave_elevation_long_crested(const std::vector<AWaveTime_t> &AWaveT,
                                     std::vector<AWavePuls_t> &AWaveP_FFT
) {
	
	const size_t numSteps = AWaveT.size();
	size_t numFreqEvenFFT;
	
	numFreqEvenFFT = size_t(numSteps / 2); //half of freqs num rounded back if odd
	
	std::vector<double> Z(numSteps);
	std::vector<double> omFFT(numSteps);
	std::vector<double> spcFFT(numSteps);
	
	double dt = AWaveT[0].dt;
	double domFFT;
	
	for (int i = 0; i < numSteps; i++) {
		Z[i] = AWaveT[i].zeta;
	}
	
	UTILS::fftfreq_pos(omFFT, dt); //size of positive freqs
	
	for (int i = 0; i < numFreqEvenFFT; i++) {
		omFFT[i] *= 2 * PI;
	}
	
	domFFT = omFFT[2] - omFFT[1];
	
	UTILS::fft_pos(spcFFT, Z, numFreqEvenFFT);
	
	for (int i = 0; i < numFreqEvenFFT; i++) {
		spcFFT[i] /= (2 * domFFT);
	}
	
	for (int i = 0; i < numFreqEvenFFT; i++) {
		AWaveP_FFT[i].spc = spcFFT[i];
		AWaveP_FFT[i].om = omFFT[i];
		AWaveP_FFT[i].dom = domFFT;
		AWaveP_FFT[i].eps = 0;
		AWaveP_FFT[i].amp = 0;
	}
	
}


void wave_elevation_long_crested(const std::vector<AWavePuls_t> &O, std::vector<AWaveTime_t> &T) {
	
	AWavePuls_t Oj;
	AWaveTime_t Ti;
	
	const size_t N = O.size();
	const size_t M = T.size();
	
	double theta;
	double zeta;
	double eta;
	double eps;
	double amp;
	double om;
	double t;
	double k;
	double x;
	
	double Zeta[M];
	
	
	for (size_t i = 0; i < M; i++) {
		T[i].zeta = 0;
	}
	
	int i, j;
	for (i = 0; i < M; i++) {
		//#pragma omp for
		zeta = 0;
		for (j = 0; j < N; j++) {
			
			Oj = O[j];
			om = Oj.om;
			amp = Oj.amp;
			eps = Oj.eps;
			k = 0;//Oj.k; //TODO ADD WAVENUMBER
			
			theta = om * T[i].t + eps - k * T[i].x;
			eta = amp * sin(theta);
			zeta += eta;
		}
		T[i].zeta = zeta;
	}
	
	for (int i = 0; i < M; i++) {
		Zeta[i] = T[i].zeta;
	}
	
	arr2file("z.txt", M, Zeta);
	//exit(0);
	
	long double s = 0;
	
	for (int i = 0; i < M; i++) {
		s += pow(T[i].zeta, 2);
	}
	
	s = pow(s / M, 0.5);
	
}

void spectrum_amplitudes(std::vector<AWavePuls_t> &O, double dom) {
	
	
	const size_t N = O.size();
	
	for (int i = 0; i < N; i++) {
		
		O[i].amp = UTILS::spc2amp(dom, O[i].spc);
	}
	
}

void waves_phase_shift(std::vector<AWavePuls_t> &O) {
	
	const size_t N = O.size();
	
	double EPS[N];
	
	randn2pi(N, EPS);
	
	for (int i = 0; i < N; i++) {
		
		O[i].eps = EPS[i];
	}
	
	
}


double pulsation_peak_mean(const std::vector<AWavePuls_t> &O) {
	
	const size_t N = O.size();
	
	std::vector<double> spc(N);
	
	for (int i = 0; i < N; i++) {
		spc[i] = O[i].spc;
	}
	
	long idSpcMax = arg_maxd(spc);
	
	double omPeak = O[idSpcMax].om;
	
	return omPeak;
	
	
}


void spectra_moments(const int n, const int *ord, double *moments, const std::vector<double> &Spc,
                     const std::vector<double> &Om, const double dom) {
	//https://stackoverflow.com/questions/7627098/what-is-a-lambda-expression-in-c11
	
	const size_t N = Spc.size();
	
	std::vector<double> tmp(N);
	
	for (int i = 0; i < n; i++) {
		
		for (int j = 0; j < N; j++) {
			tmp[j] = Spc[j] * pow(Om[j], ord[i]);
		}
		
		moments[i] = simpshy(dom, tmp);
	}
}

void spectra_parameters(const std::vector<AWavePuls_t> &O, double H[4], double T[6], double M[7]) {
	
	/*
	 fmoments [-1, 0, 1, 2, 3, 4, 5,]
	 heights [ 'RMS', 'mean: 1/2', 'significiant: 1/3', 'maximal: 1/10' ],
	 periods [ 'energy: -1(-10)', 'zerocrossing: 0(02)', 'mean: 1(01)',\
			   \'peak: T(S_max)', 'extrema: 2(24)', 'bandwi. e(20,40)', ]

	RMS = H[0]/2;
	 RMS = sqrt(sum(pow(Zi,2))/N);

	 */
	
	const int ord[7] = {-1, 0, 1, 2, 3, 4, 5};
	
	H[0] = 1;
	H[1] = 2.5;
	H[2] = 4.005;
	H[3] = 5.1;
	
	double dom = 0;
	
	const size_t N = O.size();
	
	std::vector<double> Spc(N);
	std::vector<double> Om(N);
	
	for (int i = 0; i < N; i++) {
		Spc[i] = O[i].spc;
		Om[i] = O[i].om;
		dom += O[i].dom;
	}
	
	dom /= N;
	
	spectra_moments(7, ord, M, Spc, Om, dom);
	
	
	for (int i = 0; i < 4; i++) {
		H[i] *= sqrt(M[1]);
	}
	
	T[0] = M[0] / M[1];
	T[1] = M[1] / M[3];
	T[2] = M[1] / M[2];
	T[3] = M[3] / M[5];
	T[4] = 1 / pulsation_peak_mean(O);
	T[5] = 1 - (pow(M[3], 2) / (M[1] * M[5]));
	
	for (int i = 0; i < 5; i++) {
		T[i] *= 2 * PI;
	}
	
	T[1] = sqrt(T[1]);
	T[3] = sqrt(T[3]);
	T[5] = sqrt(T[5]);
	
	
}

int douglas_scale(double Hs) {
	
	int s;
	
	if (Hs >= 0 and Hs < 0.1) {
		s = 0;
	} else if (Hs >= 0.1 and Hs < 0.5) {
		s = 1;
	} else if (Hs >= 0.5 and Hs < 1.25) {
		s = 2;
	} else if (Hs >= 1.25 and Hs < 2.5) {
		s = 4;
	} else if (Hs >= 2.5 and Hs < 4.) {
		s = 5;
	} else if (Hs >= 4. and Hs < 6.) {
		s = 6;
	} else if (Hs >= 6. and Hs < 9.) {
		s = 7;
	} else if (Hs >= 9. and Hs < 14.) {
		s = 8;
	} else if (Hs >= 14.) {
		s = 9;
	}
	
	return s;
}


void OWavesJonswap::init() {
	
	fp = 1 / Tp;
	omp = 2 * PI * fp;
	
	df = 1 / tmax;
	dom = 2 * PI * df;
	
	
	sea_scale_t seaScale(Tp, Hs);
	
	std::vector<double> om = arange<double>(ommin, ommax, dom);
	std::vector<double> t = arange<double>(0, tmax, dt);
	
	
	for (int i = 0; i < numWaves; i++) {
		AWaveP[i].id = i;
		AWaveP[i].dom = dom;
		AWaveP[i].om = om[i];
	}
	for (int i = 0; i < numSteps; i++) {
		AWaveT[i].dt = dt;
		AWaveT[i].t = t[i];
	}
	
	ratio_narrowing();
	
	ratio_peakadnes();
	
	spectrum(alpha);
	
	pulsation_boundary(alpha, false);
	
	spectrum_amplitudes(AWaveP, dom);
	
	waves_phase_shift(AWaveP);
	
	spectra_parameters(AWaveP, H, T, M);
	
	wave_elevation_long_crested(AWaveP, AWaveT);
	
	fft_wave_elevation_long_crested(AWaveT, AWaveP_FFT);
	
	pulsation_boundary(0, true);
	
	spectra_parameters(AWaveP_FFT, H_FFT, T_FFT, M_FFT);
	
	printf("Waves JONSWAP check spectra/spectraFFT Params:\n");
	printf("H = %f, %f, %f, %f\n", H[0], H[1], H[2], H[3]);
	printf("H_FFT = %f, %f, %f, %f\n", H_FFT[0], H_FFT[1], H_FFT[2], H_FFT[3]);
	
	
}


void OWavesJonswap::ratio_narrowing_dnv() {
	double r = Tp / sqrt(Hs);
	if (r <= 3.5) {
		gamma = 5;
	} else if (3.6 < r < 5) {
		gamma = exp(5.75 - (0.367 * Tp * sqrt(GRAV / Hs)));
	} else if (5 <= r) {
		gamma = 1;
	}
	
	mrationarrow = DNV;
}

void OWavesJonswap::ratio_narrowing_brodtkorb() {
	gamma = 0.1975;
	gamma *= pow(Tp, 4) * pow(Hs, -2);
	gamma *= 0.036 - (0.0056 * Tp / sqrt(Hs));
	
	gamma = exp(3.484 * (1 - gamma));
	
	mrationarrow = Brodtkorb;
}

void OWavesJonswap::ratio_narrowing_standard() {
	gamma = 3.3;
	
	mrationarrow = standard;
	
}

bool OWavesJonswap::check_ratio_narrowing() {
	
	double r = Tp / sqrt(Hs);
	
	bool c = (gamma > 7 or gamma < 1);
	
	switch (mrationarrow) {
		case (DNV) : {
			c |= (3.6 < r < 5);
			break;
		}
		case (Brodtkorb) : {
			c |= (3.6 > r > 5);
			break;
		}
	}
	
	return c;
}

void OWavesJonswap::ratio_narrowing() {
	
	switch (mrationarrow) {
		case (standard) : {
			ratio_narrowing_standard();
			return;
		}
		case (DNV) : {
			ratio_narrowing_dnv();
			if (check_ratio_narrowing()) {
				ratio_narrowing_standard();
			}
			return;
		}
		case (Brodtkorb) : {
			ratio_narrowing_brodtkorb();
			if (check_ratio_narrowing()) {
				ratio_narrowing_dnv();
				if (check_ratio_narrowing()) {
					ratio_narrowing_standard();
				}
			}
			return;
			
		}
		
	}
}

void OWavesJonswap::ratio_peakadnes(double a, double HsM0, double tol) {
	
	double err = 1;
	
	double M0 = 0;
	
	std::vector<double> spc(numWaves);
	
	while (err > tol) {
		
		spectrum(a);
		
		for (int i = 0; i < numWaves; i++) {
			spc[i] = AWaveP[i].spc;
		}
		
		pulsation_boundary(a, false);
		
		if (spc.size() != numWaves) {
			spc.resize(numWaves);
		}
		
		for (int i = 0; i < numWaves; i++) {
			spc[i] = AWaveP[i].spc;
		}
		
		M0 = simpshy(dom, spc);
		
		HsM0 = 4 * sqrt(M0);
		
		a *= Hs / HsM0;
		
		err = abs(Hs - HsM0);
		
	}
	
	alpha = a;
}

void OWavesJonswap::spectrum(double alpha) {
	
	double spc;
	double tau;
	double om;
	double om_omp;
	
	const double M = 5, N = 4, M_N = 5. / 4.;
	
	AWavePuls_t tmp;
	
	for (int i = 0; i < numWaves; i++) {
		tmp = AWaveP[i];
		om = tmp.om;
		tau = om <= omp ? 0.07 : 0.09;
		om_omp = om / omp;
		
		spc = alpha * pow(Hs, 2) * pow(omp, 4) * pow(om, -5);
		spc *= exp(-M_N * pow(om_omp, -N));
		spc *= pow(gamma, exp(-0.5 * pow((om_omp - 1) / tau, 2)));
		
		AWaveP[i].spc = spc;
	}
	
}

void OWavesJonswap::pulsation_boundary_fit() {
	//limit spectra broadness of pulsation
	
	const int n = 20;
	const int ord = 10;
	
	double x[n];
	
	double y[n] = {5.1101, 4.4501, 4.100, 3.8700,
	               3.7000, 3.5700, 3.4600, 3.3700,
	               3.2900, 3.2200, 3.1600, 3.1050,
	               3.0550, 3.0100, 2.9650, 2.9300,
	               2.8950, 2.8600, 2.8300, 2.8000
	};
	
	for (int i = 0 + 1; i < n + 1; i++) {
		x[i - 1] = i;
	}
	
	double poly[ord + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	polyfit(x, y, n, ord, poly);
	
	ommax = 0;
	
	for (int i = 0; i < ord + 1; i++) {
		ommax += pow(gamma, i) * poly[i];
	}
	
	ommax *= omp;
	
	ommin = omp * (0.58 + (0.05 * (gamma - 1.) / 19.));
	
};

void OWavesJonswap::pulsation_boundary_range(std::vector<AWavePuls_t> &AWaveP, size_t &numWaves, double &dom) {
	
	int idOmmin = -1;
	int idOmmax = -1;
	double om, dom_;
	
	dom = dom_ = AWaveP[0].dom;
	
	for (int i = 0; i < numWaves; i++) {
		om = AWaveP[i].om;
		if (idOmmin == -1) {
			idOmmin = (ommin - dom) <= om ? i : -1;
		};
		
		if (idOmmax == -1) {
			idOmmax = (ommax + dom) <= om ? i : -1;
		};
		
	}
	
	if (idOmmin == -1 and idOmmax == -1) {
		return;
	}
	
	if (idOmmin == -1) {
		idOmmin = 0;
	}
	
	if (idOmmax == -1) {
		idOmmax = numWaves;
	}
	
	std::vector<AWavePuls_t> AWaveP_(AWaveP.begin() + idOmmin, AWaveP.begin() + idOmmax);
	
	AWaveP.swap(AWaveP_);
	
	AWaveP_.clear();
	
	numWaves = AWaveP.size();
	
	dom = (ommax - ommin) / numWaves;
	
	double errdom = dom_ / dom;
	
	for (int i = 0; i < numWaves; i++) {
		AWaveP[i].spc *= errdom;
	}
	
	
}

void OWavesJonswap::pulsation_boundary(double alpha, bool isFFT) {
	
	pulsation_boundary_fit();
	
	if (!isFFT) {
		
		pulsation_boundary_range(AWaveP, numWaves, dom);
		
		spectrum(alpha);
		
		
	} else {
		
		pulsation_boundary_range(AWaveP_FFT, numWavesFFT, domFFT);
		
	}
	
}

void OWavesJonswap::swap_vec_wave_puls(std::vector<AWavePuls_t> &vec, bool getFFT) {
	if (!getFFT) {
		vec.swap(AWaveP);
	} else {
		vec.swap(AWaveP_FFT);
	}
}

void OWavesJonswap::swap_vec_wave_time(std::vector<AWaveTime_t> &vec) {
	vec.swap(AWaveT);
}


void OWavesJonswap::save_data() {
	std::vector<double> aspc(numWaves);
	std::vector<double> aom(numWaves);
	std::vector<double> aspcFFT(numWavesFFT);
	std::vector<double> aomFFT(numWavesFFT);
	
	for (int i = 0; i < numWavesFFT; i++) {
		aspcFFT[i] = AWaveP_FFT[i].spc;
		aomFFT[i] = AWaveP_FFT[i].om;
	}
	
	for (int i = 0; i < numWaves; i++) {
		aspc[i] = AWaveP[i].spc;
		aom[i] = AWaveP[i].om;
	}
	
	vec2file("spcFFT.txt", aspcFFT);
	vec2file("omFFT.txt", aomFFT);
	
	vec2file("spc.txt", aspc);
	vec2file("om.txt", aom);
	
}

OWavesJonswap::OWavesJonswap(
		const double tmax, const double dt,
		double Hs, double Tp, std::string nameSeas,
		MRatioNarrow_t mrationarrow
) :
		
		dt(dt),
		tmax(tmax),
		fsamp(NYQUIST(dt)),
		Hs(Hs), Tp(Tp),
		
		numSteps(tmax / dt),
		numWaves(frng * tmax),
		numWavesFFT(tmax * NYQUIST(dt)),
		
		AWaveT(tmax / dt),
		AWaveP(frng * tmax),
		AWaveP_FFT(tmax * NYQUIST(dt)),
		
		nameSeas(nameSeas),
		mrationarrow(mrationarrow) {
	
	
	if (0) {
		printf("OWavesJonswap terminated...\n ---> tmax should be number like : 200, 400, 600, 800, 1000, 1200... 3600\n");
		
		return;
	}
	
	init();
};

