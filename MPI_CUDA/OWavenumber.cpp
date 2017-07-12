//
// Created by robgrzel on 11.04.17.
//

#include "OWavenumber.h"

#define PI 3.1415926536
#define PI2 PI*2.

void OWavenumber::init_guess_without_currents( ) {
	/*
	 Makes initial guess using:
	 "Guo, J. (2002) Simple and explicit solution of wave dispersion, Coastal Engineering, 46(2), 71-74."

	 A simple correction is applied to the initial guess to account for currents.
	xi = wa./sqrt(g./h); %note: =h*wa/sqrt(h*g/h)
	yi = xi.*xi/(1.0-exp(-xi.^2.4908)).^0.4015;
	wki = yi/h; %Initial guess without current-wave interaction
	
	%Current Veloc in wave direction
	uk = cos(wd).*dotu + sin(wd).*v;
	
	%Simple correction for currents (tends to overcorrect, hense the relaxation)
	wki = 0.3*wki + 0.7*(wa-wki.*uk).^2./(g*tanh(wki.*h));
	 */
	xi = om / sqrt( Grav / depth );
	yi = pow( xi, 2. ) / pow( 1. - exp( -pow( xi, 2.4908 ) ), 0.4015 );

	// Initial guess without current - wave interaction
	k0 = yi / depth;

}


void OWavenumber::init_guess_with_current_velocity( ) {

	//# Current Veloc in wave direction
	v = cos( phi ) * vx + sin( phi ) * vy;

	//# Simple correction for currents (tends to overcorrect, hense the relaxation)
	k0 = 0.3 * k0 + 0.7 * pow( om - k0 * v, 2. ) / (Grav * tanh( k0 * depth ));

}

void OWavenumber::wavenumber_to_wavelenght( ) {
	lambda = PI2 / k;
}

void OWavenumber::wavenumber_to_vector( ) {

	K[0] = cos( phi ) * k;
	K[1] = sin( phi ) * k;

}

inline void OWavenumber::check_solution( ) {
	isBlocking = (k < minK) | (k > maxK);
	// No solution, assume wave blocking
	if (isBlocking) {
		k      = -1;
		doIter = false;
		return;
	} else {
		prec     = fabs( k - k0 );
		isSolved = prec < tol;
		if (isSolved) {
			doIter = false;
			return;
		} else {
			doIter = true;
			k0     = k;
			return;
		};
	};

};

inline void OWavenumber::solve_by_newton_raphson( ) {
	/*
	 	% The Newton-Raphson Method is given by
		%   wavNum(n+1) = wavNum(n) - f(k(n))/fp(k(n))
		% where
		%   f = g*wavNum*tanh(wavNum*depth) - ome^2
		%   fp = g*(tanh(wavNum*depth) - depth*wavNum*(tanh(wavNum*depth)^2-1)) + 2*v*ome
		%
	 
	 sig = wa-wk.*uk;
    tanhkh = tanh(wk.*h);
    f = g*wk.*tanhkh - sig.^2;
    fp = g*(tanhkh - h.*wk.*(tanhkh.^2-1)) + 2*uk.*sig;
    wk = wki - f./fp;
    iblk = (wk<0 | wk>1e6); %No solution, assume wave blocking
    wk(iblk) = NaN;
    wkchg = abs(wk-wki);
    if(all(iblk) || all(wkchg<tol)), break, end
    wki = wk;
	 */
	
	ome    = om - k * v;
	tanhkh = tanh( k * depth );
	f      = Grav * k * tanhkh - pow( ome, 2 );
	fp     = Grav * (tanhkh - depth * k * (pow( tanhkh, 2 ) - 1)) + 2 * v * ome;

	k = k0 - f / fp;

	//printf("om=%f,k=%f,v=%f,depth=%f,Grav=%f, ome=%f, tanhkh=%f,f=%f,fp=%f,f/fp=%f\n",om,k,v,depth,Grav,ome,tanhkh,f,fp,f/fp);

};

void OWavenumber::check_solved( ) {
	ome    = om - k * v;
	tanhkh = tanh( k * depth );
	f      = Grav * k * tanhkh - pow( ome, 2. );
	//# same as f above but updated
	if (isBlocking) {
		prec = -1;
	};

}

void OWavenumber::wavenumber_calc( ) {


	this->init_guess_without_currents();
	this->init_guess_with_current_velocity();

	//# Main loop for Newton-Raphson iteration

	k = k0;

	while ( doIter ) {
		this->solve_by_newton_raphson();
		this->check_solution();
	};

	this->check_solved();

	this->wavenumber_to_wavelenght();

	wavenumber_to_vector();
	wave_propagation_direction();

}

void OWavenumber::wave_propagation_direction( ) {
	Ek[0] = K[0] / k;
	Ek[1] = K[1] / k;
};
