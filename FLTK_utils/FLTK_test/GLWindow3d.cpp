#include "GLWindow3d.h"
#include "MainWindow.h"
#include "AppStates/StateManager.h"
#include <sstream>
#include <FL/Fl.H>
#include <iostream>



void IdleCallback( void* pData );

GLWindow3d::GLWindow3d( int x, int y, int w, int h, const char* label )
		: Fl_Gl_Window( x, y, w, h, label ) {
	std::cout << "GLWindow3d::GLWindow3d( int x, int y, int w, int h, const char* label )\n";

}

GLWindow3d::~GLWindow3d( ) {
	std::cout << "GLWindow3d::~GLWindow3d( ) \n";

}


int GLWindow3d::handle( int event ) {
	std::cout << "GLWindow3d::handle(0: int event = "<<event<<" )\n";

	int r = Fl_Gl_Window::handle( event );
	std::cout << "GLWindow3d::handle(1: int event = "<<event<<" )\n";
	STATEMNGR.switch_active_state_3d();

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
	std::cout << "GLWindow3d::handle(E: int event = "<<event<<", r = "<<r<<" )\n";

	return r;

}

void GLWindow3d::init( ) {
	std::cout << "GLWindow3d::init( )\n";

	valid( 1 );
	float cx     = this->w();
	float cy     = this->h();
	float aspect = cx / cy;
	glViewport( 0, 0, cx,cy );
	STATEMNGR.switch_active_state_3d();
	BaseState* state = STATEMNGR.activeState();
	if (state != NULL) {
		state->init( &aspect );
	}
}

void GLWindow3d::draw( ) {
	std::cout << "GLWindow3d::draw( ) \n";

	if (!valid()) {
		init();
	}

	STATEMNGR.switch_active_state_3d();
	STATEMNGR.updateActiveState();
	STATEMNGR.renderActiveState();

	this->swap_buffers();
}

void GLWindow3d::resize( int x, int y, int w, int h ) {
	std::cout<<"GLWindow3d::resize( int x, int y, int w, int h )\n";
	Fl_Gl_Window::resize( x, y, w, h );
}


void IdleCallback( void* pData ) {
	std::cout<<"IdleCallback( void* pData )\n";
	if (pData != NULL) {
		GLWindow3d* glw = reinterpret_cast<GLWindow3d*>(pData);
		/*
		if (glw->animating) {
			glw->rotation += glw->rotationIncrement / 100;
			glw->redraw();
		}*/
	}
}