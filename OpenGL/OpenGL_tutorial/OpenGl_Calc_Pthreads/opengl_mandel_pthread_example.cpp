// #include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <errno.h>

using namespace std;

void set_texture();

typedef struct { unsigned char r, g, b; } rgb_t;
rgb_t **tex = 0;
int gwin;
GLuint texture;
int width, height;
int tex_w, tex_h;
double scale = 1. / 256;
double cx = -.6, cy = 0;
int color_rotate = 0;
int saturation = 1;
int invert = 0;
int max_iter = 256;

void render()
{
	double	x = (double)width / tex_w,
	          y = (double)height / tex_h;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLenum error = glGetError();

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	error = glGetError();

	glBegin(GL_QUADS);

	//glTexCoord2f(0, 0); glVertex2i(0, 0);
	//glTexCoord2f(x, 0); glVertex2i(width, 0);
	//glTexCoord2f(x, y); glVertex2i(width, height);
	//glTexCoord2f(0, y); glVertex2i(0, height);

	glTexCoord2f(0, y); glVertex2i(0, height);
	glTexCoord2f(x, y); glVertex2i(width, height);
	glTexCoord2f(x, 0); glVertex2i(width, 0);
	glTexCoord2f(0, 0); glVertex2i(0, 0);

	glEnd();

	glFlush();

	glutSwapBuffers();
}

int dump = 1;
void screen_dump()
{
	char fn[100];
	int i;
	sprintf(fn, "screen%03d.ppm", dump++);
	FILE *fp = fopen(fn, "w");
	fprintf(fp, "P6\n%d %d\n255\n", width, height);
	for (i = height - 1; i >= 0; i--)
		fwrite(tex[i], 1, width * 3, fp);
	fclose(fp);
	printf("%s written\n", fn);
}

void keypress(unsigned char key, int x, int y)
{
	switch (key) {
		case 'q':	glFinish();
			glutDestroyWindow(gwin);
			return;
		case 27:	scale = 1. / 256; cx = -.6; cy = 0; break;

		case 'r':	color_rotate = (color_rotate + 1) % 6;
			break;

		case '>': case '.':
			max_iter += 128;
			if (max_iter > 1 << 15) max_iter = 1 << 15;
			printf("max iter: %d\n", max_iter);
			break;

		case '<': case ',':
			max_iter -= 128;
			if (max_iter < 128) max_iter = 128;
			printf("max iter: %d\n", max_iter);
			break;

		case 'c':	saturation = 1 - saturation;
			break;

		case 's':	screen_dump(); return;
		case 'z':	max_iter = 4096; break;
		case 'x':	max_iter = 128; break;
		case ' ':	invert = !invert;
	}
	set_texture();
}

void hsv_to_rgb(int hue, int min, int max, rgb_t *p)
{
	if (min == max) max = min + 1;
	if (invert) hue = max - (hue - min);
	if (!saturation) {
		p->r = p->g = p->b = 255 * (max - hue) / (max - min);
		return;
	}
	double h = fmod(color_rotate + 1e-4 + 4.0 * (hue - min) / (max - min), 6);
	#	define VAL 255
	double c = VAL * saturation;
	double X = c * (1 - fabs(fmod(h, 2) - 1));

	p->r = p->g = p->b = 0;

	switch ((int)h) {
		case 0: p->r = c; p->g = X; return;
		case 1:	p->r = X; p->g = c; return;
		case 2: p->g = c; p->b = X; return;
		case 3: p->g = X; p->b = c; return;
		case 4: p->r = X; p->b = c; return;
		default:p->r = c; p->b = X;
	}
}

struct param_mandel{
	double x_from;
	double x_to;
	double y_from;
	double y_to;
};

void* calc_mandel_by_xy(void* param_thread)
{
	printf("Thread %d starting \n", pthread_self() );
	struct param_mandel* param = (struct param_mandel*) param_thread;
	int i, j, iter, min, max;
	rgb_t *px;
	double x, y, zx, zy, zx2, zy2;
	min = max_iter; max = 0;

	for (i = param->y_from; i < param->y_to; i++) {
		px = tex[i];
		y = (i - height / 2) * scale + cy;
		for (j = 0; j < width; j++, px++) {
			x = (j - width / 2) * scale + cx;
			iter = 0;

			zx = hypot(x - .25, y);
			if (x < zx - 2 * zx * zx + .25) iter = max_iter;
			if ((x + 1)*(x + 1) + y * y < 1 / 16) iter = max_iter;

			zx = zy = zx2 = zy2 = 0;
			for (; iter < max_iter && zx2 + zy2 < 4; iter++) {
				zy = 2 * zx * zy + y;
				zx = zx2 - zy2 + x;
				zx2 = zx * zx;
				zy2 = zy * zy;
			}
			if (iter < min) min = iter;
			if (iter > max) max = iter;
			*(unsigned short *)px = iter;
		}
	}

	printf("Thread %d ending \n", pthread_self());

	return NULL;
}


