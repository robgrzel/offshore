# include <FL/x.H>
#  include <FL/Fl.H>
#  include <FL/Fl_Double_Window.H>
#  include <FL/Fl_Browser.H>
#  include <FL/Fl_Value_Output.H>
#  include <FL/fl_ask.H>
#include <iostream>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <mgl2/fltk.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>


// Should probably ifdef WIN32 this, and pull in pthread.h and
// perhaps sched.h for non-Windows platforms too. 
#ifdef _WIN32
	#include <process.h>
#else
#endif


class Thread_Gui_Control :
		public Fl_Gl_Window {

	private:
		//static bool doThread;
		bool       doThread;
		const int  myId;
		static int tcntWin;
		static int ecntWin;
		const int           * pmyId     = &myId;
		static constexpr int* ptcntWin  = &tcntWin;
		static constexpr int* pecntWin  = &ecntWin;
		bool                * pdoThread = &doThread;

	public:
		// Hmm, I'm uncomfortable about initializing the member vars like this,
		// it may not prove to be all that portable...
		// Better to do this in the constructor I'd suggest.
		register int frame         = 0;
		int          redrawCounter = 0;
		long         i;            // another variable to be shown

		char* buf;

		std::string count = "";

		size_t size;

		pthread_t pthread;

		mglWnd* wnd;    // external window for plotting
		mglPoint pnt;    // some variable for changeable data

		mglData xMglData;
		mglData yMglData;

		Fl_Value_Output* valout;
		Fl_Browser     * browser;
		Fl_Box         * redrawCount;


		float Linterp( float frac, float a, float b ) {
			return (a + (frac * (b - a)));
		}

		// Sinusoidal easein/easeout interpolation between two values based on 'frac' (0.0=a, 1.0=b)
		float SinInterp( float frac, float a, float b ) {
			float pi = 3.14159;
			frac = (sin( pi / 2 + frac * pi ) + 1.0) / 2.0;     // 0 ~ 1 -> 0 ~ 1
			return (Linterp( frac, a, b ));
		}

		// DRAW SIMPLE SHAPE INTERPOLATION
		//     Interpolation is based on the current frame number
		//
		void DrawShape( int frame ) {
			// Calculate a fraction that represents the frame# being shown
			float frac = (frame % 48) / 48.0 * 2;
			if (frac > 1.0) frac = 2.0 - frac;      // saw tooth wave:  "/\/\/\"

			static float a_xy[9][2] = {
					{-.5, -1.},
					{0.0, -.5},
					{-.5, -1.},
					{0.0, -.5},
					{0.0, 0.0},
					{0.0, -.5},
					{+.5, -1.},
					{0.0, -.5},
					{+.5, -1.},
			};
			static float b_xy[9][2] = {
					{-.25, -1.},
					{-.50, -.75},
					{-.75, -1.0},
					{-.50, -.75},
					{0.0,  0.0},
					{+.50, -.75},
					{+.75, -1.0},
					{+.50, -.75},
					{+.25, -1.0}
			};
			// Linterp a and b to form new shape c
			float        c_xy[9][2];
			for ( int    i          = 0; i < 9; i++ ) {
				for ( int xy = 0; xy < 2; xy++ ) {
					c_xy[i][xy] = SinInterp( frac, a_xy[i][xy], b_xy[i][xy] );
				}
			}
			// Draw shape
			glColor3f( 1.0, 1.0, 1.0 );
			glBegin( GL_LINE_STRIP );
			for ( int i = 0; i < 9; i++ ) {
				glVertex2f( c_xy[i][0], c_xy[i][1] );
			}
			glEnd();
		}

		void turnoff_thread( ) {
			std::cout << "turnoff_thread(0): id=" << pmyId << std::endl;
			std::cout << "turnoff_thread(0): pthread=" << pthread << std::endl;
			doThread = false;
		};

		void turnon_thread( ) {
			doThread = true;
		};

		bool get_dothread( ) {
			return doThread;
		};

		bool* get_pdothread( ) {
			return pdoThread;
		};

		int get_id( ) {
			return myId;
		};

		void dec_existing_windows_counter( ) {
			ecntWin--;
		};

		int get_existing_windows_counter( ) {
			return ecntWin;
		};

		int gui_delete_windows( ) {
			std::cout << "~Thread_Gui_Control(1)..." << std::endl;
			Fl::delete_widget( valout );
			std::cout << "~Thread_Gui_Control(2)..." << std::endl;
			Fl::delete_widget( browser );
			std::cout << "~Thread_Gui_Control(3)..." << std::endl;
			Fl::delete_widget( redrawCount );
			std::cout << "~Thread_Gui_Control(4)..." << std::endl;
			Fl::delete_widget( this );
			std::cout << "~Thread_Gui_Control(E)..." << std::endl;
			valout      = 0;
			browser     = 0;
			redrawCount = 0;
			return 0;
		}

		Thread_Gui_Control( int xWin, int yWin, int wWin, int hWin, const char* l );


		void thread_start( );

		void thread_body_plotter( );

		static void* thread_enter_plotter( void* p );

		const char* strKey = "Simulation begin in %d [s]\n";


		void draw( ) {

		};

		static void Timer_CB( void* userdata ) {
			Thread_Gui_Control* pb = ( Thread_Gui_Control* ) userdata;
			pb->redraw();
			Fl::repeat_timeout( 1. / 10000., Timer_CB, userdata );
		}

};

//bool Thread_Gui_Control::doThread = true; // this now can go in header
//int  Thread_Gui_Control::myId     = 0; // this now can go in header
int  Thread_Gui_Control::tcntWin = 0; // this now can go in header
int  Thread_Gui_Control::ecntWin = 0; // this now can go in header

