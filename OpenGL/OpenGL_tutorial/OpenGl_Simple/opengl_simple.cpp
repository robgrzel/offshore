#include <GL/glut.h>
#include <cmath>

using namespace std;


float sine( float f ) {
	return sin( f );

}

float cosine( float f ) {
	return cos( f );

}

float msine( float f ) {
	return -sine( f );
}


float mcosine( float f ) {
	return -cosine( f );

}

typedef float(* FunctionPointer)( float );

FunctionPointer FunctionPointers[4] = {sine, cosine, msine, mcosine};

static float i = 0;


void Render( ) {
	double w  = glutGet( GLUT_WINDOW_WIDTH );
	double h  = glutGet( GLUT_WINDOW_HEIGHT );
	double ar = w / h;
	float  y  = 0;


	//glClearColor( 0.0, 0.0, 0.0, 1.0 );  // clear background with black
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho( -2, 2, -2, 2, -1, 1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glLineWidth( 3 );
	glBegin( GL_LINES );
	glVertex2f( -1, 0 );
	glVertex2f( 1, 0 );
	glEnd();

	glLineWidth( 3 );
	glBegin( GL_LINES );
	glVertex2f( 0, -1 );
	glVertex2f( 0, 1 );
	glEnd();

	glPointSize( 10 );
	glColor3f( 0.0, 1.0, 0.0 );
	glBegin( GL_POINTS );
	glVertex2f( 0, 0 );
	glEnd();


	glPointSize( 1 );
	glColor3f( 1.0, 0.0, 1.0 );
	glBegin( GL_POINTS );
	for ( int k = 0; k < 4; k++ ) {
		for ( float j = -2; j <= 2; j += 0.01 ) {
			y = FunctionPointers[k]( j + i );
			glVertex2f( j, y );
		}
	}

	glEnd();

	glPointSize( 15 );
	glColor3f( 1.0, 0.0, 0.0 );
	glBegin( GL_POINTS );

	for ( int j = 0; j < 4; j++ ) {
		y = FunctionPointers[j]( i );
		glVertex2f( 0, y );
	}

	glEnd();

	glutSwapBuffers();

	i += 0.01;

	glutPostRedisplay();

}

int main( int argc, char** argv ) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( 640, 480 );
	glutCreateWindow( "SineWave.cpp" );
	glutDisplayFunc( Render );
	glutMainLoop();
	return 0;
}
