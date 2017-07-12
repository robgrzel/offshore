//
// Created by robgrzel on 21.04.17.
//

#include <iostream>
#include "OpenGl_Plot2d_002.h"


void fill_sample_points( const size_t n, point graph[] ) {

	// Fill buffer in just like an array
	for ( size_t i = 0; i < n; i++ ) {
		float x = (i - 1000.0) / 100.0;

		graph[i].x = x;
		graph[i].y = sin( x * 10.0 ) / (1.0 + x * x);
	};


};


void Gl_Plot_Graph::plot_graph_sample( ) {
	std::cout << "Gl_Plot_Graph::plot_graph_sample(0)\n";

	point graph[N];
	fill_sample_points( N, graph );
	plot_graph( graph );

};

void Gl_Plot_Graph::set_graph( point graph[N] ) {

	for ( size_t i = 0; i < N; i++ ) {
		this->graph[i] = graph[i];
	}

};

void Gl_Plot_Graph::plot_graph( point graph[N] ) {

	// Tell OpenGL to copy our array to the buffer object
	set_graph( graph );
	plot_graph();
}


void Gl_Plot_Graph::plot_graph( ) {
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );

	// Tell OpenGL to copy our array to the buffer object
	glBufferData( GL_ARRAY_BUFFER, N * sizeof( point ), graph, GL_STATIC_DRAW );

}

int Gl_Plot_Graph::init_glsl_program( ) {
	program = create_program(
			"/mnt/WSPACE/MEGASync-P/WORK/Proj/UTILS/OpenGL/OpenGL_tutorial/graph002.v.glsl",
			"/mnt/WSPACE/MEGASync-P/WORK/Proj/UTILS/OpenGL/OpenGL_tutorial/graph002.f.glsl"
	                        );
	if (program == 0) {
		return 1;
	}

	attribute_coord2d = get_attrib( program, "coord2d" );
	uniform_transform = get_uniform( program, "transform" );
	uniform_color     = get_uniform( program, "color" );

	if (attribute_coord2d == -1 || uniform_transform == -1 || uniform_color == -1) {
		return 0;
	}
}

int Gl_Plot_Graph::init_resources( ) {
	std::cout << "Gl_Plot_Graph::init_resources(0)\n";

	glew_status = glewInit();

	if (GLEW_OK != glew_status) {
		fprintf( stderr, "Error: %s\n", glewGetErrorString( glew_status ) );
		exit( 1 );
	}

	if (!GLEW_VERSION_2_0) {
		fprintf( stderr, "No support for OpenGL 2.0 found\n" );
		exit( 1 );
	}

	if (init_glsl_program()) return 1;

	// Create the vertex buffer object (VBO)
	glGenBuffers( 3, vbo );

	std::cout << "Gl_Plot_Graph::init_resources(E)\n";

	return 0;
}


// Create a projection matrix that has the same effect as glViewport().
// Optionally return scaling factors to easily convert normalized device coordinates to pixels.
//
glm::mat4 Gl_Plot_Graph::viewport_transform( float x, float y, float width, float height, float* pixel_x, float* pixel_y ) {

	// Map OpenGL coordinates (-1,-1) to window coordinates (x,y),
	// (1,1) to (x + width, y + height).

	// Calculate how to translate the x and y coordinates:
	float offset_x = (2.0 * x + (width - window_width)) / window_width;
	float offset_y = (2.0 * y + (height - window_height)) / window_height;

	// Calculate how to rescale the x and y coordinates:
	float scale_x = width / window_width;
	float scale_y = height / window_height;

	// Calculate size of pixels in OpenGL coordinates
	if (pixel_x) {
		*pixel_x = 2.0 / width;
	}
	if (pixel_y) {
		*pixel_y = 2.0 / height;
	}

	return glm::scale( glm::translate( glm::mat4( 1 ), glm::vec3( offset_x, offset_y, 0 ) ), glm::vec3( scale_x, scale_y, 1 ) );
}

void Gl_Plot_Graph::draw_xticks( ) {
	/* ---------------------------------------------------------------- */
	/* Draw the x tick marks */

	float tickspacing = 0.1 * powf( 10, -floor( log10( scale_x ) ) );    // desired space between ticks, in graph coordinates
	float left        = -1.0 / scale_x - offset_x;    // left edge, in graph coordinates
	float right       = 1.0 / scale_x - offset_x;    // right edge, in graph coordinates
	int   left_i      = ceil( left / tickspacing );    // index of left tick, counted from the origin
	int   right_i     = floor( right / tickspacing );    // index of right tick, counted from the origin
	float rem         = left_i * tickspacing - left;    // space between left edge of graph and the first tick

	float firsttick = -1.0 + rem * scale_x;    // first tick in device coordinates

	int nticks = right_i - left_i + 1;    // number of ticks to show

	if (nticks > 21) {
		nticks = this->nticks;
	}    // should not happen

	float xtick, xtickscale;

	for ( int i = 0; i < nticks; i++ ) {
		xtick      = firsttick + i * tickspacing * scale_x;
		xtickscale = ((i + left_i) % 10) ? 0.5 : 1;

		ticks[i * 2].x     = xtick;
		ticks[i * 2].y     = -1;
		ticks[i * 2 + 1].x = xtick;
		ticks[i * 2 + 1].y = -1 - ticksize * xtickscale * pixel_y;
	}

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW );
	//glVertexAttribPointer( attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_LINES, 0, nticks * 2 );

}

