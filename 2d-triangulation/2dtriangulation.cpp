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
	for (int i = 0; i < points; i++)
		glVertex2i(rand() % WIDTH, rand() % HEIGHT);
	glEnd();
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("|                          2D - TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
    // quit with q
    case 'q':
        exit(0);
        break;
	// quit with esc
	case 27:
		exit(0);

		break;
	case 's':
		break;
	case 'c':
		break;
	case 'z':
		break;
	case 'p':
		break;
	case 'r':
		
		break;
	case 'h':
		showcmds();
	}
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

	// Set dot color to white
	glColor3f(1.0, 1.0, 1.0);
	drawPoints(5);

    glutSwapBuffers();
}

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

	// print controls
	showcmds();

	// flut functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}