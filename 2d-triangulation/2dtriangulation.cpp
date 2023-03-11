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
// Alec Ames 68643577
// Julian Geronimo 
// COSC 3P98 - Computer Graphics - Assignment 2

// Immutable
const int WIDTH = 800;
const int HEIGHT = 800;
const float POINT_RADIUS = 6.0f;
const int POINT_COUNT = 10; // "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

struct Point {
	int x, y;
};

Point P[POINT_COUNT];

struct Edge {
	Point p1;
	Point p2;
	unsigned int length;
};

Edge Edge_List[POINT_COUNT * (POINT_COUNT - 1) / 2]; // number of edges calculated using nC2 = n(n-1) / 2

void initPoints() {
	int x = rand() % WIDTH;
	int y = rand() % HEIGHT;

	for (Point& p : P) {
		p.x = rand() % WIDTH;  // coordinate on width side
		p.y = rand() % HEIGHT; // coordinate on height side
	}

	// from here you can  start to initialize the edges
}

void calcEdges() {
	// calculate length, save to edge list;
	int numEdges = 0;

	for (int i = 0; i < POINT_COUNT; i++) {
		for (int j = i + 1; j < POINT_COUNT; j++) {
			// calculate the distance between the two points
			int dx = P[j].x - P[i].x;
			int dy = P[j].y - P[i].y;
			unsigned int length = sqrt((dx * dx) + (dy * dy));

			Edge e = { P[i], P[j], length };
			Edge_List[numEdges] = e;
			numEdges++;
		}
	}

	// sort edge list in order of least-greatest edge length (takes nlogn time)
	std::sort(Edge_List, Edge_List + numEdges, [](const Edge& e1, const Edge& e2)
		{ return e1.length < e2.length; });

	// print the shortest edges
	printf("Shortest edges:\n");
	for (int i = 0; i < POINT_COUNT - 1; i++) {
		printf("Edge #%i\tfrom (%i,%i) to (%i,%i)\tlength: %i\n", i, Edge_List[i].p1.x, Edge_List[i].p1.y, Edge_List[i].p2.x, Edge_List[i].p2.y, Edge_List[i].length);
	}
}

void drawPoints() {
	glClear(GL_COLOR_BUFFER_BIT);
	// render the points
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(POINT_RADIUS);
	glBegin(GL_POINTS);

	for (Point& p : P) {
		glVertex2i(p.x, p.y);
	}
	glEnd();

	glutSwapBuffers();
}

void drawEdges() {
	// render the edges
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	for (Edge& e : Edge_List) {
		glVertex2i(e.p1.x, e.p1.y);
		glVertex2i(e.p2.x, e.p2.y);
	}
	glEnd();

	glutSwapBuffers();
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D   TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| R: Reset Points | D: Draw Edges                                       |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (tolower(key))
	{ // convert to lowercase
	// quit with q
	case 'q':
		exit(0);
		break;
		// quit with esc
	case 27:
		exit(0);
		break;
		// reset display
	case 'r':
		initPoints();
		drawPoints();
		break;
	case 'd':
		calcEdges();
		drawEdges();
		break;
		// h - help
	case 'h':
		showcmds();
		break;
	}
}

// commenting this out because we might need this later, but I doubt it.
// void display(void) {
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
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2); // makes the window appear in the center of the screen
	glutCreateWindow("2D Triangulation");

	// initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT); // creates drawable projection

	// show print controls
	showcmds();

	// Set seed for rand()
	srand(time(NULL));

	// glut functions
	// glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	// initialize all the points to be rendered
	drawPoints();

	// initialize all the edges to be rendered

	glutMainLoop();
	return 0;
}