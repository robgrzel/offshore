//
// Created by robgrzel on 17.04.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#  include <FL/Fl_Double_Window.H>
#  include <FL/Fl_Browser.H>
#  include <FL/Fl_Value_Output.H>
#  include <FL/fl_ask.H>
#include <unistd.h>

//
// Demonstrate fltk timers continue to run while menu items navigated and picked
// even when Fl::wait() is used in place of Fl::run()
//

// Timer callback updates label
void Update_CB( void* userdata ) {
	Fl_Box * box = ( Fl_Box* ) userdata;
	time_t lt    = time( NULL );
	box->label( ctime( &lt ) );
	Fl::repeat_timeout( 1.0, Update_CB, ( void* ) box );
}

// Menu callback prints which option was chosen
void Menu_CB( Fl_Widget*, void* userdata ) {
	fprintf( stderr, "menu_callback(%d)...\n", ( int* ) userdata );
	if (userdata == 0) exit( 0 );
}


int main( void ) {
	Fl_Window   win( 400, 100 );
	Fl_Menu_Bar menubar( 0, 0, 400, 25 );
	Fl_Box      box( 0, 25, 400, 100 - 25 );

	menubar.add( "File/Quit", 0, Menu_CB, ( void* ) 0 );
	menubar.add( "Stuff/Do something 1", 0, Menu_CB, ( void* ) 1 );
	menubar.add( "Stuff/Do something 2", 0, Menu_CB, ( void* ) 2 );


	Fl::add_timeout( 1.0, Update_CB, ( void* ) &box );
	win.show();

	// EVENT LOOP
	while ( win.visible() ) {
		static int count = 0;
		Fl::wait( 0.5 );
		count++;
	}
	return (0);
}