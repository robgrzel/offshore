
#include <stdio.h>
#include <iostream>
#include <mgl2/mgl.h>
#include <mgl2/qt.h>
#include <mgl2/fltk.h>
#include <mgl2/glut.h>
#include <unistd.h>


void mgls_prepare1d( mglData* y, mglData* y1 = 0, mglData* y2 = 0, mglData* x1 = 0, mglData* x2 = 0 ) {
	register long i, n = 50;
	if (y) y->Create( n, 3 );
	if (x1) x1->Create( n );
	if (x2) x2->Create( n );
	if (y1) y1->Create( n );
	if (y2) y2->Create( n );
	mreal xx;
	for ( i = 0; i < n; i++ ) {
		xx = i / (n - 1.);
		if (y) {
			y->a[i]         = 0.7 * sin( 2 * M_PI * xx ) + 0.5 * cos( 3 * M_PI * xx ) + 0.2 * sin( M_PI * xx );
			y->a[i + n]     = sin( 2 * M_PI * xx );
			y->a[i + 2 * n] = cos( 2 * M_PI * xx );
		}

		if (y1) y1->a[i] = 0.5 + 0.3 * cos( 2 * M_PI * xx );
		if (y2) y2->a[i] = 0.3 * sin( 2 * M_PI * xx );
		if (x1) x1->a[i] = xx * 2 - 1;
		if (x2) x2->a[i] = 0.05 + 0.03 * cos( 2 * M_PI * xx );
	}
}

int sample( mglGraph* gr ) {
	gr->NewFrame();             // the first frame
	gr->Rotate( 60, 40 );
	gr->Box();
	gr->EndFrame();             // end of the first frame
	gr->NewFrame();             // the second frame
	gr->Box();
	gr->Axis( "xy" );
	gr->EndFrame();             // end of the second frame
	return gr->GetNumFrame();   // returns the frame number
}

int sample1( mglGraph* gr ) {
	mglData y;
	mgls_prepare1d( &y );
	gr->SetOrigin( 0, 0, 0 );
	gr->SubPlot( 2, 2, 0, "" );
	gr->Title( "Plot plot (default)" );
	gr->Box();
	gr->Plot( y );

	gr->SubPlot( 2, 2, 2, "" );
	gr->Title( "'!' style; 'rgb' palette" );
	gr->Box();
	gr->Plot( y, "o!rgb" );

	gr->SubPlot( 2, 2, 3, "" );
	gr->Title( "just markers" );
	gr->Box();
	gr->Plot( y, " +" );

	gr->SubPlot( 2, 2, 1 );
	gr->Title( "3d variant" );
	gr->Rotate( 50, 60 );
	gr->Box();
	mglData yc( 30 ), xc( 30 ), z( 30 );
	z.Modify( "2*x-1" );
	yc.Modify( "sin(pi*(2*x-1))" );
	xc.Modify( "cos(pi*2*x-pi)" );
	gr->Plot( xc, yc, z, "rs" );
	return 0;
}


class Foo : public mglDraw {
	public:
		int Draw( mglGraph* gr );
};

int Foo::Draw( mglGraph* gr ) {
	gr->Rotate( 60, 40 );
	gr->Box();
	return 0;
}

void long_calculations( ) {
	for ( int i = 0; i < int( 1e6 ); i++ ) {
		i *= 1;
	}
}


int mathgl_animate_to_gif( ) {
	mglGraph  gr;
	mglData   dat( 100 );
	char      str[32];
	//gr.StartGIF("sample.gif");
	for ( int i = 0; i < 40; i++ ) {
		gr.NewFrame();     // start frame
		gr.Box();          // some plotting
		for ( int j = 0; j < dat.nx; j++ ) {
			dat.a[j] = sin( M_PI * j / dat.nx + M_PI * 0.05 * i );
		}
		gr.Plot( dat, "b" );
		gr.EndFrame();     // end frame
	}
	//gr.CloseGIF();
	return 0;
}