//constexpr int * Thread_Gui_Control::pmyId;

void Thread_Gui_Control::thread_start( ) {
	std::cout << "---> thread_start(0)..." << std::endl;
	// I'd add some #ifdef's for portability..
	this->turnon_thread();

	#ifdef _WIN32
	_beginthread(Thread_Gui_Control::thread_enter, 0, this);
	#else
	pthread_create( &pthread, 0, this->thread_enter_plotter, this );
	#endif

	std::cout << "pthread=" << pthread << std::endl;

	std::cout << "<--- thread_start(E)..." << std::endl;

}


void Thread_Gui_Control::thread_body_plotter( ) {
	std::cout << "---> thread_body_plotter(0)..." << std::endl;
	// There's no way to signal this thread to expire, since you are not
	// holding on to its handle, nor are you joining it, or etc...
	// Using a global static var is possible the easiest way to go.
	//        while (true){
	std::string count;
	//int redrawCounter =0;
	std::cout << "thread_body_plotter(1)... pdoThread=" << pdoThread << std::endl;
	std::cout << "thread_body_plotter(2)... myId=" << myId << std::endl;
	std::cout << "...\n" << std::endl;

	while ( *pdoThread ) {
		if (!valid()) {
			valid( 1 );
			glLoadIdentity();
			glViewport( 0, 0, w(), h() );
		}
		glClear( GL_COLOR_BUFFER_BIT );
		// Draw shape 4x, rotated at 90 degree positions
		glPushMatrix();
		DrawShape( frame );
		glRotatef( 90.0, 0, 0, 1 );
		DrawShape( frame );
		glRotatef( 90.0, 0, 0, 1 );
		DrawShape( frame );
		glRotatef( 90.0, 0, 0, 1 );
		DrawShape( frame );
		glPopMatrix();
		// Advance frame counter
		++frame;

	}

	std::cout << "<--- thread_body_plotter(E)..." << std::endl;

}


void* Thread_Gui_Control::thread_enter_plotter( void* p ) {
	std::cout << "---> thread_enter_browser(0)..." << std::endl;
	(( Thread_Gui_Control* ) p)->thread_body_plotter();
	// Calling endthread here is probably redundant, as you are returning from the
	// function at this point, and that is supposed to clean up for you anyway.
	std::cout << "<--- thread_enter_browser(E)..." << std::endl;
	pthread_exit( 0 );

}

static void window_cb( Fl_Widget*, void* v ) {
	std::cout << "window_cb(0)" << std::endl;
	Thread_Gui_Control* gui = ( Thread_Gui_Control* ) v;
	std::cout << "window_cb(1)" << std::endl;
	gui->turnoff_thread();
	std::cout << "window_cb(2)" << std::endl;
	gui->gui_delete_windows();
	std::cout << "window_cb(3)" << std::endl;
	gui->dec_existing_windows_counter();
	std::cout << "window_cb(4)" << std::endl;

	if (gui->get_existing_windows_counter() <= 0) {
		std::cout << "window_cb(4.0)" << std::endl;
		exit( 0 );
	} else {
		std::cout << "window_cb(4.1)" << std::endl;
		gui->hide();
		//delete gui; //Process finished with exit code 139 (interrupted by signal 11: SIGSEGV)

	};

	std::cout << "window_cb(E)" << std::endl;
}

Thread_Gui_Control::Thread_Gui_Control( int xWin, int yWin, int wWin, int hWin, const char* l ) :
		Fl_Gl_Window( 300, 50, 100, 100, "Glut" ),
		myId( ++tcntWin ) {
	ecntWin++;

	std::cout << "---> Thread_Gui_Control(0)..." << std::endl;
	std::cout << "pmyId=" << pmyId << ", pdoThread=" << pdoThread << std::endl;

	int hSpc = 10;

	int xBox = 200 - 55, yBox = hSpc;
	int wBox = 100, hBox = 20;
	xBox -= wBox / 2;
	int xBrw = 55, yBrw = hSpc + yBox + hBox;
	int wBrw = 200, hBrw = 175;
	int xVot = 55, yVot = hSpc + yBrw + hBrw;
	int wVot = 200, hVot = 50;

	frame = 0;
	//Fl::add_timeout( 1. / 10000., Timer_CB, this );       // 24fps timer

	redrawCount = new Fl_Box( xBox, yBox, wBox, hBox );

	browser = new Fl_Browser( xBrw, yBrw, wBrw, hBrw );
	valout  = new Fl_Value_Output( xVot, yVot, wVot, hVot, "Max\nCount" );

	xMglData.Create( 1, 0, 0 );
	yMglData.Create( 1, 0, 0 );

	redrawCount->label( "Pending..." );

	this->end();

	this->resizable( browser );
	this->resizable( this );
	this->show();

	this->callback( ( Fl_Callback* ) window_cb, this );

	this->thread_start();

	std::cout << "<--- Thread_Gui_Control(E)..." << std::endl;

}


int main( ) {
	/*
	 * TODO:
	 * creating multiple obj Thread_Gui_Control and closing one of them may cause exception
	 * (ie. Process finished with exit code 134 (interrupted by signal 6: SIGABRT))
	 * from child threads (both)
	 */

	Thread_Gui_Control* tpo0 = new Thread_Gui_Control( 0, 0, 800, 600, "Threaded Gui 1" );
	//Thread_Gui_Control* tpo1 = new Thread_Gui_Control( 500, 0, 300, 300, "Threaded Gui 2" );
	int exit_status = Fl::run();
	return exit_status;
}