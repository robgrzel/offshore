from __init__ import *

'optimize with: cython jonswap.pyx -a'
import numpy as np
import pandas as pd

import matplotlib.pyplot as plt
import scipy.integrate

from numexpr import evaluate as ne_evaluate, set_num_threads as numexpr_threads
import math

try :
	import pycuda.autoinit
	import pycuda.driver as drv
	from pycuda.compiler import SourceModule
except Exception as err :
	print( "EXCEPTION (NO PYCUDA IMPORTED)::: ", err )

np.random.seed( 0 )

PI = np.pi
GRAV = 9.81

DTYPE = np.float64
ITYPE = np.int
ITYPE1 = np.int64
BTYPE = np.uint8
BOTYPE = np.bool_


def wave_elevation( dom = None, OMDATA = None, TIMEDATA = None, numThreadsElevation = 4, self = None ) :
	if self is not None :
		numexpr_threads( self.numThreadsElevation )

		dom, SPECTRA, SPECAMPLI = self.dom, self.OMDATA[ 1 ], self.OMDATA[ 3 ]
		ne_evaluate(
			'sqrt(dom*2*S)',
			out = SPECAMPLI,
			local_dict = {
				'dom' : dom,
				'S'   : SPECTRA
			}
		)


		self.OMDATA[ 2, 0 ] = 2. * np.random.standard_normal( self.OMDATA.shape[ 2 ] )

		N = self.TIMEDATA.shape[ 2 ] // 10

		self.OMDATA = self.OMDATA.astype( 'f' )
		self.TIMEDATA = self.TIMEDATA.astype( 'f' )

		dct = { 'A' : self.OMDATA[ 3 ], 'om' : self.OMDATA[ 0 ], 'eps' : self.OMDATA[ 2 ] }

		for n in range( N ) :
			sl = slice( N * n, N * (n + 1) )

			dct[ 'ti' ] = self.TIMEDATA[ 0, 0, sl ]

			ne_evaluate(
				r'sum(A*sin(om*ti+eps),axis=1)',
				global_dict = dct,
				out = self.TIMEDATA[ 1, 0, sl, 0 ]
			)

		self.TIMEDATA[ : ] = self.TIMEDATA.astype( 'd' )

		self.TIMEDATA.shape = self.TIMEDATA.shape[ :3 ]

		return

	else :
		numexpr_threads( numThreadsElevation )

		dom, SPECTRA, SPECAMPLI = dom, OMDATA[ 1 ], OMDATA[ 3 ]
		ne_evaluate(
			'sqrt(dom*2*S)',
			out = SPECAMPLI,
			local_dict = {
				'dom' : dom,
				'S'   : SPECTRA
			}
		)

		OMDATA[ 2, 0 ] = 2. * np.random.standard_normal( OMDATA.shape[ 2 ] )

		N = TIMEDATA.shape[ 2 ] // 10

		OMDATA = OMDATA.astype( 'f' )
		TIMEDATA = TIMEDATA.astype( 'f' )

		dct = { 'A' : OMDATA[ 3 ], 'om' : OMDATA[ 0 ], 'eps' : OMDATA[ 2 ] }

		for n in range( N ) :
			sl = slice( N * n, N * (n + 1) )

			dct[ 'ti' ] = TIMEDATA[ 0, 0, sl ]

			ne_evaluate(
				r'sum(A*sin(om*ti+eps),axis=1)',
				global_dict = dct,
				out = TIMEDATA[ 1, 0, sl, 0 ]
			)

		TIMEDATA[ : ] = TIMEDATA.astype( 'd' )

		TIMEDATA.shape = TIMEDATA.shape[ :3 ]


def pulsation_peak_mean( OM, SPECTRA ) :
	periodPeak = 1. / OM[ np.argmax( SPECTRA ) ]
	return periodPeak


def moments_nth( N, dom, OM, SPECTRA ) :
	MOMENTS = np.zeros( N.shape[ 0 ] )
	for n in N :
		MOMENT = scipy.integrate.simps( y = SPECTRA * (OM ** n), dx = dom )
		MOMENTS[ n + 1 ] = MOMENT
	return MOMENTS


