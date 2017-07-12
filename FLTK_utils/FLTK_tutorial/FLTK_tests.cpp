#include <iostream>
#include <algorithm>


#include <FL/x.H> // for fl_open_callback
#include <FL/filename.H>
#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Value_Output.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

#include <pthread.h>
#include <signal.h>

#include "../Fl_Threads.h"

#define TS 14 // default editor textsize

int  loading               = 0;
int  num_windows           = 0;
int  changed               = 0;
char filename[FL_PATH_MAX] = "";
char title[FL_PATH_MAX];


Fl_Text_Buffer* textbuf  = 0;
Fl_Text_Buffer* stylebuf = 0;
Fl_Text_Display::Style_Table_Entry
		styletable[] = {    // Style table
		{FL_BLACK,      FL_COURIER,          TS}, // A - Plain
		{FL_DARK_GREEN, FL_HELVETICA_ITALIC, TS}, // B - Line comments
		{FL_DARK_GREEN, FL_HELVETICA_ITALIC, TS}, // C - Block comments
		{FL_BLUE,       FL_COURIER,          TS}, // D - Strings
		{FL_DARK_RED,   FL_COURIER,          TS}, // E - Directives
		{FL_DARK_RED,   FL_COURIER_BOLD,     TS}, // F - Types
		{FL_BLUE,       FL_COURIER_BOLD,     TS}, // G - Keywords
};


void magic_number_cb( void* p ) {
	Fl_Value_Output* w = ( Fl_Value_Output* ) p;
	w->labelcolor( FL_RED );
	w->redraw_label();
}

void* prime_func( void* p );


// width of line number display, if enabled
const int line_num_width = 75;

void save_cb( );

void saveas_cb( );

void find2_cb( Fl_Widget*, void* );

void replall_cb( Fl_Widget*, void* );

void replace2_cb( Fl_Widget*, void* );

void replcan_cb( Fl_Widget*, void* );

void new_cb( Fl_Widget*, void* );

void open_cb( Fl_Widget*, void* );

void insert_cb( Fl_Widget*, void* );

void view_cb( Fl_Widget*, void* );

void close_cb( Fl_Widget*, void* v );

void cut_cb( Fl_Widget*, void* );

void quit_cb( Fl_Widget*, void* );

void copy_cb( Fl_Widget*, void* );

void paste_cb( Fl_Widget*, void* );

void delete_cb( Fl_Widget*, void* );

void linenumbers_cb( Fl_Widget*, void* );

void wordwrap_cb( Fl_Widget*, void* );

void find_cb( Fl_Widget*, void* );

void replace_cb( Fl_Widget*, void* );

void style_unfinished_cb( int, void* ) {
};

void changed_cb( int, int nInserted, int nDeleted, int, const char*, void* v );

void style_update(
		int pos,        // I - Position of update
		int nInserted,    // I - Number of inserted chars
		int nDeleted,    // I - Number of deleted chars
		int        /*nRestyled*/,    // I - Number of restyled chars
		const char* /*deletedText*/,// I - Text that was deleted
		void* cbArg
) {
};

void style_parse(
		const char* text,
		char* style,
		int length
) {
};


void style_init( void ) {
	char* style = new char[textbuf->length() + 1];
	char* text  = textbuf->text();

	memset( style, 'A', textbuf->length() );
	style[textbuf->length()] = '\0';

	if (!stylebuf) stylebuf = new Fl_Text_Buffer( textbuf->length() );

	style_parse( text, style, textbuf->length() );

	stylebuf->text( style );
	delete[] style;
	free( text );
}