void Gl_Plot_Graph::draw_yticks( ) {
	/* ---------------------------------------------------------------- */
	/* Draw the y tick marks */

	float     ytick, ytickscale;
	for ( int i = 0; i < nticks; i++ ) {
		ytick      = -1 + i * 0.1;
		ytickscale = (i % 10) ? 0.5 : 1;

		ticks[i * 2].x     = -1;
		ticks[i * 2].y     = ytick;
		ticks[i * 2 + 1].x = -1 - ticksize * ytickscale * pixel_x;
		ticks[i * 2 + 1].y = ytick;
	}

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW );
	glVertexAttribPointer( attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_LINES, 0, nticks * 2 );

}

void Gl_Plot_Graph::draw_borders( ) {
	/* ---------------------------------------------------------------- */
	/* Draw the borders */


	// Calculate a transformation matrix that gives us the same normalized device coordinates as above
	transform = viewport_transform( wBorder + ticksize,
	                                wBorder + ticksize,
	                                window_width - wBorder * 2 - ticksize,
	                                window_height - wBorder * 2 - ticksize,
	                                &pixel_x, &pixel_y
	                              );

	// Tell our vertex shader about it
	glUniformMatrix4fv( uniform_transform, 1, GL_FALSE, glm::value_ptr( transform ) );

	// Set the color to black
	GLfloat black[4] = {0, 0, 0, 1};
	glUniform4fv( uniform_color, 1, black );

	// Draw a border around our graph
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW );
	glVertexAttribPointer( attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
}

void Gl_Plot_Graph::draw_graph( ) {
	/* ---------------------------------------------------------------- */
	/* Draw the graph */

	// Set our viewport, this will clip geometry
	glViewport( wBorder + ticksize, wBorder + ticksize, window_width - wBorder * 2 - ticksize, window_height - wBorder * 2 - ticksize );

	// Set the scissor rectangle,this will clip fragments
	glScissor( wBorder + ticksize, wBorder + ticksize, window_width - wBorder * 2 - ticksize, window_height - wBorder * 2 - ticksize );

	glEnable( GL_SCISSOR_TEST );

	// Set our coordinate transformation matrix
	transform = glm::translate(
			glm::scale(
					glm::mat4( 1.0f ),
					glm::vec3( scale_x, 1, 1 )
			          ),
			glm::vec3( offset_x, 0, 0 )
	                          );

	glUniformMatrix4fv( uniform_transform, 1, GL_FALSE, glm::value_ptr( transform ) );

	// Set the color to red
	GLfloat red[4] = {1, 0, 0, 1};
	glUniform4fv( uniform_color, 1, red );

	// Draw using the vertices in our vertex buffer object
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );

	glEnableVertexAttribArray( attribute_coord2d );
	glVertexAttribPointer( attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glDrawArrays( GL_LINE_STRIP, 0, 2000 );

	// Stop clipping
	glViewport( 0, 0, window_width, window_height );
	glDisable( GL_SCISSOR_TEST );
}


void Gl_Plot_Graph::mainLoop( SDL_Window* window ) {
	while ( true ) {
		Gl_Plot_Graph::display();
		SDL_GL_SwapWindow( window );

		bool redraw = true;

		while ( redraw ) {
			SDL_Event ev;
			if (!SDL_WaitEvent( &ev )) {
				return;
			}

			switch ( ev.type ) {
				case SDL_QUIT: return;
				case SDL_KEYDOWN: Gl_Plot_Graph::keyDown( &ev.key );
					redraw = false;
					break;
				case SDL_WINDOWEVENT: Gl_Plot_Graph::windowEvent( &ev.window );
					redraw = false;
					break;
				default: break;
			}
		}
	}
}


Gl_Plot_Graph::Gl_Plot_Graph( ) {

	std::cout << "Gl_Plot_Graph::Gl_Plot_Graph(0)\n";

	window = SDL_CreateWindow( "My Graph",
	                           SDL_WINDOWPOS_CENTERED,
	                           SDL_WINDOWPOS_CENTERED,
	                           window_width, window_height,
	                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	                         );

	context = SDL_GL_CreateContext( window );

	if (init_resources()) exit( EXIT_FAILURE );

	printf( "Use left/right to move horizontally.\n" );
	printf( "Use up/down to change the horizontal scale.\n" );
	printf( "Press home to reset the position and scale.\n" );
	printf( "Press F1 to toggle interpolation.\n" );
	printf( "Press F2 to toggle clamping.\n" );
	printf( "Press F3 to toggle drawing points.\n" );

	plot_graph_sample();

	this->mainLoop( window );

	free_resources();

	SDL_GL_DeleteContext( context );
	SDL_DestroyWindow( window );
	SDL_Quit();
}

int main( int argc, char* argv[] ) {
	std::cout << "SDL_Init(SDL_INIT_VIDEO)\n";
	SDL_Init( SDL_INIT_VIDEO );
	std::cout << "Gl_Plot_Graph()\n";


	Gl_Plot_Graph();


	return EXIT_SUCCESS;
}
