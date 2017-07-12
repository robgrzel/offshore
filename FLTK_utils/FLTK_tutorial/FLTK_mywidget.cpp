#include <cmath>
#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_ask.H>

#include <iostream>
#include <mgl2/mgl.h>
#include <unistd.h>
// Should probably ifdef WIN32 this, and pull in pthread.h and
// perhaps sched.h for non-Windows platforms too.
#ifdef _WIN32
	#include <process.h>
#else
#endif

#include <FL/fl_draw.H>


#include <FL/Fl_Int_Input.H>

int sample( mglGraph* gr, void * p );

class Fl_MyWidget : public Fl_Group {
	private:
		int _value;

	public:
		mglGraph * mglgr;
		mglData xMglData;
		mglData yMglData;

		// CALLBACK HANDLERS
		//    These 'attach' the input and slider's values together.
		//
		void Slider_CB2( ) {
			static int recurse = 0;
			if (recurse) {
				return;
			} else {
				recurse = 1;
				char s[80];
				//sprintf( s, "%d", ( int ) (slider->value() + .5) );
				// fprintf(stderr, "SPRINTF(%d) -> '%s'\n", (int)(slider->value()+.5), s);
				//input->value( s );          // pass slider's value to input
				recurse = 0;
			}
		}

		static void Slider_CB( Fl_Widget* w, void* data ) {
			(( Fl_MyWidget* ) data)->Slider_CB2();
		}

		void Input_CB2( ) {
			static int recurse = 0;
			if (recurse) {
				return;
			} else {
				recurse = 1;
				int val = 0;
				//if (sscanf( input->value(), "%d", &val ) != 1) {val = 0;}
				// fprintf(stderr, "SCANF('%s') -> %d\n", input->value(), val);
				//slider->value( val );         // pass input's value to slider
				recurse = 0;
			}
		}

		static void Input_CB( Fl_Widget* w, void* data ) {
			(( Fl_MyWidget* ) data)->Input_CB2();
		}

		void draw_value( int val ) {
			_value = val;
			redraw();
		}

		void draw( ) {
			Fl_Group::draw();

			mglgr->Alpha( true );         // draws something using MathGL
			mglgr->Light( true );
			sample( mglgr, this );
			fl_draw_image( mglgr->GetRGB(), x()+10, y()+10, mglgr->GetWidth(), mglgr->GetHeight(), 3 );
		}

	public:
		// CTOR
		Fl_MyWidget( int x, int y, int w, int h, const char* l = 0 ) : Fl_Group( x, y, w, h, l ), _value(0) {
			mglgr = new mglGraph(0,w-20,h-20);

			xMglData.Create( 1, 0, 0 );
			yMglData.Create( 1, 0, 0 );

			end();             // close the group
		}
};




int sample( mglGraph* gr, void * p ) {
	Fl_MyWidget* gui = ( Fl_MyWidget* ) p;

	//mgls_prepare1d( &y,0,0,&x );

	gr->Title( "Plot plot (default)" );
	gr->Plot( gui->xMglData,gui->yMglData, "*" );
	gr->SetRanges(gui->xMglData.Minimal()-0.1,gui->xMglData.Maximal()+0.1,gui->yMglData.Minimal()-0.1,gui->yMglData.Maximal()+0.1);
	gr->Box();
	gr->Grid();
	gr->Axis();
	gr->Label('x',"axis x",0);
	gr->Label('y',"axis y",0);

	return 0;
}


int main( ) {
	Fl_Window  win( 600, 400 );
	Fl_MyWidget* si = new Fl_MyWidget( 20, 20, 250, 350, "Slider Input" );
	si->box( FL_BORDER_BOX );
	win.show();
	return (Fl::run());
}