Fl_Menu_Item menuitems[] = {
		{"&File",       0, 0,                            0, FL_SUBMENU},
		{"&New File",   0, ( Fl_Callback* ) new_cb},
		{"&Open File...",    FL_COMMAND + 'o',            ( Fl_Callback* ) open_cb},
		{"&Insert File...",  FL_COMMAND + 'i',            ( Fl_Callback* ) insert_cb,      0, FL_MENU_DIVIDER},
		{"&Save File",       FL_COMMAND + 's',            ( Fl_Callback* ) save_cb},
		{"Save File &As...", FL_COMMAND + FL_SHIFT + 's', ( Fl_Callback* ) saveas_cb,      0, FL_MENU_DIVIDER},
		{"New &View",        FL_ALT
		                     #ifdef __APPLE__
		                     + FL_COMMAND
		                     #endif
		                     + 'v',                       ( Fl_Callback* ) view_cb,        0
		},
		{"&Close View",      FL_COMMAND + 'w',            ( Fl_Callback* ) close_cb,       0, FL_MENU_DIVIDER},
		{"E&xit",            FL_COMMAND + 'q',            ( Fl_Callback* ) quit_cb,        0},
		{0},

		{"&Edit",       0, 0,                            0, FL_SUBMENU},
		{"Cu&t",             FL_COMMAND + 'x',            ( Fl_Callback* ) cut_cb},
		{"&Copy",            FL_COMMAND + 'c',            ( Fl_Callback* ) copy_cb},
		{"&Paste",           FL_COMMAND + 'v',            ( Fl_Callback* ) paste_cb},
		{"&Delete",     0, ( Fl_Callback* ) delete_cb},
		{"Preferences", 0, 0,                            0, FL_SUBMENU},
		{"Line Numbers",     FL_COMMAND + 'l',            ( Fl_Callback* ) linenumbers_cb, 0, FL_MENU_TOGGLE},
		{"Word Wrap",   0, ( Fl_Callback* ) wordwrap_cb, 0, FL_MENU_TOGGLE},
		{0},
		{0},

		{"&Search",     0, 0,                            0, FL_SUBMENU},
		{"&Find...",         FL_COMMAND + 'f',            ( Fl_Callback* ) find_cb},
		{"F&ind Again",      FL_COMMAND + 'g',            find2_cb},
		{"&Replace...",      FL_COMMAND + 'r',            replace_cb},
		{"Re&place Again",   FL_COMMAND + 't',            replace2_cb},
		{0},

		{0}
};


class EditorWindow : public Fl_Double_Window {
	public:
		int create_thread( void* (* func)( void* ) );

	public:
		EditorWindow( int w, int h, const char* t );

		~EditorWindow( );

		Fl_Window       * replace_dlg;
		Fl_Input        * replace_find;
		Fl_Input        * replace_with;
		Fl_Button       * replace_all;
		Fl_Return_Button* replace_next;
		Fl_Button       * replace_cancel;

		Fl_Text_Editor * editor;
		Fl_Value_Output* valout;
		Fl_Browser     * browser;

		pthread_t thread;
		bool doThread = true;

		int wrap_mode;
		int line_numbers;


		char search[256];
};

EditorWindow::EditorWindow( int w, int h, const char* t ) : Fl_Double_Window( w, h, t ) {
	replace_dlg = new Fl_Window( 300, 105, "Replace" );

	replace_find = new Fl_Input( 80, 10, 210, 25, "Find:" );
	replace_find->align( FL_ALIGN_LEFT );

	replace_with = new Fl_Input( 80, 40, 210, 25, "Replace:" );
	replace_with->align( FL_ALIGN_LEFT );

	replace_all = new Fl_Button( 10, 70, 90, 25, "Replace All" );
	replace_all->callback( ( Fl_Callback* ) replall_cb, this );

	replace_next = new Fl_Return_Button( 105, 70, 120, 25, "Replace Next" );
	replace_next->callback( ( Fl_Callback* ) replace2_cb, this );

	replace_cancel = new Fl_Button( 230, 70, 60, 25, "Cancel" );
	replace_cancel->callback( ( Fl_Callback* ) replcan_cb, this );
	replace_dlg->end();
	replace_dlg->set_non_modal();

	editor  = 0;
	browser = 0;
	valout  = 0;


	*search = ( char ) 0;
	wrap_mode    = 0;
	line_numbers = 0;
}

EditorWindow::~EditorWindow( ) {
	delete replace_dlg;
}

int EditorWindow::create_thread( void* (* func)( void* ) ) {
	Fl::unlock();
	Fl::lock();

	// Start threads...

	// One thread displaying in one browser
	fl_create_thread( thread, func, this );
	fl_create_thread( thread, func, this );
	fl_create_thread( thread, func, this );
	fl_create_thread( thread, func, this );

	// Several threads displaying in another browser
	//return Fl::run();
}

int check_save( void ) {
	if (!changed) return 1;

	int r = fl_choice( "The current file has not been saved.\n"
			                   "Would you like to save it now?",
	                   "Cancel", "Save", "Don't Save"
	                 );

	if (r == 1) {
		save_cb(); // Save the file...
		return !changed;
	}

	return (r == 2) ? 1 : 0;
}

void load_file( const char* newfile, int ipos ) {
	loading    = 1;
	int insert = (ipos != -1);
	changed = insert;
	if (!insert) strcpy( filename, "" );
	int r;
	if (!insert) { r = textbuf->loadfile( newfile ); }
	else { r = textbuf->insertfile( newfile, ipos ); }
	changed = changed || textbuf->input_file_was_transcoded;
	if (r) {
		fl_alert( "Error reading from file \'%s\':\n%s.", newfile, strerror( errno ) );
	} else if (!insert) strcpy( filename, newfile );
	loading = 0;
	textbuf->call_modify_callbacks();
}

