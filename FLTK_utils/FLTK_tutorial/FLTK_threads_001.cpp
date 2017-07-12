# include <FL/x.H>
#  include <FL/Fl.H>
#  include <FL/Fl_Double_Window.H>
#  include <FL/Fl_Browser.H>
#  include <FL/Fl_Value_Output.H>
#  include <FL/fl_ask.H>
#include <iostream>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#include <mgl2/mgl.h>
#include <zconf.h>


// Should probably ifdef WIN32 this, and pull in pthread.h and
// perhaps sched.h for non-Windows platforms too.
#ifdef _WIN32
	#include <process.h>
#else
#endif

int sample( mglGraph* gr, void * p );
int sample1( mglGraph* gr, void * p );


class Thread_Gui_Control :
		public Fl_Double_Window {

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
		int redrawCounter = 0;
		std::string count = "";

		pthread_t pthread;

		mglGraph * mglgr;
		mglData xMglData;
		mglData yMglData;

		Fl_Value_Output* valout;
		Fl_Browser     * browser;
		Fl_Box         * redrawCount;

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

		void thread_body_counter( );

		void thread_body_browser( );

		static void* thread_enter_counter( void* p );

		static void* thread_enter_browser( void* p );

		void draw_plot();

		void draw(){
			std::cout<<"drawing(0)..."<<std::endl;

			Fl_Double_Window::draw();
			draw_plot();

			std::cout<<"drawing(E)..."<<std::endl;
		};

};

//bool Thread_Gui_Control::doThread = true; // this now can go in header
//int  Thread_Gui_Control::myId     = 0; // this now can go in header
int  Thread_Gui_Control::tcntWin = 0; // this now can go in header
int  Thread_Gui_Control::ecntWin = 0; // this now can go in header

//constexpr int * Thread_Gui_Control::pmyId;

register int num = 0;

void Thread_Gui_Control::draw_plot(){
	std::cout<<"Thread_Gui_Control(0)\n";
	if (mglgr) {
		std::cout<<"Thread_Gui_Control(1)\n";

		//mglgr->Clf();
		mglgr->Alpha( true );         // draws something using MathGL
		mglgr->Light( true );
		sample1( mglgr, this );
		fl_draw_image( mglgr->GetRGB(), 300, 100, mglgr->GetWidth(), mglgr->GetHeight(), 3 );
		std::cout<<"Thread_Gui_Control(2)\n";

	};
	std::cout<<"Thread_Gui_Control(E)\n";
}

void Thread_Gui_Control::thread_start( ) {
	std::cout << "---> thread_start(0)..." << std::endl;
	// I'd add some #ifdef's for portability..
	this->turnon_thread();

	#ifdef _WIN32
	_beginthread(Thread_Gui_Control::thread_enter, 0, this);
	#else
	pthread_create( &pthread, 0, this->thread_enter_browser, this );
	pthread_create( &pthread, 0, this->thread_enter_counter, this );
	#endif

	std::cout << "pthread=" << pthread << std::endl;

	std::cout << "<--- thread_start(E)..." << std::endl;

}

void magic_number_cb( void* p ) {
	Fl_Value_Output* w = ( Fl_Value_Output* ) p;
	w->labelcolor( FL_RED );
	w->redraw_label();
}

void magic_number_cb1( void* p ) {
	Fl_Value_Output* w = ( Fl_Value_Output* ) p;
	w->labelcolor( FL_BLUE );
	w->redraw_label();
}

void normal_number_cb( void* p ) {
	Fl_Value_Output* w = ( Fl_Value_Output* ) p;
	w->labelcolor( FL_BLACK );
	w->redraw_label();
}

void Thread_Gui_Control::thread_body_counter( ) {
	std::cout << "---> thread_body_counter(0)..." << std::endl;
	// There's no way to signal this thread to expire, since you are not
	// holding on to its handle, nor are you joining it, or etc...
	// Using a global static var is possible the easiest way to go.
	//        while (true){
	std::string count;
	//int redrawCounter =0;
	std::cout << "thread_body_counter(1)... pdoThread=" << pdoThread << std::endl;
	std::cout << "thread_body_counter(2)... myId=" << myId << std::endl;
	std::cout << "...\n" << std::endl;

	while ( *pdoThread ) {
		// Id move the lock from here...
		//            Fl::lock();
		count = "";
		count += std::to_string( redrawCounter );

		// Move lock to here; we want to be holding the lock as little as possible,
		// and only whilst altering fltk objects
		Fl::lock();
		if (redrawCount) {
			redrawCount->label( count.c_str() );
		};

		Fl::unlock();
		Fl::awake();

		redrawCounter++;
	}

	std::cout << "<--- thread_body_counter(E)..." << std::endl;

}

void Thread_Gui_Control::thread_body_browser( ) {
	std::cout << "---> thread_body_browser(0)..." << std::endl;
	// There's no way to signal this thread to expire, since you are not
	// holding on to its handle, nor are you joining it, or etc...
	// Using a global static var is possible the easiest way to go.
	//        while (true){
	std::string count;

	//int redrawCounter =0;
	std::cout << "thread_body_browser(1)... pdoThread=" << pdoThread << "<--\n";
	std::cout << "thread_body_browser(2)... myId=" << myId << "<--\n";

	while ( *pdoThread ) {
		sleep( 1 );
		// Id move the lock from here...
		count = "";
		count += std::to_string( redrawCounter );

		// Move lock to here; we want to be holding the lock as little as possible,
		// and only whilst altering fltk objects
		Fl::lock();

		if (browser && valout) {

			browser->add( count.c_str() );
			browser->bottomline( browser->size() );
			if (redrawCounter > valout->value()) {
				valout->value( redrawCounter );
			};

			if ((redrawCounter % 2) == 0) {
				Fl::awake( magic_number_cb, valout );
			} else if ((redrawCounter % 3) == 0) {
				Fl::awake( magic_number_cb1, valout );
			} else {
				Fl::awake( normal_number_cb, valout );
			};

		};

		Fl::unlock();
		Fl::awake();
		redrawCounter++;

	}

	std::cout << "<--- thread_body_browser(E)..." << std::endl;

}

