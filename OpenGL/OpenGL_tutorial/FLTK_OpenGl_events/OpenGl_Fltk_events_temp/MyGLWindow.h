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

class MyGLWindow : public Fl_Gl_Window {
		void draw( );

		int handle( int event );

	public:
		MyGLWindow(int X, int Y, int W, int H, const char * L);

		void SaveCurrentContent();
		void RestoreContent();

		int windowWidth, windowHeight;
		int drawWidth, drawHeight;

		int mx, my;
		int isAnEvent;
		events eventToDo;

		int curColor;

		unsigned char * buf, * bitstart;

};


#endif //MARINE_AND_INTERMODAL_TRANSPORT_MYGLWINDOW_H
