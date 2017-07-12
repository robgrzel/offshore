
#include "GLWindow2d.h"
#include "MainWindow.h"
#include "AppStates/StateManager.h"
#include <FL/Fl.H>
#include <iostream>


GLWindow2d::GLWindow2d( int x, int y, int w, int h, const char* label )
		: Fl_Gl_Window( x, y, w, h, label ) {
	std::cout<<"GLWindow2d::GLWindow2d( int x, int y, int w, int h, const char* label )\n";

}

GLWindow2d::~GLWindow2d( ) {
	std::cout<<"GLWindow2d::~GLWindow2d( )\n";

}


int GLWindow2d::handle( int event ) {
	std::cout<<"GLWindow2d::handle( int event ) \n";
	int r = Fl_Gl_Window::handle( event );

	STATEMNGR.switch_active_state_2d();

	switch ( event ) {
		case FL_ENTER: r = 1;
			break;
		case FL_PUSH: {
			r = 1;
			UIEvent event( UIEvent::MOUSE_DOWN );
			event.rbutton( 0 ) = Fl::event_button1();
			event.rbutton( 1 ) = Fl::event_button2();
			event.rbutton( 2 ) = Fl::event_button3();
			event.rx()         = Fl::event_x();
			event.ry()         = Fl::event_y();
			STATEMNGR.sendEvent( &event );
		}
			break;
		case FL_RELEASE: {
			r = 1;
			UIEvent event( UIEvent::MOUSE_UP );
			event.rbutton( 0 ) = Fl::event_button1();
			event.rbutton( 1 ) = Fl::event_button2();
			event.rbutton( 2 ) = Fl::event_button3();
			event.rx()         = Fl::event_x();
			event.ry()         = Fl::event_y();
			STATEMNGR.sendEvent( &event );
		}
			break;
		case FL_DRAG: {
			r = 1;
			UIEvent event( UIEvent::MOUSE_MOVE );
			event.rbutton( 0 ) = Fl::event_button1();
			event.rbutton( 1 ) = Fl::event_button2();
			event.rbutton( 2 ) = Fl::event_button3();
			event.rx()         = Fl::event_x();
			event.ry()         = Fl::event_y();
			STATEMNGR.sendEvent( &event );

			redraw();
		}
			break;
		case FL_MOUSEWHEEL: {
			r = 1;
			UIEvent event( UIEvent::MOUSE_WHEEL );
			event.rbutton( 0 ) = Fl::event_button1();
			event.rbutton( 1 ) = Fl::event_button2();
			event.rbutton( 2 ) = Fl::event_button3();
			event.rx()         = Fl::event_dx();
			event.ry()         = Fl::event_dy();
			STATEMNGR.sendEvent( &event );

			redraw();
		}
		default:

			break;
	}

	return r;
}

void GLWindow2d::init( ) {
	std::cout<<"GLWindow2d::init( \n";
	valid( 1 );
	float cx     = this->w();
	float cy     = this->h();
	float aspect = cx / cy;
	glViewport( 0, 0, cx, cy );
	STATEMNGR.switch_active_state_2d();
	BaseState* state = STATEMNGR.activeState();
	if (state != NULL) {
		state->init( &aspect );
	}
}

void GLWindow2d::draw( ) {
	std::cout<<"GLWindow2d::draw( )\n";
	if (!valid()) {
		init();
	}
	STATEMNGR.switch_active_state_2d();

	STATEMNGR.updateActiveState();
	STATEMNGR.renderActiveState();

	this->swap_buffers();
}

void GLWindow2d::resize( int x, int y, int w, int h ) {
	std::cout<<"GLWindow2d::resize( int x, int y, int w, int h )\n";
	Fl_Gl_Window::resize( x, y, w, h );
}


void GLWindow2d::IdleCallback( void* pData ) {
	std::cout<<"GLWindow2d::IdleCallback( void* pData )\n";
	if (pData != NULL) {
		GLWindow2d* glw = reinterpret_cast<GLWindow2d*>(pData);
		/*
		if (glw->animating) {
			glw->rotation += glw->rotationIncrement / 100;
			glw->redraw();
		}*/
	}
}