void shut_down_thread( void* p ) {
	std::cout << "---> shut_down_thread(0)..." << std::endl;
	std::cout << p << std::endl;

}

void* Thread_Gui_Control::thread_enter_browser( void* p ) {
	std::cout << "---> thread_enter_browser(0)..." << std::endl;
	(( Thread_Gui_Control* ) p)->thread_body_browser();
	// Calling endthread here is probably redundant, as you are returning from the
	// function at this point, and that is supposed to clean up for you anyway.
	std::cout << "<--- thread_enter_browser(E)..." << std::endl;
	pthread_exit( 0 );

}

void* Thread_Gui_Control::thread_enter_counter( void* p ) {
	std::cout << "---> thread_enter_counter(0)..." << std::endl;
	(( Thread_Gui_Control* ) p)->thread_body_counter();
	// Calling endthread here is probably redundant, as you are returning from the
	// function at this point, and that is supposed to clean up for you anyway.
	std::cout << "<--- thread_enter_counter(E)..." << std::endl;
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
		Fl_Double_Window( xWin, yWin, wWin, hWin, l ),
		myId( ++tcntWin ) {
	ecntWin++;

	std::cout << "---> Thread_Gui_Control(0)..." << std::endl;
	std::cout << "pmyId=" << pmyId << ", pdoThread=" << pdoThread << std::endl;

	int hSpc = 10;

	int xBox = 200-55, yBox = hSpc;
	int wBox = 100, hBox = 20;
	xBox -= wBox / 2;
	int xBrw = 55, yBrw = hSpc + yBox + hBox;
	int wBrw = 200, hBrw = 175;
	int xVot = 55, yVot = hSpc + yBrw + hBrw;
	int wVot = 200, hVot = 50;

	redrawCount = new Fl_Box( xBox, yBox, wBox, hBox );

	browser = new Fl_Browser( xBrw, yBrw, wBrw, hBrw );
	valout  = new Fl_Value_Output( xVot, yVot, wVot, hVot, "Max\nCount" );

	mglgr = new mglGraph(0,300,300);

	xMglData.Create( 1, 0, 0 );
	yMglData.Create( 1, 0, 0 );

	redrawCount->label( "Pending..." );
	this->end();

	resizable( browser );
	this->resizable( this );
	this->show();

	this->callback( ( Fl_Callback* ) window_cb, this );

	this->thread_start();

	std::cout << "<--- Thread_Gui_Control(E)..." << std::endl;

}

void mgls_prepare1d( mglData* y,  mglData* x = 0 ) {
	register long i, n = 50;
	if (y) y->Create( n, 3 );
	if (x) x->Create( n );
	mreal xx;
	for ( i = 0; i < n; i++ ) {
		xx = i / (n - 1.);
		if (y) {
			y->a[i]         = 0.7 * sin( 2 * M_PI * xx ) + 0.5 * cos( 3 * M_PI * xx ) + 0.2 * sin( M_PI * xx );
			y->a[i + n]     = sin( 2 * M_PI * xx );
			y->a[i + 2 * n] = cos( 2 * M_PI * xx );
		}

		if (x) x->a[i] = xx * 2 - 1;
	}
}


int sample1( mglGraph* gr, void * p ) {
	mglData x;
	mglData y;
	mgls_prepare1d( &y , &x);

	Thread_Gui_Control* gui = ( Thread_Gui_Control* ) p;

	gr->Title( "Plot plot (default)" );
	gr->Plot( x,y, "*" );
	gr->SetRanges(x*1.1,y*1.1);
	gr->Box();
	gr->Grid();
	gr->Axis();
	gr->Label('x',"axis x",0);
	gr->Label('y',"axis y",0);

	return 0;
}

int sample( mglGraph* gr, void * p ) {
	Thread_Gui_Control* gui = ( Thread_Gui_Control* ) p;

	//mgls_prepare1d( &y,0,0,&x );

	gr->Title( "Plot plot (default)" );
	gr->Plot( gui->xMglData,gui->yMglData, "*" );
	gr->SetRanges(gui->xMglData*2,gui->yMglData*2);
	gr->Box();
	gr->Grid();
	gr->Axis();
	gr->Label('x',"axis x",0);
	gr->Label('y',"axis y",0);

	return 0;
}

int main( ) {
	/*
	 * TODO:
	 * creating multiple obj Thread_Gui_Control and closing one of them may cause exception
	 * (ie. Process finished with exit code 134 (interrupted by signal 6: SIGABRT))
	 * from child threads (both)
	 */
	Fl::lock();

	Thread_Gui_Control* tpo0 = new Thread_Gui_Control( 0, 0, 800, 600, "Threaded Gui 1" );
	//Thread_Gui_Control* tpo1 = new Thread_Gui_Control( 500, 0, 300, 300, "Threaded Gui 2" );
	int exit_status = Fl::run();
	return exit_status;
}