void mathgl_save_to_file( ) {
	mglGraph gr;
	gr.Light( true );
	sample( &gr );              // The same drawing function.
	gr.WriteEPS( "test.eps" );  // Don't forget to save the result!
}

int mathgl_qt_window( ) {
	Foo   foo;
	mglQT gr( &foo, "MathGL examples" );

	return gr.Run();
}

int mathgl_glut_window( ) {
	Foo     foo;
	mglGLUT gr( sample1, "MathGL examples" );

	return gr.Run();
}

mglFLTK* gr = NULL;    // pointer to window
void* calc1( void* )    // function with calculations
{
	mglPoint   pnt;    // some data for plot
	for ( long i = 0;; i++ )        // do calculation
	{
		//long_calculations();	// which can be very long
		pnt.Set( 2 * mgl_rnd() - 1, 2 * mgl_rnd() - 1 );
		if (gr) {
			gr->Clf();            // make new drawing
			// draw something
			gr->Line( mglPoint(), pnt, "Ar2" );
			char str[16];
			snprintf( str, 15, "i=%ld", i );
			gr->Puts( mglPoint(), str );
			// don't forgot to update window
			gr->Update();
		}
	}
}

#include <memory>

template <typename ... Args>
const char* string_format( const char* format, Args ... args ) {
	size_t                  size = size_t( snprintf( nullptr, 0, format, args ... ) + 1 ); // Extra space for '\0'
	std::unique_ptr<char[]> buf( new char[size] );
	snprintf( buf.get(), size, format, args ... );
	const char* str = buf.get();
	return str; // We don't want the '\0' inside
}

pthread_mutex_t mutex;
pthread_cond_t  mcond1;
pthread_cond_t  mcond2;
pthread_cond_t  mcond3;
pthread_cond_t  condWinClosed;
mglFLTK* mgr = NULL;
bool calc_locked1 = true;
bool calc_locked2 = true;

mglPoint pnt;    // some data for plot
size_t   size;
char* buf;




void* thread_window_plot_init( void* t ) {

	long my_id = ( long ) t;

	printf( "Starting thread_window_plot_init(0): thread %ld\n", my_id );

	pthread_mutex_lock( &mutex );

	printf( "thread_window_plot_init(1): calc_locked = %d, mcond2 = %d\n", calc_locked1, mcond2 );
	while ( calc_locked1 ) {
		pthread_cond_wait( &mcond2, &mutex );
	}
	printf( "thread_window_plot_init(1.5): calc_locked = %d, mcond2 = %d\n", calc_locked1, mcond2 );

	printf( "thread_window_plot_init(2): calc_locked = %d\n", calc_locked1 );

	const char* strKey = "Simulation begin in %d [s]\n";

	for ( uint i = 0; i < 1; i++ ) {
		size = size_t( snprintf( nullptr, 0, strKey, i ) + 1 ); // Extra space for '\0'
		buf  = new char[size];
		snprintf( buf, size, strKey, i );
		const char* str = buf;

		mgr->Clf();
		mgr->Puts( mglPoint( 0, 0.6 ), str );
		mgr->Update();
		sleep( 1 );
	};


	calc_locked2 = false;
	pthread_cond_signal( &mcond3 );
	pthread_mutex_unlock( &mutex );

	printf( "<---Ending thread_window_plot_init(e): thread %ld\n", my_id );
	pthread_exit( NULL );


}

void* thread_window_plot_calc( void* t ) {

	long my_id = ( long ) t;

	printf( "Starting thread_window_plot_calc(0): thread %ld\n", my_id );

	pthread_mutex_lock( &mutex );
	printf( "thread_window_plot_calc(1):  cond3 = %d\n", calc_locked2 );

	while ( calc_locked2 ) {
		pthread_cond_wait( &mcond3, &mutex );
	};
	printf( "thread_window_plot_calc(2): cond3 = %d\n", calc_locked2 );

	uint i = 0;

	while ( mgr ) {
		printf( "mgr -> %d, %d\n", i, mgr);
		sleep( 1 );
		//long_calculations();	// which can be very long
		pnt.Set( 2 * mgl_rnd() - 1, 2 * mgl_rnd() - 1 );

		mgr->Clf();            // make new drawing
		// draw something
		mgr->Line( mglPoint(), pnt, "Ar2" );
		char str[16];
		snprintf( str, 15, "i=%u", i );
		mgr->Puts( mglPoint(), str );
		// don't forgot to update window
		mgr->Update();
		i++;
	};

	printf( "thread_window_plot_calc(3): mgr == NULL...\n" );

	pthread_mutex_unlock( &mutex );

	mgr->Clf();
	mgr->Update();

	printf( "<---Ending thread_window_plot_calc(E): thread %ld\n", my_id );
	pthread_exit( NULL );


}


