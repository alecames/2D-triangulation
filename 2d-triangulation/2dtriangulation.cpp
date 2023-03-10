#include <freeglut.h>
#include <FreeImage.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

const int WIDTH = 800;
const int HEIGHT = 800;

// This program will preform 2D triangulation on a set of points

// render the points to be triangulated to each other
void drawPoints(int points) {
	glPointSize(4);
	glBegin(GL_POINTS);
	for (int i = 0; i < points; i++) {
		int x = rand() % WIDTH;
		int y = rand() % HEIGHT;
		glVertex2i(x, y);
	}
	glEnd();
}

void init() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Set dot color to white
	glColor3f(1.0, 1.0, 1.0);
	drawPoints(10);	// argument should be some number greater than 3 so that atleast one triangle can be formed
	// ... unless we don't wanna do that (?)

	glutSwapBuffers();
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D - TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| R: Reset Points |                                                     |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
    // quit with q
    case 'q':
	case 'Q':
        exit(0);
        break;
	// quit with esc
	case 27:
		exit(0);
		break;
	// reset display
	case 'r':
	case 'R':
		init();
		break;
	// h - help
	case 'h':
	case 'H':
		showcmds();
		break;
	}
}

// commenting this out because we might need this later, but I doubt it. 
//void display(void) {
//    glClear(GL_COLOR_BUFFER_BIT);
//
//	// Set dot color to white
//	glColor3f(1.0, 1.0, 1.0);
//	drawPoints(10);	// argument should be some number greater than 3 so that atleast one triangle can be formed
//					// ... unless we don't wanna do that (?)
//
//    glutSwapBuffers();
//}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH)/2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT)/2);	// makes the window appear in the center of the screen
    glutCreateWindow("2D Triangulation");

	// initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);	// creates drawable projection

	// show print controls
	showcmds();

	// glut functions
    //glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

	init();		// initialize all the points to be rendered 
	glutSetCursor(GLUT_CURSOR_NONE);	// makes mouse cursor disappear on glut window

    glutMainLoop();
    return 0;
}