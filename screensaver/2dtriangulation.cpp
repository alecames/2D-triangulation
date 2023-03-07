
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

// This program will preform 2D triangulation on a set of points

struct Color { float r, g, b; };
struct Cube { float x, y, z; };

// formats the prints with the table edges
void padprint(const char* str) {
	int len = strlen(str);
	int pad = 70 - len;
	printf("| ");
	printf("%s", str);
	for (int i = 0; i < pad; i++)
		printf(" ");
	printf("|\n");
	printf("|-----------------------------------------------------------------------|\r");
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                   CUBE SCREENSAVER              ESC / Q: Quit |\n");
	printf("|------------------------------ CONTROLS ------------------------------ |\n");
	printf("| S: Slow mode | C: Change Colour | Z: Default | P: Pulse | R: RGB Mode |\n");
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(600, 1, 0.01, 1000);
    glutSwapBuffers();
}

void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("2D Triangulation");
	glutFullScreen();
    glEnable(GL_DEPTH_TEST);

	// print controls
	showcmds();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    // attempt to smooth edges
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();
    return 0;
}