void* thread_window_run( void* t ) {
	long my_id = ( long ) t;

	printf( "thread_window_run(0) : ...\n" );
	pthread_mutex_lock( &mutex );
	mglFLTK GR( "normal" );
	mgr = &GR;    // now create window
	GR.RunThr();            // and run event loop
	pthread_cond_signal( &mcond1 );
	pthread_mutex_unlock( &mutex );
	printf( "thread_window_run(1) : ...\n" );


	GR.Run();
	printf( "thread_window_run(E.1) : ...\n" );
	pthread_cond_signal(&condWinClosed);
	printf( "thread_window_run(E.2) : ...\n" );
	pthread_cond_broadcast(&condWinClosed);
	printf( "thread_window_run(E.3) : ...\n" );
	mgr = NULL;

	pthread_exit( NULL );

}

void* thread_window_watch( void* t ) {
	long my_id = ( long ) t;

	printf( "Starting watch_count(0): thread %ld\n", my_id );


	//Lock mutex and wait for signal.

	pthread_mutex_lock( &mutex );

	/*
	 if mgr is !NULL before this routine is run by the waiting thread,
	 the loop will be skipped to prevent pthread_cond_wait from never returning.
	 */

	while ( !mgr ) {
		printf( "watch_window(1): thread %ld calc_locked = %d.\n", my_id, calc_locked1 );
		pthread_cond_wait( &mcond1, &mutex );
	};

	printf( "watch_window(2): thread %ld Condition signal received.\n", my_id );

	calc_locked1 = false;
	printf( "watch_window(3): thread %ld calc_locked = %d.\n", my_id, calc_locked1 );

	// Note that the mcond routine will automatically and atomically unlock mutex while it waits.

	pthread_cond_signal( &mcond2 );

	pthread_mutex_unlock( &mutex );
	printf( "<---Ending watch_count(E): thread %ld\n", my_id );

	pthread_exit( NULL );

}
#include <signal.h>

void
signal_callback_handler(int signum) {
printf("Caught signal %d\n",signum);
// Cleanup and close up stuff here
// Terminate program

}


int mathgl_threads_draw( int argc, char** argv ) {

	long             t1 = 1, t2 = 2, t3 = 3, t4 = 4;
	static pthread_t threads[4];
	pthread_attr_t   tattr;

	printf( "### Init shared data...\n" );
	pthread_mutex_init( &mutex, NULL );
	pthread_cond_init( &mcond1, NULL );
	pthread_cond_init( &mcond2, NULL );
	pthread_cond_init( &mcond3, NULL );
	pthread_attr_init( &tattr );

	printf( "### Create threads...\n" );
	pthread_attr_setdetachstate( &tattr, PTHREAD_CREATE_JOINABLE );
	pthread_create( &threads[0], &tattr, thread_window_watch, ( void* ) t1 );
	pthread_create( &threads[1], &tattr, thread_window_plot_init, ( void* ) t2 );
	pthread_create( &threads[2], &tattr, thread_window_plot_calc, ( void* ) t3 );
	pthread_create( &threads[3], &tattr, thread_window_run, ( void* ) t4 );

	printf( "### Detach run window...\n" );
	pthread_detach( threads[3] );

	printf( "### Join...\n" );
	for ( int i = 0; i < 4; i++ ) { pthread_join( threads[i], NULL ); }

	printf( "### Exiting...\n" );
	pthread_attr_destroy( &tattr );
	pthread_mutex_destroy( &mutex );
	pthread_cond_destroy( &mcond1 );
	pthread_cond_destroy( &mcond2 );
	//pthread_exit( NULL );

	signal(NULL, signal_callback_handler);
	while(1) {
		printf("Program processing stuff here.\n");
		sleep(1);
		pthread_kill(t4,0);

	};


}