def spectra_parameters( OM, dom ) :
	# dom,OM[0],OM[1],N

	OM, SPECTRA = OM[ 0, 0 ], OM[ 1, 0 ]

	N = np.arange( -1, 5, 1 )
	MOMENTS = moments_nth( N, dom, OM, SPECTRA )
	HEIGHTS = np.array( [ 1, 2.5, 4.005, 5.1 ] ) * np.sqrt( MOMENTS[ 1 ] )
	PERIODS = np.array( [
		MOMENTS[ 0 ] / MOMENTS[ 1 ],
		MOMENTS[ 1 ] / MOMENTS[ 3 ],
		MOMENTS[ 1 ] / MOMENTS[ 2 ],
		pulsation_peak_mean( OM, SPECTRA ),
		MOMENTS[ 3 ] / MOMENTS[ 5 ],
		1 - (MOMENTS[ 3 ] ** 2 / (MOMENTS[ 1 ] * MOMENTS[ 5 ])),
	] )

	PERIODS[ :5 ] *= 2 * PI
	PERIODS[ [ 1, 4, 5 ] ] **= 0.5

	IDX = [
		N,
		[ 'RMS', 'mean: 1/2', 'significiant: 1/3', 'maximal: 1/10' ],
		[ 'energy: -1(-10)', 'zerocrossing: 0(02)', 'mean: 1(01)',
		  'peak: T(S_max)', 'extrema: 2(24)', 'bandwi. e(20,40)', ]
	]

	return pd.Series( np.round( MOMENTS, 2 ), index = IDX[ 0 ], dtype = 'd' ), \
	       pd.Series( np.round( HEIGHTS, 2 ), index = IDX[ 1 ], dtype = 'd' ), \
	       pd.Series( np.round( PERIODS, 2 ), index = IDX[ 2 ], dtype = 'd' )


def match_freq_sampling( freqSampl = None, numSteps = None, freqMax = None, self = None ) :
	if self is not None :
		if self.freqSampl is not None and self.freqSampl > 0 :
			freqSampl = self.freqSampl
		else :
			freqSampl = 50

		if not self.freqSampl and self.numSteps < 1000. :
			self.freqSampl = np.float64(
				int(
					self.freqMax * self.numSteps / freqSampl
				)
			)

		elif self.numSteps >= 1000. :
			self.freqSampl = freqSampl

		return

	else :
		if freqSampl is not None and freqSampl > 0 :
			freqSampl = freqSampl
		else :
			freqSampl = 50

		if not freqSampl and numSteps < 1000. :
			freqSampl = np.float64(
				int(
					freqMax * numSteps / freqSampl
				)
			)

		elif numSteps >= 1000. :
			freqSampl = freqSampl

		return freqSampl


def seas_scales( periodPeak = None, heightSig = None, self = None, ) :
	if self is not None :
		self.velWind = self.periodPeak * 0.87 * GRAV / (2 * PI * (1.95 ** (1 / 7.)))
		self.freqPeak = 1. / self.periodPeak
		self.fetchDimless = (self.freqPeak * self.velWind / GRAV) ** (-3.)
		self.fetch = self.fetchDimless * (self.velWind ** 2) / GRAV

		# beaufort_scale_of_wind(self.periodPeak)
		self.degBeaufort = un.roundup( (self.velWind / 0.836) ** (2. / 3.), 1. )
		self.degDouglass = un.roundup(
			np.poly1d(
				np.polyfit(
					np.array( [ 0, 0.1, 0.5, 1.25, 2.5, 4.,
					            6., 9., 14, 20, 25, 30, 35, 40, 45 ] ),
					np.append( np.arange( 0, 10, 1 ), [ 9, 9, 9, 9, 9 ] ),
					deg = 6
				)
			)( self.heightSig ),
			1. )

		return
	else :
		velWind = periodPeak * 0.87 * GRAV / (2 * PI * (1.95 ** (1 / 7.)))
		freqPeak = 1. / periodPeak
		fetchDimless = (freqPeak * velWind / GRAV) ** (-3.)
		fetch = fetchDimless * (velWind ** 2) / GRAV

		# beaufort_scale_of_wind(periodPeak)
		degBeaufort = un.roundup( (velWind / 0.836) ** (2. / 3.), 1. )
		degDouglass = un.roundup(
			np.poly1d(
				np.polyfit(
					np.array( [ 0, 0.1, 0.5, 1.25, 2.5, 4.,
					            6., 9., 14, 20, 25, 30, 35, 40, 45 ] ),
					np.append( np.arange( 0, 10, 1 ), [ 9, 9, 9, 9, 9 ] ),
					deg = 6
				)
			)( heightSig ),
			1. )

		return velWind, fetch, degBeaufort, degDouglass