void save_file( const char* newfile ) {
	if (textbuf->savefile( newfile )) {
		fl_alert( "Error writing to file \'%s\':\n%s.", newfile, strerror( errno ) );
	} else {
		strcpy( filename, newfile );
	}
	changed = 0;
	textbuf->call_modify_callbacks();
}

void copy_cb( Fl_Widget*, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	Fl_Text_Editor::kf_copy( 0, e->editor );
}

void cut_cb( Fl_Widget*, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	Fl_Text_Editor::kf_cut( 0, e->editor );
}

void delete_cb( Fl_Widget*, void* ) {
	textbuf->remove_selection();
}

void linenumbers_cb( Fl_Widget* w, void* v ) {
	EditorWindow      * e = ( EditorWindow* ) v;
	Fl_Menu_Bar       * m = ( Fl_Menu_Bar* ) w;
	const Fl_Menu_Item* i = m->mvalue();
	if (i->value()) {
		e->editor->linenumber_width( line_num_width );    // enable
		e->editor->linenumber_size( e->editor->textsize() );
	} else {
		e->editor->linenumber_width( 0 );    // disable
	}
	e->line_numbers = (i->value() ? 1 : 0);
	e->redraw();
}

void wordwrap_cb( Fl_Widget* w, void* v ) {
	EditorWindow      * e = ( EditorWindow* ) v;
	Fl_Menu_Bar       * m = ( Fl_Menu_Bar* ) w;
	const Fl_Menu_Item* i = m->mvalue();
	if (i->value()) {
		e->editor->wrap_mode( Fl_Text_Display::WRAP_AT_BOUNDS, 0 );
	} else {
		e->editor->wrap_mode( Fl_Text_Display::WRAP_NONE, 0 );
	}
	e->wrap_mode = (i->value() ? 1 : 0);
	e->redraw();
}

void find_cb( Fl_Widget* w, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	const char  * val;

	val = fl_input( "Search String:", e->search );
	if (val != NULL) {
		// User entered a string - go find it!
		strcpy( e->search, val );
		find2_cb( w, v );
	}
}

void find2_cb( Fl_Widget* w, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	if (e->search[0] == '\0') {
		// Search string is blank; get a new one...
		find_cb( w, v );
		return;
	}

	int pos   = e->editor->insert_position();
	int found = textbuf->search_forward( pos, e->search, &pos );
	if (found) {
		// Found a match; select and update the position...
		textbuf->select( pos, pos + strlen( e->search ) );
		e->editor->insert_position( pos + strlen( e->search ) );
		e->editor->show_insert_position();
	} else { fl_alert( "No occurrences of \'%s\' found!", e->search ); }
}

void set_title( Fl_Window* w ) {
	if (filename[0] == '\0') { strcpy( title, "Untitled" ); }
	else {
		char* slash;
		slash = strrchr( filename, '/' );
		#ifdef WIN32
		if (slash == NULL) slash = strrchr(filename, '\\');
		#endif
		if (slash != NULL) { strcpy( title, slash + 1 ); }
		else { strcpy( title, filename ); }
	}

	if (changed) strcat( title, " (modified)" );

	w->label( title );
}

void changed_cb( int, int nInserted, int nDeleted, int, const char*, void* v ) {
	std::cout << "changed_cb()..." << std::endl;
	if ((nInserted || nDeleted) && !loading) changed = 1;
	EditorWindow* w = ( EditorWindow* ) v;
	set_title( w );
	if (loading) w->editor->show_insert_position();
}

void new_cb( Fl_Widget*, void* ) {
	if (!check_save()) return;

	filename[0] = '\0';
	textbuf->select( 0, textbuf->length() );
	textbuf->remove_selection();
	changed = 0;
	textbuf->call_modify_callbacks();
}

void open_cb( Fl_Widget*, void* ) {
	if (!check_save()) return;
	Fl_Native_File_Chooser fnfc;
	fnfc.title( "Open file" );
	fnfc.type( Fl_Native_File_Chooser::BROWSE_FILE );
	if (fnfc.show()) return;
	load_file( fnfc.filename(), -1 );

}

void insert_cb( Fl_Widget*, void* v ) {
	Fl_Native_File_Chooser fnfc;
	fnfc.title( "Insert file" );
	fnfc.type( Fl_Native_File_Chooser::BROWSE_FILE );
	if (fnfc.show()) return;
	EditorWindow* w = ( EditorWindow* ) v;
	load_file( fnfc.filename(), w->editor->insert_position() );
}

