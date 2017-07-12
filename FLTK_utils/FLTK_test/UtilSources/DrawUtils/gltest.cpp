#include "gltest.h"


//cube
const GLfloat l              = 30.0f;
const GLfloat d              = -l / sqrtf( 3.f * l * l );
GLfloat       cube[8 * 10]   = {
		-l, l, l, 0.0, 1.0, 1.0, 1.0, -d, d, d * 0,//0
		l, l, l, 1.0, 1.0, 1.0, 1.0, d, d, d * 0,//1
		l, l, -l, 1.0, 1.0, 0.0, 1.0, d, d, -d * 0,//2
		-l, l, -l, 0.0, 1.0, 0.0, 1.0, -d, d, -d * 0,//3

		-l, -l, l, 0.0, 0.0, 1.0, 1.0, -d, -d, d * 0,//4
		l, -l, l, 1.0, 0.0, 1.0, 1.0, d, -d, d * 0,//5
		l, -l, -l, 1.0, 0.0, 0.0, 1.0, d, -d, -d * 0,//6
		-l, -l, -l, 0.0, 0.0, 0.0, 1.0, -d, -d, -d * 0 //7
};
GLuint        indices[6 * 4] = {
		0, 1, 2, 3,
		0, 3, 7, 4,
		0, 4, 5, 1,
		1, 5, 6, 2,
		2, 6, 7, 3,
		4, 5, 6, 7
};

void draw_cube( ) {
	std::cout << "draw_cube\n";

	glVertexPointer( 3, GL_FLOAT, 10 * 4, cube );
	glColorPointer( 4, GL_FLOAT, 10 * 4, ( void* ) (cube + 3) );
	glNormalPointer( GL_FLOAT, 10 * 4, ( void* ) (cube + 7) );
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, indices );
}

void drawin_object(void (*func)(void), float xt, float yt, float zt, float xr, float yr, float zr){
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnable( GL_NORMALIZE );
	glPushMatrix();

	glTranslatef(xt,yt,zt);
	glRotatef(xr,1,0,0);
	glRotatef(yr,0,1,0);
	glRotatef(zr,0,0,1);
	func();
	glPopMatrix();

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisable( GL_NORMALIZE );
}

void drawin_cube(float xt, float yt, float zt, float xr, float yr, float zr){
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glPushMatrix();

	glTranslatef(xt,yt,zt);
	glRotatef(xr,1,0,0);
	glRotatef(yr,0,1,0);
	glRotatef(zr,0,0,1);
	draw_cube();
	glPopMatrix();

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void draw_scal_axis( float radius, float length ) {
	std::cout << "draw_scal_axis\n";
	const int   N   = 16;
	const float ang = 2.0 * M_PI / N;
	const float h   = length / 20.0;
	{
		glBegin( GL_QUAD_STRIP );
		for ( int i = 0; i <= N; i++ ) {
			float x  = radius * cosf( ang * i );
			float y  = radius * sinf( ang * i );
			float sq = sqrt( x * x + y * y );
			glNormal3f( x / sq, y / sq, 0 );
			glVertex3f( x, y, length / 2.0 - h );
			glVertex3f( x, y, -length / 2 );

		}
		glEnd();
	}
	{
		glBegin( GL_TRIANGLE_FAN );
		glNormal3f( 0, 0, 1 );
		glVertex3f( 0, 0, length / 2 );
		for ( int i = 0; i <= N; i++ ) {
			float x  = 2.0 * radius * cosf( ang * i );
			float y  = 2.0 * radius * sinf( ang * i );
			float sq = sqrt( x * x + y * y );
			glNormal3f( x / sq, y / sq, 0 );
			glVertex3f( x, y, length / 2 - h );

		}
		glEnd();
	}
}



//

void draw_vec_axis( OVector3 origin, OVector3 direction, float length, bool arrow, float radius_f ) {
	std::cout << "draw_axis_3d( OVector3 origin, OVector3 direction, float length, bool arrow, float radius_f )\n";
	direction.normalize();

	const float radius = radius_f * length;
	const int   N      = 16;
	const float ang    = 2.0f * ( float ) M_PI / N;
	const float h      = arrow ? length / 20.0f : 0;

	OVector3 middle_point = origin + (length / 2.0f) * direction;
	OVector3 z_axis( 0.0f, 0.0f, 1.0f );

	OQuaternion q_orientation;
	q_orientation.shortest_rotation( z_axis, direction );
	OMatrix4 matrix_orientation = q_orientation.ToMatrix();

	glPushMatrix();
	glTranslatef( middle_point.X(), middle_point.Y(), middle_point.Z() );
	glMultMatrixf( matrix_orientation.GetSafeM() );

	glBegin( GL_TRIANGLE_STRIP );
	for ( int i = 0; i <= N; i++ ) {
		float x  = radius * cosf( ang * i );
		float y  = radius * sinf( ang * i );
		float sq = sqrt( x * x + y * y );
		glNormal3f( x / sq, y / sq, 0.0f );
		glVertex3f( x, y, length / 2.0f - h );
		glVertex3f( x, y, -length / 2.0f );

	}
	glEnd();
	if (!arrow) {
		glPopMatrix();
		return;
	}
	glBegin( GL_TRIANGLE_FAN );
	glNormal3f( 0, 0, 1 );
	glVertex3f( 0, 0, length / 2 );
	for ( int i = 0; i <= N; i++ ) {
		float x  = 2.0f * radius * cosf( ang * i );
		float y  = 2.0f * radius * sinf( ang * i );
		float sq = sqrt( x * x + y * y );
		glNormal3f( x / sq, y / sq, 0 );
		glVertex3f( x, y, length / 2 - h );

	}
	glEnd();

	glPopMatrix();
}


void draw_scal_axes( float length ) {
	std::cout << "draw_scal_axes\n";
	//x
	glPushMatrix();
	glRotatef( 90, 0, 1, 0 );
	glColor4f( 1.0, 0.0, 0.0, 0.8 );
	draw_scal_axis( 0.01 * length, length );
	//y
	glRotatef( -90, 1, 0, 0 );
	glColor4f( 0.0, 1.0, 0.0, 0.8 );
	draw_scal_axis( 0.01 * length, length );
	glPopMatrix();

	//z
	glColor4f( 0.0, 0.0, 1.0, 0.8 );
	draw_scal_axis( 0.01 * length, length );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
}


void draw_vec_axes( float length ) {
	std::cout << "draw_scal_axes\n";
	//x
	glColor4f( 1.0f, 0.0f, 0.0f, 0.8f );
	draw_vec_axis( OVector3( -length * 0.5f, 0.0f, 0.0f ), OVector3( 1.0f, 0.0f, 0.0f ), length );
	//y
	glColor4f( 0.0f, 1.0f, 0.0f, 0.8f );
	draw_vec_axis( OVector3( 0.0f, -length * 0.5f, 0.0f ), OVector3( 0.0f, 1.0f, 0.0f ), length );
	//z
	glColor4f( 0.0f, 0.0f, 1.0f, 0.8f );
	draw_vec_axis( OVector3( 0.0f, 0.0f, -length * 0.5f ), OVector3( 0.0f, 0.0f, 1.0f ), length );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}