class Irregular_Waves_by_Jonswap( ) :
	# float32 dont have enougth precision for FFT

	freqMin = np.float64( 0.01 )
	freqMax = np.float64( 3. )

	counter = [ 0 ]

	def __getattribute__( self, name ) :
		attr = super( ).__getattribute__( name )
		# un._check_getattribute(__class__,self,name,attr)
		return attr

	def __setattr__( self, name, v ) :
		# un._ceck_setattr(__class__,self,name,v)
		super( ).__setattr__( name, v )

	def __init__( self, STORES, DIRS, shipsParameters = None, **OPT ) :
		'''
		:param STORES:
		:param DIRS:
		:param shipsParameters:
		:param OPT:
            numSteps=1000,
		    heightSig=11,
		    periodPeak=13.37,
		    methRatioNarrow='Brodtkorb/DNV/standard',
		    freqSampl=0,
		    nameSeas='nameSeas',
		    figsize=[5*11.69,5*8.27],
		    elevationEngine='numexpr',
		    numThreadsElevation=8
		'''

		self.STORES = STORES
		self.DIRS = DIRS
		self.shipsParameters = shipsParameters

		Hs = OPT[ 'heightSig' ]
		Tp = OPT[ 'periodPeak' ]
		nameSeas = OPT[ 'nameSeas' ]

		if hasattr( nameSeas, '__iter__' ) and type( nameSeas ) != str :
			nameSeas = list( nameSeas )
		if hasattr( Hs, '__iter__' ) :
			Hs = list( Hs )
		if hasattr( Tp, '__iter__' ) :
			Tp = list( Tp )

		if (type( Hs ) == list) & (type( Tp ) == list) & (type( nameSeas ) == list) :
			for Hsi, Tpi, nameSeasI in zip( Hs, Tp, nameSeas ) :
				OPT[ 'heightSig' ], OPT[ 'periodPeak' ], OPT[ 'nameSeas' ] = Hsi, Tpi, nameSeasI
				self.__run__( **OPT )
		elif (type( Hs ) == list) & (type( Tp ) == list) & (type( nameSeas ) != list) :
			for Hsi, Tpi in zip( Hs, Tp ) :
				OPT[ 'heightSig' ], OPT[ 'periodPeak' ] = Hsi, Tpi
				self.__run__( **OPT )
		elif (type( Hs ) == list) & (type( Tp ) != list) & (type( nameSeas ) == list) :
			for Hsi, nameSeasI in zip( Hs, nameSeas ) :
				OPT[ 'heightSig' ], OPT[ 'nameSeas' ] = Hsi, nameSeasI
				self.__run__( **OPT )
		elif (type( Hs ) != list) & (type( Tp ) == list) & (type( nameSeas ) == list) :
			for Tpi, nameSeasI in zip( Tp, nameSeas ) :
				OPT[ 'periodPeak' ], OPT[ 'nameSeas' ] = Tpi, nameSeasI
				self.__run__( **OPT )
		elif (type( Hs ) == list) & (type( Tp ) != list) & (type( nameSeas ) != list) :
			for Hsi in Hs :
				OPT[ 'heightSig' ] = Hsi
				self.__run__( **OPT )
		elif (type( Hs ) != list) & (type( Tp ) == list) & (type( nameSeas ) != list) :
			for Tpi in Tp :
				OPT[ 'periodPeak' ] = Tpi
				self.__run__( **OPT )
		elif (type( Hs ) != list) & (type( Tp ) != list) & (type( nameSeas ) == list) :
			for nameSeasI in nameSeas :
				OPT[ 'nameSeas' ] = nameSeasI
				self.__run__( **OPT )
		else :
			self.__run__( **OPT )

	def __run__( self, numSteps = 1000, heightSig = 11, periodPeak = 13.37, methRatioNarrow = 'Brodtkorb/DNV/standard',
	             freqSampl = 0, nameSeas = 'North Sea', figsize = [ 5 * 11.69, 5 * 8.27 ], elevationEngine = 'numexpr',
	             numThreadsElevation = 8 ) :
		print( '#' * 100 )
		print( '#' * 100 )
		print( '#' * 100 )
		print( '#' * 10, 'IRREGULAR_WAVES_BY_JONSWAP::__run__()', '#' * 10, )

		self.counter[ 0 ] += 1

		self.heightSig = heightSig
		self.periodPeak = periodPeak
		self.freqSampl = freqSampl
		self.nameSeas = nameSeas
		self.figsize = figsize
		self.elevationEngine = elevationEngine
		self.numThreadsElevation = numThreadsElevation
		self.methRatioNarrow = methRatioNarrow
		self.numSteps = un.roundup( numSteps )

		self.set_store_dirs( )

		match_freq_sampling( self = self )
		self.set_vars_of_pulsation_and_time( )

		self.set_plot_styles( )
		self.set_memmaps( )

		seas_scales( self = self )
		self.ratio_narrowing( )
		self.ratio_peakadness( )

		self.spectrum_solver( solverOpt = 'Empirical' )

		self.MOMENTS, self.HEIGHTS, self.PERIODS = spectra_parameters( self.OMDATA, self.dom )

		wave_elevation( self = self )

		self.spectrum_solver( solverOpt = 'FFT' )

		self.MOMENTSFFT, self.HEIGHTSFFT, self.PERIODSFFT = spectra_parameters( self.OMFFTDATA, self.domFFT )

		un.plt_fig_sets( figsize, font_divide = 1.7, legendfont_divide = 1.7 )

		self.data_saver( )
		self.data_plotter( )

		self.memmaps_close( )

	def set_vars_of_pulsation_and_time( self ) :
		self.omMin, self.omMax, \
		self.dom, self.dt, self.omPeak = \
			np.float64( self.freqMin * PI * 2 ), \
			np.float64( self.freqMax * PI * 2 ), \
			np.float64( 2 * PI / self.numSteps ), \
			np.float64( 1. / self.freqSampl ), \
			np.float64( 2 * PI / self.periodPeak )

	def ratio_peakadness( self, a = 1, HsM0 = 0, tol = 1e-5 ) :
		while np.abs( self.heightSig - HsM0 ) > tol :
			self.spectrum_jonswap( ratioPeakd = a )
			self.spectrum_pulsation_boundary( ratioPeakd = a, recSp = 1 )
			HsM0 = 4. * np.sqrt( scipy.integrate.simps( y = self.OMDATA[ 1, 0 ], dx = self.dom ) )
			a = a * self.heightSig / HsM0

		self.ratioPeakad = a

	def ratio_narrowing( self ) :
		def gamma_by_DNV( ) :
			if self.periodPeak / np.sqrt( self.heightSig ) <= 3.6 :
				self.ratioNarrow = 5.
			elif 3.6 < self.periodPeak / np.sqrt( self.heightSig ) < 5. :
				self.ratioNarrow = np.exp( 5.75 - (0.367 * self.periodPeak * np.sqrt( GRAV / self.heightSig )) )
			elif 5. <= self.periodPeak / np.sqrt( self.heightSig ) :
				self.ratioNarrow = 1
			self.methRatioNarrow = 'DNV'

		def gamma_by_Brodtkorb2004( ) :
			g1 = 0.036 - (0.0056 * self.periodPeak / np.sqrt( self.heightSig ))
			g2 = (self.periodPeak ** 4) * (self.heightSig ** (-2))
			g3 = 0.1975 * g1 * g2
			self.ratioNarrow = np.exp( 3.484 * (1 - g3) )
			self.methRatioNarrow = 'Brodtkorb'

		def gamma_standard( ) :
			self.ratioNarrow = 3.3
			self.methRatioNarrow = 'standard'

		if self.methRatioNarrow == 'standard' :
			gamma_standard( )
		elif self.methRatioNarrow == 'DNV/standard' :
			gamma_by_DNV( )
			if self.check_ratio_narrowing( ) :
				gamma_standard( )
		elif self.methRatioNarrow == 'Brodtkorb/DNV/standard' :
			gamma_by_Brodtkorb2004( )
			if self.check_ratio_narrowing( ) :
				gamma_by_DNV( )
				if self.check_ratio_narrowing( ) :
					gamma_standard( )

	def check_ratio_narrowing( self, ) :
		GAM = self.ratioNarrow
		TP = self.periodPeak
		HS = self.heightSig

		if self.methRatioNarrow == 'DNV' :
			checkRatioNarrowing = ((GAM > 7. or GAM < 1.) or (3.6 < TP / np.sqrt( HS ) < 5.))
		elif self.methRatioNarrow == 'Brodtkorb' :
			checkRatioNarrowing = ((GAM > 7. or GAM < 1.) or (3.6 > TP / np.sqrt( HS ) > 5.))

		return checkRatioNarrowing

	def spectrum_jonswap( self, ratioPeakd = 1., M = 5., N = 4., ) :
		tau = lambda OM, OMP : np.where( OM <= OMP, 0.07, 0.09 )

		OM = self.OMDATA[ 0 ]
		SPECTRA = self.OMDATA[ 1 ]

		OMP = self.omPeak

		TAU = tau( OM, OMP )
		ALFA = ratioPeakd
		GAMMA = self.ratioNarrow
		HS = self.heightSig

		SPECTRA[ : ] = ALFA * (HS ** 2) * ((OMP ** 4) * (OM ** (-5)))
		SPECTRA[ : ] *= np.exp( -(M / N) * ((OMP / OM) ** N) )
		SPECTRA[ : ] *= GAMMA ** np.exp( -0.5 * (((OM / OMP) - 1) / TAU) ** 2 )

	def spectrum_pulsation_boundary( self, ratioPeakd = 0, recSp = 0 ) :
		def ratio_narrow_dependant_range( ) :
			gaW = np.array( [ 5.1101, 4.4501, 4.100, 3.8700,
			                  3.7000, 3.5700, 3.4600, 3.3700,
			                  3.2900, 3.2200, 3.1600, 3.1050,
			                  3.0550, 3.0100, 2.9650, 2.9300,
			                  2.8950, 2.8600, 2.8300, 2.8000 ] )

			fgaW = np.polyfit( x = np.arange( 1., 21. ), y = gaW, deg = 10. )
			poly = np.poly1d( fgaW )

			omMin = self.omPeak * (0.58 + (0.05 * (self.ratioNarrow - 1) / 19.))
			omMax = self.omPeak * poly( self.ratioNarrow )

			return np.float64( omMin ), np.float64( omMax )

		get_om_id_between_extremas = lambda OM, omMin, omMax, dom : (omMin - dom <= OM) & (OM <= omMax + dom)

		omMin, omMax = ratio_narrow_dependant_range( )

		if recSp :
			idxOmRanged = get_om_id_between_extremas( self.OMDATA[ 0, 0 ], omMin, omMax, self.dom )
			numOmRanged = np.sum( idxOmRanged )

			self.OMDATA[ :, 0, :numOmRanged ] = self.OMDATA[ :, 0, idxOmRanged ]

			self.OMDATA.resize(
				(
					self.OMDATA.shape[ 0 ],
					1,
					numOmRanged
				),
				refcheck = False )

			self.spectrum_jonswap( ratioPeakd )

			self.dom = np.float64( (omMax - omMin) / numOmRanged )

		else :
			idxOmRanged = get_om_id_between_extremas( self.OMFFTDATA[ 0, 0 ], omMin, omMax, self.domFFT )
			numOmRanged = np.sum( idxOmRanged )

			self.OMFFTDATA[ 0, 0, :2 * numOmRanged ].flat = self.OMFFTDATA[ :, 0, idxOmRanged ]

			self.OMFFTDATA.resize(
				(
					self.OMFFTDATA.shape[ 0 ],
					1,
					numOmRanged
				),
				refcheck = False )

			self.domFFT = np.float64( (omMax - omMin) / numOmRanged )

	def FFT_of_time_to_puls_domain( self ) :
		TIME = self.TIMEDATA.astype( 'f' )

		N = self.TIMEDATA.shape[ 2 ]

		self.OMFFTDATA[ 0 ] = 2 * PI * np.fft.fftfreq( N, self.dt )

		ID = self.OMFFTDATA[ 0 ] >= 0

		self.domFFT = np.mean( np.diff( self.OMFFTDATA[ 0, :N // 2 ] ) )

		self.OMFFTDATA.resize( (self.OMFFTDATA.shape[ 0 ], 1, N // 2), refcheck = False )

		self.OMFFTDATA[ 1 ] = (2 * np.abs( np.fft.fft( TIME[ 1, 0 ], N )[ ID ] ) / N) ** 2 / (2 * self.domFFT)

	def spectrum_solver( self, solverOpt ) :
		print( '...spectrum_solver(%s)' % solverOpt )
		if solverOpt == 'Empirical' :
			self.spectrum_jonswap( self.ratioPeakad )
			recSp = 1

		elif solverOpt == 'FFT' :
			self.FFT_of_time_to_puls_domain( )
			recSp = 0

		self.spectrum_pulsation_boundary( self.ratioPeakad, recSp )

	def set_plot_styles( self, styleNum = None, styleName = None ) :
		'''
			plt.style.available
			plt.rc('text', usetex=True)
		'''
		if styleNum is None and styleName is None :
			plt.style.use( 'ggplot' )
		else :
			if styleName is None :
				names = [ u'dark_background', u'bmh', u'grayscale', u'ggplot', u'fivethirtyeight' ]
				name = names[ styleNum ]
			plt.style.use( name )

	def set_store_dirs( self, ) :
		STORES, DIRS, shipsParameters = \
			self.STORES, self.DIRS, self.shipsParameters

		self.STORE = STORES[ 'PATH_STORE_WAVES_IRREGULAR' ]

		self.dirTmp = DIRS[ 'DIR_FILES_TEMP' ]
		self.dirPlot = DIRS[ 'DIR_FIGS_WAVES_IRREGULAR_JONSWAP' ]

		idx = [
			'waves_nameSpectrum',
			'waves_nameSeas',
			'waves_heightSig',
			'waves_periodPeak'
		]

		for shipName in shipsParameters.keys( ) :
			DATA = pd.DataFrame(
				[
					'JONSWAP',
					self.nameSeas,
					self.heightSig,
					self.periodPeak,
				],
				index = idx,
				columns = self.counter,
			)

			k = shipName.replace( ' ', '_' ).upper( )

			key = '/SHIP/%s/WAVES_DATA' % k

			if (key in self.STORE) :
				DATA0 = self.STORE[ key ]

				if not self.counter[ 0 ] in DATA0.columns.values :
					DATA = pd.concat( [ DATA0, DATA ], axis = 1 )

			un.ppr( key, DATA )

			self.STORE[ key ] = DATA

	def set_memmaps( self ) :
		self.shapeOm, self.shapeTime = \
			un.roundup( (self.omMax - self.omMin) / self.dom ), \
			un.roundup( self.numSteps / self.dt )

		self.shapeMemmapOm, self.shapeMemmapOmFFT, self.shapeMemmapTime = \
			(4, 1, self.shapeOm), \
			(2, self.shapeTime), \
			(2, 1, self.shapeTime, 1)

		if 0 :
			self.fnameMemmapOm = self.dirTmp + '\\OM.npy'
			self.fnameMemmapOmFFT = self.dirTmp + '\\OMFFT.npy'
			self.fnameMemmapTime = self.dirTmp + '\\TIME.npy'
		else :
			self.fnameMemmapOm = self.dirTmp + '\\OM_numSteps%d.npy' % (math.ceil( self.numSteps ))
			self.fnameMemmapOmFFT = self.dirTmp + '\\OMFFT_numSteps%d.npy' % (math.ceil( self.numSteps ))
			self.fnameMemmapTime = self.dirTmp + '\\TIME_numSteps%d.npy' % (math.ceil( self.numSteps ))

		if platform != 'win32' :
			self.fnameMemmapOm = self.fnameMemmapOm.replace( '\\', '/' )
			self.fnameMemmapOmFFT = self.fnameMemmapOmFFT.replace( '\\', '/' )
			self.fnameMemmapTime = self.fnameMemmapTime.replace( '\\', '/' )

		self.fnamesMemmap = self.fnameMemmapOm, self.fnameMemmapOmFFT, self.fnameMemmapTime

		np.arange( 0, self.numSteps, self.dt, dtype = 'd' ).tofile( self.fnameMemmapTime )
		np.arange( self.omMin, self.omMax, self.dom, dtype = 'd' ).tofile( self.fnameMemmapOm )

		self.OMDATA = np.require(
			np.memmap( self.fnameMemmapOm, 'd', 'r+', 0, self.shapeMemmapOm, 'C' ), requirements = [ 'O' ] )
		self.OMFFTDATA = np.require(
			np.memmap( self.fnameMemmapOmFFT, 'd', 'w+', 0, self.shapeMemmapOmFFT, 'C' ), requirements = [ 'O' ] )
		self.TIMEDATA = np.memmap( self.fnameMemmapTime, 'd', 'r+', 0, self.shapeMemmapTime, 'C' )

	def memmaps_close( self ) :
		del self.OMDATA
		del self.OMFFTDATA
		del self.TIMEDATA

	def data_saver( self ) :
		# self.heightSig,self.periodPeak,oB,oD,fetch,Vwind,GAMMAOPT,ratioNarrow,ratioPeakd=seasSpectraInitials
		# self.dt,dom,TIME,wave_elevation,self.omMax,SPECTRA,EPS,OMFFT,SPECTRAFFT=seasParams
		# Moments1,Moments2,Heights1,Heights2,Periods1,Periods2=seasSpectraParams

		STORE = self.STORE

		NAMESEAS = self.nameSeas

		HS, TP = self.heightSig, self.periodPeak
		keyHS = str( HS ).replace( '-', 'm' ).replace( '.', '_' )
		keyTP = str( TP ).replace( '-', 'm' ).replace( '.', '_' )

		keyWaves = '/WAV/IRR/JONSWAP/SEAS/%s/%s/HS_%s/TP_%s/'
		keyInputParams = keyWaves % (NAMESEAS, 'PARAMETERS/SPECTRUM_INPUT', keyHS, keyTP)
		keyWavesTime = keyWaves % (NAMESEAS, 'TIME', keyHS, keyTP)
		keyWavesPhase = keyWaves % (NAMESEAS, 'PHASE', keyHS, keyTP)
		keyWavesPulse = keyWaves % (NAMESEAS, 'PULSATION', keyHS, keyTP)
		keyWavesElevation = keyWaves % (NAMESEAS, 'ELEVATION', keyHS, keyTP)
		keyWavesSpectrum = keyWaves % (NAMESEAS, 'SPECTRUM', keyHS, keyTP)
		keyWavesSpectrumFFT = keyWaves % (NAMESEAS, 'SPECTRUM_FFT', keyHS, keyTP)
		keyWavesMoments = keyWaves % (NAMESEAS, 'PARAMETERS/MOMENTS', keyHS, keyTP)
		keyWavesHeights = keyWaves % (NAMESEAS, 'PARAMETERS/HEIGHTS', keyHS, keyTP)
		keyWavesPeriods = keyWaves % (NAMESEAS, 'PARAMETERS/PERIODS', keyHS, keyTP)

		self.keyInputParams = keyInputParams.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesTime = keyWavesTime.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesPhase = keyWavesPhase.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesPulse = keyWavesPulse.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesElevation = keyWavesElevation.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesSpectrum = keyWavesSpectrum.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesSpectrumFFT = keyWavesSpectrumFFT.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesMoments = keyWavesMoments.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesHeights = keyWavesHeights.replace( '-', 'm' ).replace( '.', '_' )
		self.keyWavesPeriods = keyWavesPeriods.replace( '-', 'm' ).replace( '.', '_' )

		STORE[ self.keyInputParams ] = pd.Series( {
			'Hs'                            : HS,
			'Tp'                            : TP,
			'nameSeas'                      : NAMESEAS,
			'dom'                           : self.dom,
			'domFFT'                        : self.domFFT,
			'dt'                            : self.dt,
			'oBeaufort'                     : self.degBeaufort,
			'oDouglas'                      : self.degDouglass,
			'fetch'                         : self.fetch,
			'wind velocity'                 : self.velWind,
			'gammaMethod'                   : self.methRatioNarrow,
			'gamma_' + self.methRatioNarrow : self.ratioNarrow,
			'alfa'                          : self.ratioPeakad
		} )

		MOMENTS = pd.DataFrame( { 'Mn(S)[Nm]' : self.MOMENTS, 'Mn(PSD.FFT(z))[Nm]' : self.MOMENTSFFT }, dtype = 'd' )
		HEIGHTS = pd.DataFrame( { 'H(S)[m]' : self.HEIGHTS, 'H(PSD.FFT(z))[m]' : self.HEIGHTSFFT }, dtype = 'd' )
		PERIODS = pd.DataFrame( { 'T(S)[s]' : self.PERIODS, 'T(PSD.FFT(z))[s]' : self.PERIODS }, dtype = 'd' )

		STORE[ self.keyWavesTime ] = pd.DataFrame( { 't' : self.TIMEDATA[ 0, 0 ] }, dtype = 'd' )
		STORE[ self.keyWavesPhase ] = pd.DataFrame( { 'eps' : self.OMDATA[ 2, 0 ] }, dtype = 'd' )
		STORE[ self.keyWavesPulse ] = pd.DataFrame( { 'om' : self.OMDATA[ 0, 0 ] }, dtype = 'd' )
		STORE[ self.keyWavesElevation ] = pd.DataFrame( { 'z' : self.TIMEDATA[ 1, 0 ] }, index = self.TIMEDATA[ 0, 0 ],
		                                                dtype = 'd' )
		STORE[ self.keyWavesSpectrum ] = pd.DataFrame( { 'S_wave' : self.OMDATA[ 1, 0 ] }, index = self.OMDATA[ 0, 0 ],
		                                               dtype = 'd' )
		STORE[ self.keyWavesSpectrumFFT ] = pd.DataFrame( { 'S_wave.fft(z)' : self.OMFFTDATA[ 1, 0 ] },
		                                                  index = self.OMFFTDATA[ 0, 0 ], dtype = 'd' )

		STORE[ self.keyWavesMoments ] = MOMENTS
		STORE[ self.keyWavesHeights ] = HEIGHTS
		STORE[ self.keyWavesPeriods ] = PERIODS

	def data_plotter( self, ) :
		import matplotlib.pyplot as plt
		from matplotlib import cm
		import matplotlib.gridspec as gridspec
		from matplotlib.patches import Ellipse

		STORE = self.STORE

		HS, TP, oB, oD, fetch, Vwind, GAMMAOPT, GAMMA, ALFA, nameSeas = \
			self.heightSig, self.periodPeak, self.degBeaufort, self.degDouglass, \
			self.fetch, self.velWind, self.methRatioNarrow, self.ratioNarrow, \
			self.ratioPeakad, self.nameSeas

		dt, dom, TIME, ELEVATION, OM, SPECTRA, EPS, OMFFT, SPECTRAFFT = \
			self.dt, self.dom, self.TIMEDATA[ 0, 0 ], self.TIMEDATA[ 1, 0 ], self.OMDATA[ 0, 0 ], \
			self.OMDATA[ 1, 0 ], self.OMDATA[ 2, 0 ], self.OMFFTDATA[ 0, 0 ], self.OMFFTDATA[ 1, 0 ]

		MOMENTS = STORE[ self.keyWavesMoments ]
		HEIGHTS = STORE[ self.keyWavesHeights ]
		PERIODS = STORE[ self.keyWavesPeriods ]

		pathFilePlot = self.dirPlot + 'IW_JONSWAP__Seas(' + nameSeas + ')_Hs(' + str( HS ) + ')_Tp(' + str(
			TP ) + ')_t(' + str( np.around( np.max( TIME ), 2 ) ) + ')'

		pathFilePlot = pathFilePlot.replace( '.', '_' ).replace( ' ', '-' ).replace( '_', '-' ) + '.pdf'

		print( 'plot saving: ', pathFilePlot )

		if un.files_check_if_exists( pathFilePlot ) :
			return

		fig = plt.figure( 1 )

		plt.clf( )

		fig.suptitle(
			r'JONSWAP SPECTRA PARAMETERS: sign. height: $' + 'H_s=' + str( HS ) + '[m]$, peak period: $T_p=' + str(
				TP ) +
			'[s]$, ( $ {3.6} < {T_p}/{\sqrt{H_s}} < {5} $ == $' + str( 3.6 < TP / np.sqrt( HS ) < 5 ) +
			'$ ), peakadness ratio: $ c_{1}=' + str( np.round( ALFA, decimals = 4 ) ) +
			'[-]$, narrowing ratio: $ c_{2}=' + str( np.round( GAMMA, decimals = 4 ) ) +
			'[-]$; \n TIME/FREQUENCY DOMAIN SIMULATION INITIAL PARAMETERS: time step $/$ time of simulation: $\delta t$/$t = '
			+ str( np.round( dt, decimals = 4 ) ) + '$/$' + str( int( np.max( TIME ) - np.min( TIME ) ) ) +
			'[s]$ , step/min/max pulsation: $\delta \omega$/$\omega_{min}$/$\omega_{max} = '
			+ str( np.round( dom, 5 ) ) + '$/$' + str( np.round( min( OM ), 3 ) ) + '$/$' + str(
				np.round( max( OM ), decimals = 3 ) ) +
			'[rad/s]$; \n SEA STATE: Beaufort wind scale degree : $ oB = ' + str( oB ) +
			'[-]$, Douglas waves height scale degree: $oD = ' + str( oD ) +
			'[-]$; SEA CHARACTERISTIC: wind velocity: $V_{wind} = ' + str( np.round( Vwind, 0 ) ) +
			'[m/s]$, fetch: $F = ' + str( np.round( fetch, 0 ) ) + '[m]$.' )

		gs = gridspec.GridSpec( 15, 4 )
		ax1 = plt.subplot( gs[ :8, :3 ] )
		# ax2 = plt.subplot(gs[:4,:1],axisbg='y')
		ax3 = plt.subplot( gs[ 9 :, :3 ] )
		ax4 = plt.subplot( gs[ 7 :, 3 : ] );
		ax4.grid( False );
		ax4.axis( 'off' )

		ax1.plot( TIME, ELEVATION, label = r'$\zeta(t) = \sum_{i=1}^{N} A_i (\omega) * sin(\omega_i t + \phi_i)$',
		          linewidth = 2, c = 'b' )

		ax1.plot( TIME, np.zeros( shape = len( ELEVATION ) ), c = 'k', linewidth = 5 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 0, 0 ] / 2., c = 'm', label = r'$RMS(S)$',
		          linewidth = 2 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 1, 0 ] / 2., c = 'r',
		          label = r'$A_{1/2}(S)$', linewidth = 2 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 2, 0 ] / 2., c = 'c',
		          label = r'$A_{1/3}(S)$', linewidth = 2 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 3, 0 ] / 2., c = 'y',
		          label = r'$A_{1/10}(S)$', linewidth = 2 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 0, 1 ] / 2., '--', c = 'm',
		          label = r'$RMS(PSD.FFT)$', linewidth = 3 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 1, 1 ] / 2., '--', c = 'r',
		          label = r'$A_{1/2}(PSD.FFT)$', linewidth = 3 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 2, 1 ] / 2., '--', c = 'c',
		          label = r'$A_{1/3}(PSD.FFT)$', linewidth = 3 )
		ax1.plot( TIME, np.ones( shape = len( ELEVATION ) ) * HEIGHTS.iloc[ 3, 1 ] / 2., '--', c = 'y',
		          label = r'$A_{1/10}(PSD.FFT)$', linewidth = 3 )

		ax1.set_xlim( [ -0.1, TIME.max( ) + 0.1 ] )
		ax1.set_ylim( [ ELEVATION.min( ) - 0.1, ELEVATION.max( ) + 0.1 ] )

		ax3.plot( OM, SPECTRA, '-', linewidth = 10, c = 'r',
		          label = r'$S_{JONSWAP}(\omega)= {c_{1} H_s^2}{\omega^{-5} \omega_P^{-4}}'
		                  r' exp \left[-\frac{5}{4} \left(\frac{\omega_P}{\omega} \right)^4 \right] '
		                  r' c_{2}^{exp \left[-0.5 \left(\frac{\omega - \omega_P}{\tau \omega_P}\right)^2 \right]}$' )

		ax3.stem( OMFFT, SPECTRAFFT, linewidth = 0.001, c = 'g',
		          label = r'$ S_{JONSWAP}(\omega) = PSD.FFT(\zeta(t))$ : histogram' )
		ax3.plot( OMFFT, SPECTRAFFT, '-', linewidth = 4.000, c = 'w',
		          label = r'$ S_{JONSWAP}(\omega) = PSD.FFT(\zeta(t))$' )

		table1 = ax4.table( cellText = np.around( MOMENTS.values, 2 ), rowLabels = MOMENTS.index,
		                    colLabels = MOMENTS.columns, loc = 'top', cellLoc = 'center',
		                    bbox = [ 0.1, 0.7, 1.45, 0.35 ] )
		table2 = ax4.table( cellText = np.around( HEIGHTS.values, 2 ), rowLabels = HEIGHTS.index,
		                    colLabels = HEIGHTS.columns, loc = 'center', cellLoc = 'center',
		                    bbox = [ 0.6, 0.3, 1., 0.35 ] )
		table3 = ax4.table( cellText = np.around( PERIODS.values, 2 ), rowLabels = PERIODS.index,
		                    colLabels = PERIODS.columns, loc = 'bottom', cellLoc = 'center',
		                    bbox = [ 0.7, -0.1, .98, 0.35 ] )

		table1.scale( 1.5, 1.5 )
		table2.scale( 1.15, 1.3 )
		table3.scale( 1.15, 1.25 )

		ax1.set_ylabel( r'$ \zeta(t) [m]$' );
		ax1.set_xlabel( r'$ t [s] $' );
		# ax2.set_ylabel(r'$ z(t) [m]$');ax2.set_xlabel(r'$ t [s] $');
		ax3.set_ylabel( r'$ S(\omega) [m^2 s]$' );
		ax3.set_xlabel( r'$ \omega [\frac{rad}{s}]$' )

		ax1.legend( ncol = 1, title = 'Wave elevation', bbox_to_anchor = (1.4, 1.) );
		ax3.legend( ncol = 1, title = 'Spectra JONSWAP' )

		ax3.set_xlim( [ OM.min( ) - 0.1, 0.1 + OM.max( ) ] )
		ax3.set_ylim( [ -0.1, 0.5 + SPECTRA.max( ) ] )

		plt.savefig( pathFilePlot, dpi = 1000 )
		plt.close( )

		#
