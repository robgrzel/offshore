#pragma once
#ifdef _WIN32
	#include <windows.h>
#endif

#include <FL/Fl_Gl_Window.H>

#include <FL/gl.h>

class GLWindow2d : public Fl_Gl_Window {
	public:
		GLWindow2d( int x, int y, int w, int h, const char* label = 0 );

		~GLWindow2d( );

	protected:
		void init( );

		void draw( );

		int handle( int event );

		void resize( int x, int y, int w, int h );

		void IdleCallback( void* pData );

	private:

};

