//
// Created by robgrzel on 24.04.17.
//

#include "MyGLWindow.h"


#define MIN( a, b ) ( ((a)<(b)) ? (a): (b))

static GLubyte drawColor[3][3] = {{255, 0,   0},
                                  {0,   255, 0},
                                  {0,   0,   255}
};


MyGLWindow::MyGLWindow( int X, int Y, int W, int H, const char* L ) :
		Fl_Gl_Window( X, Y, W, H, L ) {
	std::cout << "MyGLWindow::MyGLWindow(0)" << std::endl;

	windowWidth  = W;
	windowHeight = H;

	buf = new unsigned char[W * H * 3];
	memset( buf, 0, W * H * 3 );

	curColor = 0;

	std::cout << "MyGLWindow::MyGLWindow(E)" << std::endl;
}



void MyGLWindow::draw_event( ) {
	std::cout << "MyGLWindow::draw(3)\n";
	isAnEvent = 0;

	double x[4], y[4];

	std::cout << "mx=" << mx << std::endl;
	std::cout << "windowWidth=" << windowWidth << std::endl;
	switch ( eventToDo ) {
		case events::LEFT_MOUSE_DOWN : {
			glColor3ubv( drawColor[curColor] );
			glBegin( GL_QUADS );


			x[0] = x[1] = mx;
			x[2] = x[3] = mx+0.1;
			y[0] = y[3] = my;
			y[1] = y[2] = my+0.1;


			printf("mx=%f, my=%f\n",mx,my);
			for ( int i = 0; i < 4; i++ ) {
				printf( "x[%d]=%f, y[%d] = %f\n", i, x[i], i, y[i] );
				glVertex2d( x[i], y[i] );
			}
			glEnd();

			break;
		}
		case events::RIGHT_MOUSE_DOWN : {
			curColor = (curColor + 1) % 3;
			break;

		}
	}
}

void MyGLWindow::draw_2d( ) {
	windowWidth  = w();
	windowHeight = h();

	std::cout << "MyGLWindow::draw(1)\n";
	glClearColor( 0.7f, 0.7f, 0.7f, 1.0f );
	glDisable( GL_DEPTH_TEST );
	//ortho();
	glOrtho( -1, 1, -1, 1, 0, 1 );

	glClear( GL_COLOR_BUFFER_BIT );

	//glViewport (0, 0, windowWidth, windowHeight) ;
	//glMatrixMode ( GL_PROJECTION ) ;//control camera
	//glMatrixMode ( GL_MODELVIEW ) ;//control drawing
	//glLoadIdentity(); //clear currently modifiable matrix for future transformation

	glLineWidth(10);
	glBegin( GL_LINES );
	glVertex2d( -0.1, 0 );
	glVertex2d( 0.1, 0 );
	glVertex2d( 0, -0.1 );
	glVertex2d( 0, 0.1 );
	glEnd();

}

void MyGLWindow::draw( ) {
	std::cout << "MyGLWindow::draw(0)\n";
	if (!valid()) {
		draw_2d();
	};

	if (!isAnEvent) {
		std::cout << "MyGLWindow::draw(2)\n";
	} else {
		draw_event();
	}

	std::cout << "MyGLWindow::draw(E)\n";
}

int MyGLWindow::handle( int event ) {
	switch ( event ) {
		case FL_PUSH: {
			mx        = norm_coordinate((float)Fl::event_x(),windowWidth,-1,1);
			my        = -norm_coordinate((float)Fl::event_y(),windowHeight,-1,1);
			if (Fl::event_button() > 1) {
				eventToDo = events::RIGHT_MOUSE_DOWN;
			} else {
				eventToDo = events::LEFT_MOUSE_DOWN;
			}
			isAnEvent = 1;
			redraw();
			return 1;
		}
		default:

			break;
	};
	return 0;
}

