//
// Created by robgrzel on 24.04.17.
//

#include "MyGLWindow.h"


#define MIN( a, b ) ( ((a)<(b)) ? (a): (b))


MyGLWindow::MyGLWindow( int X, int Y, int W, int H, const char* L ) :
		Fl_Gl_Window( X, Y, W, H, L ) {
	std::cout << "MyGLWindow::MyGLWindow(0)" << std::endl;

	windowWidth  = drawWidth  = W;
	windowHeight = drawHeight = H;

	buf = new unsigned char[W * H * 3];
	memset( buf, 0, W * H * 3 );

	curColor = 0;

	std::cout << "MyGLWindow::MyGLWindow(E)" << std::endl;
}

static GLubyte drawColor[3][3] = {{255, 0,   0},
                                  {0,   255, 0},
                                  {0,   0,   255}
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

void MyGLWindow::draw( ) {
	std::cout << "MyGLWindow::draw(0)\n";
	if (!valid()) {

		windowWidth  = w();
		windowHeight = h();

		std::cout << "MyGLWindow::draw(1)\n";
		glClearColor( 0.7f, 0.7f, 0.7f, 1.0f );
		glDisable( GL_DEPTH_TEST );
		ortho();

		glClear( GL_COLOR_BUFFER_BIT );

		//glViewport (0, 0, windowWidth, windowHeight) ;
		//glMatrixMode ( GL_PROJECTION ) ;//control camera
		//glMatrixMode ( GL_MODELVIEW ) ;//control drawing
		//glLoadIdentity(); //clear currently modifiable matrix for future transformation

		//glOrtho(-1,1,-1,1,-1,1);




		int startrow = drawHeight - MIN( drawHeight, windowHeight );
		if (startrow < 0) startrow = 0;

		bitstart = buf + 3 * ((drawWidth * startrow));



		//glLineWidth(10);
		//glBegin( GL_LINES );
		//glVertex2d( -0.1*windowWidth, 0 );
		//glVertex2d( 0.1*windowWidth, 0 );
		//glVertex2d( 0, -0.1*windowHeight );
		//glVertex2d( 0, 0.1*windowHeight );
		//glEnd();

		//SaveCurrentContent();
		RestoreContent();

	}

	if (!isAnEvent) {
		std::cout << "MyGLWindow::draw(2)\n";
		RestoreContent();
	} else {
		std::cout << "MyGLWindow::draw(3)\n";
		isAnEvent = 0;

		double x[4], y[4];

		std::cout<<"mx="<<mx<<std::endl;
		std::cout<<"windowWidth="<<windowWidth<<std::endl;
		switch ( eventToDo ) {
			case events::LEFT_MOUSE_DOWN : {
				glColor3ubv( drawColor[curColor] );
				glBegin( GL_QUADS );


				x[0] = x[1] = mx;
				x[2] = x[3] = mx + 20;
				y[0] = y[3] = (windowHeight - my);
				y[1] = y[2] = (windowHeight - my) - 20;

				for (int i = 0; i<4; i++) {
					printf( "x[%d]=%f, y[%d] = %f\n", i, x[i], i, y[i] );
					glVertex2d( x[i], y[i] );
				}
				glEnd();

				SaveCurrentContent();
				break;
			}
			case events::RIGHT_MOUSE_DOWN : {
				curColor = (curColor + 1) % 3;
				break;

			}
		}
	}
	std::cout << "MyGLWindow::draw(E)\n";

}

int MyGLWindow::handle( int event ) {
	switch ( event ) {
		case FL_PUSH: mx = Fl::event_x();
			my        = Fl::event_y();
			if (Fl::event_button() > 1) {
				eventToDo = events::RIGHT_MOUSE_DOWN;
			} else {
				eventToDo = events::LEFT_MOUSE_DOWN;
			}
			isAnEvent = 1;
			redraw();
			return 1;

		default:

			break;
	};
	return 0;
}

void MyGLWindow::SaveCurrentContent( ) {
	std::cout << "MyGLWindow::SaveCurrentContent(0)\n";

	glReadBuffer( GL_BACK );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, drawWidth );


	glReadPixels( 0,
	              windowHeight - MIN( drawHeight, windowHeight ),
	              MIN( drawWidth, windowWidth ),
	              MIN( drawHeight, windowHeight ),
	              GL_RGB, GL_UNSIGNED_BYTE, bitstart
	            );


	std::cout << "MyGLWindow::SaveCurrentContent(E)\n";
}

void MyGLWindow::RestoreContent( ) {
	std::cout << "MyGLWindow::RestoreContent(0)\n";
	glDrawBuffer( GL_BACK );
	glRasterPos2i( 0, windowHeight - MIN( drawHeight, windowHeight ) );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, drawWidth );
	glDrawPixels( MIN( drawWidth, windowWidth ),
	              MIN( drawHeight, windowHeight ),
	              GL_RGB, GL_UNSIGNED_BYTE, bitstart
	            );
	std::cout << "MyGLWindow::RestoreContent(E)\n";
}