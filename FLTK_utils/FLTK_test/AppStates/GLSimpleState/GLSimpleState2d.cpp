#include "GLSimpleState2d.h"


GLSimpleState2d::GLSimpleState2d( ) :
		m_initialized( false ) {
	std::cout << "GLSimpleState2d::GLSimpleState2d( )\n";
	m_buttonPressed = false;
	m_startX        = -100;
	m_startY        = -100;
}


GLSimpleState2d::~GLSimpleState2d( ) {
	std::cout << "GLSimpleState2d::~GLSimpleState2d( )\n";

}



void GLSimpleState2d::init( void* data ) {
	std::cout << "GLSimpleState2d::init( void* data )\n";


	if (data != NULL) {
		float aspect = *( float* ) data;
		m_camera.SetPerspective( OVector4( 1, 10000, 10, aspect ) );
		if (!m_initialized) {
			m_camera.PlaceAt( OVector3( 0,-150, 0 ) );
			m_camera.LookAt( OVector3( 0, 0, 0 ) );
		}
	}


	if (!m_initialized) {
		glDisable( GL_CULL_FACE );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
		glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
		glEnable( GL_TEXTURE_2D );

		//?
		glPixelZoom( 1.0, 1.0 );

		m_initialized = true;
	}

}

void GLSimpleState2d::clear( ) {
	std::cout << "GLSimpleState2d::clear( )\n";

}

void GLSimpleState2d::update( void* data ) {
	std::cout << "GLSimpleState2d::update( void* data )\n";
	m_camera.SetGLModelViewMatrix();
}

static float px = 5;

void loop_simple_wave(){

	float z = px;
	DRAW2D.SetColor(1,1,0,1);
	DRAW2D.DrawPointXZ(0,-5,z,0.5);
	DRAW2D.SetColor(1,0,1,1);
	DRAW2D.DrawLine(0,-6,z-0.1,0,-6,z+0.1,2);

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	std::cout<<"z=sin(px)="<<z<<std::endl;
	static OSpecialMesh mesh(10, 10, 5.0f, 1.0f,0,0,z);
	mesh.DrawMesh(OSpecialMesh::CYLINDER);

	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisable( GL_NORMALIZE );

	px*=-1;


}

void GLSimpleState2d::render( ) {
	std::cout << "GLSimpleState2d::render( )\n";
	glClearColor( 0,0,0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	draw_scal_axes( 5.0f );
	glEnable( GL_NORMALIZE );

	DRAW2D.SetColor(1,1,1,1);

	VDraws->DrawBox( OVector3( -10,0,-10 ), OVector3( 10,0,10 ) );

	VDraws->DrawVerticalXZQuad(10,10,0.3f, 0.5f, 0.7f, 0.1f);

	loop_simple_wave();

	glFlush();

}

bool GLSimpleState2d::initialized( ) {
	std::cout << "GLSimpleState2d::initialized( )\n";
	return m_initialized;
}


bool GLSimpleState2d::event( UIEvent* event ) {
	std::cout << "GLSimpleState2d::event( UIEvent* event )\n";
	if (event == nullptr) {
		return true;
	}

	switch ( event->type() ) {

		case UIEvent::MOUSE_WHEEL: {
			const float dist = 10.0f;
			if (event->ry() > 0) {
				m_camera.Translate( dist );
			} else {
				m_camera.Translate( -dist );
			}
		}
			break;
	}

	return true;
}