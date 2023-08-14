#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "texture.h"
#include "camera.h"
#include "LightSource.h"
#include "Material.h"
#include "Model.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800
#define HALF_WIDTH (WINDOW_WIDTH >> 1)     
#define HALF_HEIGHT (WINDOW_HEIGHT >> 1)

// Perspective Camera Parameters
#define FOVY 70.0f  // field of view, pitch angle.
#define NEAR_PLANE 0.01f
#define FAR_PLANE  500.0f

// Animation
#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

// Camera Turn/Move with Mouse
#define  MOUSE        1  // Turn around with mouse
#define  INVERT_Y     1  // to invert look, set to -1

#define  MOUSE_SENSITIVITY  10.0f

#define D2R 0.0174532

/* Global Variables for Template File */
bool upKey = false, downKey = false, rightKey = false, leftKey = false;
bool wKey = false, sKey = false, aKey = false, dKey = false, spaceKey = false;
int  winWidth, winHeight; // current Window width and height
int  halfWidth = HALF_WIDTH, halfHeight = HALF_HEIGHT;
bool mode = true;  // F1 key to toggle drawing mode between GL_LINE and GL_FILL
bool capture = true;  // Middle Mouse button set/unset mouse capture
int  mouseX, mouseY; // current mouse position

// Application Global Variables

Camera cam(2.5, 0, 2, 0, 0, 0.1); // at the origin, looking at -z, speed: 0.1
Texture pyramid;
int drawMode = 0;
float angle = 0;

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

// Bitmap font text (2D)
void print(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vectoral 3D text (3D) at the origin
void text3D(const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
}

int mood = 0;
LightSource pLS1 = { GL_LIGHT1,
					 { 0.1, 0.1, 0.1, 1 },  // ambient component (set all values to 0.1 for white)
					{ 1.0, 1.0, 1.0, 1 },  // diffuse component (set all values to 1.0 for white)
					{ 1.0, 1.0, 1.0, 1 }, // specular component
					 true
};
void Pyramid() {
	if (mood == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		pyramid.use();
	}

	// Set rendering mode based on mood
	else if (mood == 0) {
		glColor3f(1, 1, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}


	//front face
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5, 0);  glVertex3f(0, 2, 2);
	glTexCoord2f(0.25, 0.273);  glVertex3f(1, 0, 2);
	glTexCoord2f(0.75, 0.273);  glVertex3f(-1, 0, 2);
	glEnd();

	// LEFT Face
	glBegin(GL_QUADS);
	glTexCoord2f(0.75, 0.273); glVertex3f(-1, 0, 2);
	glTexCoord2f(0.25, 0.726); glVertex3f(-1, 0, -2);
	glTexCoord2f(0.5, 1);  glVertex3f(0, 2, -2);
	glTexCoord2f(0.5, 0);  glVertex3f(0, 2, 2);
	glEnd();

	// Back Face
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5, 1); glVertex3f(0, 2, -2);
	glTexCoord2f(0.25, 0.726); glVertex3f(-1, 0, -2);
	glTexCoord2f(0.75, 0.726); glVertex3f(1, 0, -2);
	glEnd();

	// RIGHT Face
	glBegin(GL_QUADS);
	glTexCoord2f(0.75, 0.273); glVertex3f(0, 2, -2);
	glTexCoord2f(0.25, 0.726); glVertex3f(0, 2, 2);
	glTexCoord2f(0.5, 1);  glVertex3f(1, 0, 2);
	glTexCoord2f(0.5, 0);  glVertex3f(1, 0, -2);
	glEnd();


}

void FPSView() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 0.5 * winWidth / winHeight, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.LookAt();
	if (mood == 3) {

		glPushMatrix();
		glRotatef(0, 2, 0, 1);
		glTranslatef(3, 3, -3);
		pLS1.addPointLS(0.4);
		glPopMatrix();
		Texture::on();
		Texture::modulate(); // mix texture, lighting and solid colors
		pyramid.minFilter(GL_LINEAR_MIPMAP_LINEAR);
		pyramid.magFilter(GL_LINEAR);
		glPushMatrix();
		glTranslatef(2.5, -0.5, -3);
		glRotatef(angle, 0, 1, 0);
		Pyramid();
		glPopMatrix();

		angle += 0.2;
		Texture::off();

	}
	else if (mood == 2) {
		glPushMatrix();
		glRotatef(0, 2, 0, 1);
		glTranslatef(3, 3, -3);
		pLS1.addPointLS(0.4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(2.5, -0.5, -3);
		glRotatef(angle, 0, 1, 0);
		Pyramid();
		glPopMatrix();
	}
	// Trilinear Filtering (Mipmapping)
	else if (mood == 1) {
		Texture::on();

		Texture::replace();
		pyramid.minFilter(GL_LINEAR_MIPMAP_LINEAR);
		pyramid.magFilter(GL_LINEAR);
		glPushMatrix();
		glTranslatef(2.5, -0.5, -3);
		glRotatef(angle, 0, 1, 0);
		Pyramid();
		glPopMatrix();

		Texture::off();
	}
	else if (mood == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();
		glTranslatef(2.5, -0.5, -3);
		glRotatef(angle, 0, 1, 0);
		Pyramid();
		glPopMatrix();
	}
	angle += 0.2;


}
void planeHD(int res) {
	float d = 1.0 / res;
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	for (float x = -0.5; x < 0.5; x += d) {
		for (float y = -0.5; y < 0.5; y += d) {
			glTexCoord2f(x + 0.5, y + 0.5);  glVertex3f(x, y, 0);
			glTexCoord2f(x + 0.5 + d, y + 0.5); glVertex3f(x + d, y, 0);
			glTexCoord2f(x + 0.5 + d, y + 0.5 + d); glVertex3f(x + d, y + d, 0);
			glTexCoord2f(x + 0.5, y + 0.5 + d); glVertex3f(x, y + d, 0);
		}
	}
	glEnd();
}
void OverlayView() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-winWidth, winWidth, -winHeight, winHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Render 2D objects here
	glColor3f(0, 0, 0);
	glRectf(-winWidth, -winHeight, winWidth, -winHeight + 40);

	glColor3f(1, 1, 0);
}