#include <QApplication>
#include <QMainWindow>
#include <QScrollArea>
#include <mgl2/qmathgl.h>

int mathgl_qt( int argc, char** argv ) {
	QApplication a( argc, argv );
	QMainWindow* Wnd = new QMainWindow;
	Wnd->resize( 810, 610 );  // for fill up the QMGL, menu and toolbars
	Wnd->setWindowTitle( "QMathGL sample" );
	// here I allow to scroll QMathGL -- the case
	// then user want to prepare huge picture
	QScrollArea* scroll = new QScrollArea( Wnd );

	// Create and setup QMathGL
	QMathGL* QMGL = new QMathGL( Wnd );
	//QMGL->setPopup(popup); // if you want to setup popup menu for QMGL
	//QMGL->setDraw(sample1);
	// or use
	Foo    * foo  = new Foo();
	QMGL->setDraw( foo ); //for instance of class Foo:public mglDraw
	QMGL->update();

	// continue other setup (menu, toolbar and so on)
	scroll->setWidget( QMGL );
	Wnd->setCentralWidget( scroll );
	Wnd->show();
	return a.exec();
}


#include <mgl2/mpi.h>
#include <mpi.h>

int mathgl_mpi( int argc, char* argv[] ) {
	// initialize MPI
	int rank = 0, numproc = 0;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &numproc );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	if (rank == 0) printf( "Use %d processes, rank=%d.\n", numproc, rank );
	//Next step is data creation. For simplicity, I create data arrays with the same sizes for all nodes. At this, you have to create mglGraph object too.

	// initialize data similarly for all nodes
	mglData     a( 128, 256 );
	mglGraphMPI gr;
	//Now, data should be filled by numbers. In real case, it should be some kind of calculations. But I just fill it by formula.

	// do the same plot for its own range
	char buf[64];
	sprintf( buf, "xrange %g %g", 2. * rank / numproc - 1, 2. * (rank + 1) / numproc - 1 );
	gr.Fill( a, "sin(2*pi*x)", buf );
	//It is time to plot the data. Don’t forget to set proper axis range(s) by using parametric form or by using options (as in the sample).

	// plot data in each node
	gr.Clf();   // clear image before making the image
	gr.Rotate( 40, 60 );
	gr.Surf( a, "", buf );
	//Finally, let send graphical information to node with rank=0.

	// collect information
	if (rank != 0) { gr.MPI_Send( 0 ); }
	else { for ( int i = 1; i < numproc; i++ ) gr.MPI_Recv( i ); }
	//Now, node with rank=0 have whole image. It is time to save the image to a file. Also, you can add a kind of annotations here – I draw axis and bounding box in the sample.

	if (rank == 0) {
		gr.Box();
		gr.Axis();   // some post processing
		gr.WritePNG( "test.png" ); // save result
	}
	//In my case the program is done, and I finalize MPI. In real program, you can repeat the loop of data calculation and data plotting as many times as you need.

	MPI_Finalize();
	return 0;
}


int main( int argc, char** argv ) {
	if (false) {
		printf( "mathgl_animate_to_gif...\n" );
		mathgl_animate_to_gif();
		printf( "mathgl_save_to_file...\n" );
		mathgl_save_to_file();
		printf( "mathgl_qt_window...\n" );
		mathgl_qt_window();
		printf( "mathgl_glut_window...\n" );
		mathgl_glut_window();
		printf( "mathgl_qt...\n" );
		mathgl_qt( argc, argv );
	} else if (true) {

		printf( "mathgl_threads_draw...\n" );
		mathgl_threads_draw( argc, argv );
		printf( "mathgl_mpi...\n" );
		//mathgl_mpi(argc,argv);
	} else {

	};
	return 0;
}

