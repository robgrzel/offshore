//
// Created by robgrzel on 21.04.17.
//

#define GL_GLEXT_PROTOTYPES
#define GL3_PROTOTYPES
#define GLM_FORCE_RADIANS

#include "shader_utils.h"
#include "res_texture.c"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL.h>
#include <Fl/gl.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_utils.h"

struct point {
	GLfloat x;
	GLfloat y;
};


static point* sample_points( const size_t n );

class Gl_Plot_Graph {
	private:

		float               pixel_x, pixel_y;
		float               offset_x      = 0;
		float               scale_x       = 1;
		int                 window_width  = 640;
		int                 window_height = 480;
		const int           wBorder       = 10;
		const int           ticksize      = 10;
		static const int    nticks        = 21;
		static const size_t N             = 2000;
		static const point  border[4];
		glm::mat4           transform;
		point               graph[N];
		point               ticks[nticks * 2];
		GLuint              program;
		GLuint              attribute_coord2d;
		GLint               uniform_color;
		GLint               uniform_transform;
		GLenum              glew_status;
		GLuint              vbo[3];
		SDL_GLContext       context;
		SDL_Window* window;


	public:
		int init_resources( );

		void free_resources( ) {
			glDeleteProgram( program );
		}

		static Gl_Plot_Graph& instance( );

		// Create a projection matrix that has the same effect as glViewport().
		// Optionally return scaling factors to easily convert normalized device coordinates to pixels.
		//
		glm::mat4 viewport_transform( float x, float y, float width, float height, float* pixel_x = 0, float* pixel_y = 0 );


		int init_glsl_program( );

		void plot_graph_sample( );

		void plot_graph( point graph1[N] );

		void plot_graph( );

		void set_graph( point graph[N] );

		void draw_xticks( );

		void draw_yticks( );

		void draw_borders( );

		void draw_graph( );

		void display( ) {
			glUseProgram( program );

			glClearColor( 1, 1, 1, 1 );
			glClear( GL_COLOR_BUFFER_BIT );

			draw_graph();
			draw_borders();
			draw_yticks();
			draw_xticks();

			// And we are done.

			glDisableVertexAttribArray( attribute_coord2d );
		}

		void keyDown( SDL_KeyboardEvent* ev );

		void windowEvent( SDL_WindowEvent* ev );

		void mainLoop( SDL_Window* window );

		Gl_Plot_Graph( );

};

const point Gl_Plot_Graph::border[4] = {{-1, -1},
                                        {1,  -1},
                                        {1,  1},
                                        {-1, 1}
};


void Gl_Plot_Graph::keyDown( SDL_KeyboardEvent* ev ) {
	switch ( ev->keysym.scancode ) {
		case SDL_SCANCODE_LEFT: offset_x -= 0.03;
			break;
		case SDL_SCANCODE_RIGHT: offset_x += 0.03;
			break;
		case SDL_SCANCODE_UP: scale_x *= 1.5;
			break;
		case SDL_SCANCODE_DOWN: scale_x /= 1.5;
			break;
		case SDL_SCANCODE_HOME: offset_x = 0.0;
			scale_x = 1.0;
			break;
		default: break;
	}
}

void Gl_Plot_Graph::windowEvent( SDL_WindowEvent* ev ) {
	switch ( ev->event ) {
		case SDL_WINDOWEVENT_SIZE_CHANGED: window_width = ev->data1;
			window_height = ev->data2;
			glViewport( 0, 0, ev->data1, ev->data2 );
			break;
		default: break;
	}
}