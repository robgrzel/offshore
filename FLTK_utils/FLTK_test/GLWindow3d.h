#pragma once
#ifdef _WIN32
#include <windows.h>
#endif

#include <FL/Fl_Gl_Window.H>

//#include <GL/GL.h>
#include <FL/gl.h>

class GLWindow3d : public Fl_Gl_Window
{
public:
	GLWindow3d(int x, int y, int w, int h, const char *label = 0);
	~GLWindow3d();

protected:
	void init();
	void draw();
	int handle(int event);
	void resize(int x, int y, int w, int h);


private:

};