void display() {
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);
	if (mood == 2 || mood == 3) {
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}
	FPSView();
	// 2D Overlay on top of 3D Renderings
	glLoadIdentity();
	glViewport(0, 0, winWidth, winHeight);
	glDisable(GL_DEPTH_TEST);
	glColor3f(0, 0, 0);
	OverlayView();
	if (mood == 0)
		print(-winWidth + 1000, -winHeight + 50, GLUT_BITMAP_TIMES_ROMAN_24, "Wireframe %d", mood);
	else if (mood == 1)
		print(-winWidth + 1000, -winHeight + 50, GLUT_BITMAP_TIMES_ROMAN_24, "Only texture %d", mood);
	else if (mood == 2)
		print(-winWidth + 1000, -winHeight + 50, GLUT_BITMAP_TIMES_ROMAN_24, "Only Lightning %d", mood);
	else if (mood == 3)
		print(-winWidth + 1000, -winHeight + 50, GLUT_BITMAP_TIMES_ROMAN_24, "Lightning + Texture %d", mood);
	glLoadIdentity();



	glutSwapBuffers();
}
//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeydown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = true; break;
	case 's':
	case 'S': sKey = true; break;
	case 'a':
	case 'A': aKey = true; break;
	case 'd':
	case 'D': dKey = true; break;
	case ' ': spaceKey = true; break;
	}

	// to refresh the window it calls display() function
	// glutPostRedisplay();
}

void onKeyup(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = false; break;
	case 's':
	case 'S': sKey = false; break;
	case 'a':
	case 'A': aKey = false; break;
	case 'd':
	case 'D': dKey = false; break;
	case ' ': spaceKey = false; break;
	}

	// to refresh the window it calls display() function
	// glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_upKey, GLUT_KEY_downKey, GLUT_KEY_rightKey, GLUT_KEY_rightKey
//
void onSpecialKeydown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = true; break;
	case GLUT_KEY_DOWN: downKey = true; break;
	case GLUT_KEY_LEFT: leftKey = true; break;
	case GLUT_KEY_RIGHT: rightKey = true; break;

	}
	// to refresh the window it calls display() function
	// glutPostRedisplay();
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_upKey, GLUT_KEY_downKey, GLUT_KEY_rightKey, GLUT_KEY_rightKey
//
void onSpecialKeyup(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = false; break;
	case GLUT_KEY_DOWN: downKey = false; break;
	case GLUT_KEY_LEFT: leftKey = false; break;
	case GLUT_KEY_RIGHT: rightKey = false; break;
	case GLUT_KEY_F1:
		mood = (mood + 1) % 4; // Cycle through rendering modes: 0, 1, 2
		break;
	}
	// to refresh the window it calls display() function
	// glutPostRedisplay();
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_leftKey_BUTTON , GLUT_rightKey_BUTTON
// states  : GLUT_upKey , GLUT_downKey
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int status, int x, int y)
{
	// Write your codes here.
#if MOUSE == 1
	if (button == GLUT_MIDDLE_BUTTON && status == GLUT_DOWN) {
		capture = !capture;
		if (capture) {
			glutSetCursor(GLUT_CURSOR_NONE);
			glutWarpPointer(halfWidth, halfHeight);
			mouseX = halfWidth;
			mouseY = halfHeight;
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
	}
#endif


	// to refresh the window it calls display() function
	// glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	halfWidth = w >> 1;
	halfHeight = h >> 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, winWidth * 1.0f / winHeight, NEAR_PLANE, FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	display(); // refresh window.
}

void onMovedownKey(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function	
	// glutPostRedisplay();
}

// direction: +1 or -1
void onWheel(int button, int direction, int x, int y) {

	// glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.
#if MOUSE == 1
	mouseX = x;
	mouseY = y;
#endif


	// to refresh the window it calls display() function
	// glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
#if MOUSE == 1
	if (capture) {
		cam.TurnRightLeft((halfWidth - mouseX) / MOUSE_SENSITIVITY);
		cam.TurnUpDown(INVERT_Y * (halfHeight - mouseY) / MOUSE_SENSITIVITY);
		glutWarpPointer(halfWidth, halfHeight);
	}
	if (aKey) {
		cam.Left();
	}
	if (dKey) {
		cam.Right();
	}
#endif
	if (wKey) {
		cam.Forward();
	}
	if (sKey) {
		cam.Backward();
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif


void Init() {
	// Load Textures
	pLS1.init();

	pyramid.Load("./tomb.jpg");





	glEnable(GL_DEPTH_TEST);
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if MOUSE == 1
	glutWarpPointer(halfWidth, halfHeight);
	glutSetCursor(GLUT_CURSOR_NONE);
#endif 

}


void main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Template File");

	Init();

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeydown);
	glutSpecialFunc(onSpecialKeydown);

	glutKeyboardUpFunc(onKeyup);
	glutSpecialUpFunc(onSpecialKeyup);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMovedownKey);
	glutPassiveMotionFunc(onMove);
	glutMouseWheelFunc(onWheel);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif
	glutMainLoop();
}