void calc_mandel()
{
	int i, j, iter, min, max;
	rgb_t *px;
	min = max_iter; max = 0;

	const clock_t begin_time = clock();

	double y_half = height / 2;
	double x_half = width / 2;

	pthread_t thread_id1, thread_id2, thread_id3, thread_id4;

	param_mandel param1, param2, param3, param4;

	if (true){
		param1.x_from = 0, param1.x_to = x_half, param1.y_from = 0, param1.y_to = y_half;
		param2.x_from = x_half, param2.x_to = height, param2.y_from = 0, param2.y_to = y_half;

		param3.x_from = 0, param3.x_to = x_half, param3.y_from = y_half, param3.y_to = width;
		param4.x_from = x_half, param4.x_to = height, param4.y_from = y_half, param4.y_to = width;

		pthread_create(&thread_id1, NULL, &calc_mandel_by_xy, &param1);
		pthread_create(&thread_id2, NULL, &calc_mandel_by_xy, &param2);
		pthread_create(&thread_id3, NULL, &calc_mandel_by_xy, &param3);
		pthread_create(&thread_id4, NULL, &calc_mandel_by_xy, &param4);

		pthread_join(thread_id1, NULL);
		pthread_join(thread_id2, NULL);
		pthread_join(thread_id3, NULL);
		pthread_join(thread_id4, NULL);
	}
	else{
		param1.x_from = 0, param1.x_to = height, param1.y_from = 0, param1.y_to = width;
		calc_mandel_by_xy((void*)&param1);
	}


	std::cout << float(clock() - begin_time) / CLOCKS_PER_SEC;
	std::cout << "s" << endl ;

	for (i = 0; i < height; i++)
		for (j = 0, px = tex[i]; j < width; j++, px++)
			hsv_to_rgb(*(unsigned short*)px, min, max, px);
}

void alloc_tex()
{
	int i, ow = tex_w, oh = tex_h;

	for (tex_w = 1; tex_w < width; tex_w <<= 1);
	for (tex_h = 1; tex_h < height; tex_h <<= 1);

	if (tex_h != oh || tex_w != ow)
		tex = (rgb_t**) realloc(tex, tex_h * tex_w * 3 + tex_h * sizeof(rgb_t*));

	for (tex[0] = (rgb_t *)(tex + tex_h), i = 1; i < tex_h; i++)
		tex[i] = tex[i - 1] + tex_w;
}

void set_texture()
{
	alloc_tex();
	calc_mandel();

	glBindTexture(GL_TEXTURE_2D, texture);

	GLenum error = glGetError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	error = glGetError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	error = glGetError();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex[0]);

	error = glGetError();

	render();
}

void mouseclick(int button, int state, int x, int y)
{
	if (state != GLUT_UP) return;

	cx += (x - width / 2) * scale;
	cy -= (y - height / 2) * scale;

	switch (button) {
		case GLUT_LEFT_BUTTON: /* zoom in */
			if (scale > fabs((float)x) * 1e-16 && scale > fabs((float)y) * 1e-16)
				scale /= 2;
			break;
		case GLUT_RIGHT_BUTTON: /* zoom out */
			scale *= 2;
			break;
			/* any other button recenters */
	}
	set_texture();
}


void resize(int w, int h)
{
	printf("resize %d %d\n", w, h);
	width = w;
	height = h;

	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -1, 1);

	set_texture();
}

void init_gfx(int *c, char **v)
{
	glutInit(c, v);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glClearColor(0.0, 0.0, 0.0, 1.0); // cor do fundo
	glClearDepth(1.0);

	width = 1000;
	height = 600;

	glutInitWindowSize(width, height);

	std::cout << "Initializing window" << endl;

	gwin = glutCreateWindow("Mandelbrot");

	glutDisplayFunc(render);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouseclick);
	glutReshapeFunc(resize);

	std::cout << "Initializing texture for Mandelbrot" << endl;
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(1, &texture);
	set_texture();
}

int main1(int c, char **v)
{
	init_gfx(&c, v);
	std::cout << "test!!" << endl;
	printf("keys:\n\tr: color rotation\n\tc: monochrome\n\ts: screen dump\n\t"
			       "<, >: decrease/increase max iteration\n\tq: quit\n\tmouse buttons to zoom\n");

	glutMainLoop();
	return 0;
}