void paste_cb( Fl_Widget*, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	Fl_Text_Editor::kf_paste( 0, e->editor );
}


void callMeOnExit( ) {
	printf( "Exiting Thread!\n" );
}




void close_cb( Fl_Widget*, void* v ) {
	std::cout << "QUIT(0)..." << std::endl;

	EditorWindow* w = ( EditorWindow* ) v;

	if (num_windows == 1) {
		if (!check_save()) {
			return;
		}
	}
	std::cout << "QUIT(1)..." << std::endl;

	w->doThread = false;
	w->hide();
	w->editor->buffer( 0 );
	textbuf->remove_modify_callback( style_update, w->editor );
	textbuf->remove_modify_callback( changed_cb, w );
	std::cout << "QUIT(2)..." << std::endl;
	Fl::delete_widget( w );
	std::cout << "QUIT(3)..." << std::endl;

	num_windows--;
	if (!num_windows) {
		std::cout << "QUIT(4): exit(0)" << std::endl;

		exit( 0 );
	} else {
		Fl::lock();
		std::cout << "QUIT(5): else" << std::endl;
		return;
	};
}


void quit_cb( Fl_Widget*, void* ) {
	std::cout << "QUITCB(0)..." << std::endl;
	if (changed && !check_save()) {
		return;
	}

	exit( 0 );
}

void replace_cb( Fl_Widget*, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	e->replace_dlg->show();
}

void replace2_cb( Fl_Widget*, void* v ) {
	EditorWindow* e       = ( EditorWindow* ) v;
	const char  * find    = e->replace_find->value();
	const char  * replace = e->replace_with->value();

	if (find[0] == '\0') {
		// Search string is blank; get a new one...
		e->replace_dlg->show();
		return;
	}

	e->replace_dlg->hide();

	int pos   = e->editor->insert_position();
	int found = textbuf->search_forward( pos, find, &pos );

	if (found) {
		// Found a match; update the position and replace text...
		textbuf->select( pos, pos + strlen( find ) );
		textbuf->remove_selection();
		textbuf->insert( pos, replace );
		textbuf->select( pos, pos + strlen( replace ) );
		e->editor->insert_position( pos + strlen( replace ) );
		e->editor->show_insert_position();
	} else { fl_alert( "No occurrences of \'%s\' found!", find ); }
}

void replall_cb( Fl_Widget*, void* v ) {
	EditorWindow* e       = ( EditorWindow* ) v;
	const char  * find    = e->replace_find->value();
	const char  * replace = e->replace_with->value();

	find = e->replace_find->value();
	if (find[0] == '\0') {
		// Search string is blank; get a new one...
		e->replace_dlg->show();
		return;
	}

	e->replace_dlg->hide();

	e->editor->insert_position( 0 );
	int times = 0;

	// Loop through the whole string
	for ( int found = 1; found; ) {
		int pos = e->editor->insert_position();
		found = textbuf->search_forward( pos, find, &pos );

		if (found) {
			// Found a match; update the position and replace text...
			textbuf->select( pos, pos + strlen( find ) );
			textbuf->remove_selection();
			textbuf->insert( pos, replace );
			e->editor->insert_position( pos + strlen( replace ) );
			e->editor->show_insert_position();
			times++;
		}
	}

	if (times) { fl_message( "Replaced %d occurrences.", times ); }
	else { fl_alert( "No occurrences of \'%s\' found!", find ); }
}

void replcan_cb( Fl_Widget*, void* v ) {
	EditorWindow* e = ( EditorWindow* ) v;
	e->replace_dlg->hide();
}

void save_cb( ) {
	if (filename[0] == '\0') {
		// No filename - get one!
		saveas_cb();
		return;
	} else { save_file( filename ); }
}

void saveas_cb( ) {
	Fl_Native_File_Chooser fnfc;
	fnfc.title( "Save File As?" );
	fnfc.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
	if (fnfc.show()) return;
	save_file( fnfc.filename() );
}

//Fl_Window* new_view( );


