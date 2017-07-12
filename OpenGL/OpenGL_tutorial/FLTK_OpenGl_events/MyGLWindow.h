//
// Created by robgrzel on 24.04.17.
//

#ifndef MARINE_AND_INTERMODAL_TRANSPORT_MYGLWINDOW_H
#define MARINE_AND_INTERMODAL_TRANSPORT_MYGLWINDOW_H

#include <GL/glew.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <iostream>

enum events {
	LEFT_MOUSE_DOWN,
	RIGHT_MOUSE_DOWN,
};

inline float norm_coordinate( float x, float l, float a, float b ) {
	float c = a + (b - a) * x / l;

	if (c < a) {
		c = a;

	} else if (c > b) {
		c = b;
	}

	return c;
};

inline float* norm_coordinates( float x, float y, float w, float h, float a0, float a1, float b0, float b1 ) {
	float n[2] = {
			norm_coordinate( x, w, a0, a1 ),
			norm_coordinate( y, h, b0, b1 ),
	};

	return n;
}

class MyGLWindow : public Fl_Gl_Window {
		void draw( );

		void draw_2d( );

		void draw_event( );

		int handle( int event );

	public:
		MyGLWindow( int X, int Y, int W, int H, const char* L );

		unsigned char* buf, * bitstart;

		float mx, my;
		int   windowWidth, windowHeight;
		int   isAnEvent;
		int   curColor;

		events eventToDo;

};


#endif //MARINE_AND_INTERMODAL_TRANSPORT_MYGLWINDOW_H
