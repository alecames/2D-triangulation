#include <freeglut.h>
#include <FreeImage.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <cmath>
#include <ctime>
#include <algorithm>

// This program will preform 2D triangulation on a set of points

// Immutable
const int WIDTH = 800;
const int HEIGHT = 800;
const int POINT_SIZE = 6;
const int NPOINTS = 4;		// "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

struct Point {
	int x, y;
};
Point P[NPOINTS];

struct Edge {
	Point p1;
	Point p2;
	unsigned int length;
};
Edge Edge_List[NPOINTS*(NPOINTS-1)/2];	// number of edges calculated using nC2 = n(n-1) / 2

void initEdges() {
	// calculate length, save to edge list;
	int numEdges = 0;
	//printf("numEdges: %i\n\n", numEdges);

	for (int i = 0; i < NPOINTS; i++) {
		for (int j = i+1; j < NPOINTS; j++) {
			// calculate the distance between the two points
			int dx = P[j].x - P[i].x;
			int dy = P[j].y - P[i].y;
			unsigned int length = sqrt((dx * dx) + (dy * dy));
			//printf("Edge #%i\tfrom i = P[%i] to j = P[%i]\tlength: %i\n", numEdges, i, j, length);

			Edge e = {P[i], P[j], length};
			Edge_List[numEdges] = e;
			numEdges++;
		}
	}

	// sort edge list in order of least-greatest edge length (takes nlogn time)
	std::sort(Edge_List, Edge_List + numEdges, [](const Edge& e1, const Edge& e2) {
		return e1.length < e2.length;
	});

	// this part will be used for printing all the lengths 
	for (Edge& e : Edge_List) {
		printf("length: %i\n", e.length);
	}
}

void initPoints() {
	int x = rand() % WIDTH;
	int y = rand() % HEIGHT;

	for (Point& p : P) {
		p.x = rand() % WIDTH;	// coordinate on width side
		p.y = rand() % HEIGHT;	// coordinate on height side
	}

	// from here you can  start to initialize the edges 
}

void drawPoints() {
	glClear(GL_COLOR_BUFFER_BIT);

	initPoints();
	
	// render the points
	glColor3f(0.0, 1.0, 0.0);	// Set dot color to green
	glPointSize(POINT_SIZE);
	glBegin(GL_POINTS);
	for (const Point& p : P)
		glVertex2i(p.x, p.y);
	glEnd();

	glutSwapBuffers();
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D - TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| R: Reset Points | D: Debug                                            |\n");
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
		drawPoints();
		break;
	case 'd':
	case 'D':
		initEdges();
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

	// Set seed for rand()
	srand(time(NULL));

	// glut functions
    //glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

	drawPoints();		// initialize all the points to be rendered 
	glutSetCursor(GLUT_CURSOR_NONE);	// makes mouse cursor disappear on glut 

    glutMainLoop();
    return 0;
}