Fl_Window* new_view( ) {
	int maxW  = 660, maxH = 400;
	int menuX = 0, menuY = 0;
	int menuW = maxW, menuH = 30;
	int browX = 0, browY = 30;
	int browW = 200, browH = 200;
	int valwX = 30, valwY = browY + browH;
	int valwW = 200 - valwX, valwH = 30;
	int txteX = valwX + valwW, txteY = menuH;
	int txteW = maxW - txteX, txteH = maxH - menuH;


	txteH -= menuH;

	EditorWindow* w = new EditorWindow( maxW, maxH, title );
	w->begin();

	Fl_Menu_Bar* m = new Fl_Menu_Bar( menuX, menuY, menuW, menuH );
	m->copy( menuitems, w );

	w->browser = new Fl_Browser( browX, browY, browW, browH );
	w->browser->add( "Prime numbers (1):" );

	w->valout = new Fl_Value_Output( valwX, valwY, valwW, valwH, "n=" );

	w->editor = new Fl_Text_Editor( txteX, txteY, txteW, txteH );

	w->editor->textfont( FL_COURIER );
	w->editor->textsize( TS );
	w->editor->wrap_mode( Fl_Text_Editor::WRAP_AT_BOUNDS, 250 );
	w->editor->buffer( textbuf );
	w->editor->highlight_data( stylebuf, styletable,
	                           sizeof( styletable ) / sizeof( styletable[0] ),
	                           'A', style_unfinished_cb, 0
	                         );


	w->end();
	w->resizable( w->valout );
	w->resizable( w->browser );
	w->resizable( w->editor );
	w->size_range( 300, 200 );
	w->callback( ( Fl_Callback* ) close_cb, w );

	textbuf->add_modify_callback( style_update, w->editor );
	textbuf->add_modify_callback( changed_cb, w );
	textbuf->call_modify_callbacks();


	//pthread_create( &t, 0, prime_func, w );
	Fl::unlock();
	Fl::lock();
	w->doThread=true;
	//fl_create_thread( thread, prime_func, w );
	//w->create_thread( prime_func );

	num_windows++;

	return w;
}

void view_cb( Fl_Widget*, void* ) {
	std::cout<<"view_cb()..."<<std::endl;
	Fl_Window* w = new_view();
	w->show();
}


void cb( const char* fname ) {
	load_file( fname, -1 );
}


int numThreads = 0;

void* prime_func( void* p ) {
	EditorWindow   * window  = ( EditorWindow* ) p;
	Fl_Browser     * browser = window->browser;
	Fl_Value_Output* value   = window->valout;
	numThreads++;

	std::cout << "prime_func(0): numThreads= "<< numThreads << std::endl;
	int  n     = 3 + numThreads;
	int  step  = 2;
	char proud = 0;



	// very simple prime number calculator !
	//
	// The return at the end of this function can never be reached and thus
	// will generate a warning with some compilers, however we need to have
	// a return statement or other compilers will complain there is no return
	// statement. To avoid warnings on all compilers, we fool the smart ones
	// into beleiving that there is a chance that we reach the end by testing
	// n>=0, knowing that logically, n will never be negative in this context.
	if (n >= 0) {
		while ( window->doThread ) {

			//sleep( 1 );
			int pp;
			int hn = ( int ) sqrt( ( double ) n );

			for ( pp = 3; pp <= hn; pp += 2 ) if (n % pp == 0) break;

			if (pp >= hn) {
				char s[128];
				sprintf( s, "%d", n );

				// Obtain a lock before we access the browser widget...
				Fl::lock();

				browser->add( s );
				browser->bottomline( browser->size() );
				if (n > value->value()) value->value( n );
				n += step;

				// Release the lock...
				Fl::unlock();

				// Send a message to the main thread, at which point it will
				// process any pending redraws for our browser widget.  The
				// message we pass here isn't used for anything, so we could also
				// just pass NULL.
				Fl::awake( p );
				if (n > 10000 && !proud) {
					proud = 1;
					Fl::awake( magic_number_cb, value );
				}
			} else {
				// This should not be necessary since "n" and "step" are local variables,
				// however it appears that at least MacOS X has some threading issues
				// that cause semi-random corruption of the (stack) variables.
				Fl::lock();
				n += step;
				Fl::unlock();
			}
		}
	}
	std::cout<<"finish prime_func"<<std::endl;
	return 0L;
}


int FLTK_Editor( int argc, char** argv ) {
	textbuf = new Fl_Text_Buffer;
	//textbuf->transcoding_warning_action = NULL;
	style_init();
	fl_open_callback( cb );

	std::cout<<"new_view(0)..."<<std::endl;

	Fl_Window* window = new_view();
	std::cout<<"new_view(1)..."<<std::endl;

	window->show( 1, argv );
	#ifndef __APPLE__
	if (argc > 1) load_file( argv[1], -1 );
	#endif

	return Fl::run();
}

int main( int argc, char** argv ) {
	FLTK_Editor( argc, argv );
}
