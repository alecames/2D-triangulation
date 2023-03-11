#include <freeglut.h>
#include <FreeImage.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>

// This program will preform 2D triangulation on a set of points

// Alec Ames 68643577
// Julian Geronimo 6756597
// COSC 3P98 - Computer Graphics - Assignment 2

const int WIDTH = 800;
const int HEIGHT = 800;
const float POINT_RADIUS = 6.0f;
const int POINT_COUNT = 10; // "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

struct Point {
	float x, y;
};

struct Edge {
	Point p1;
	Point p2;
	unsigned int length;
};

struct Triangle {
	Point p1;
	Point p2;
	Point p3;
};

std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT*(POINT_COUNT-1) / 2);
std::vector<Triangle> Triangles;


// formats the prints with the table edges
void padprint(const char* str) {
	int len = strlen(str);
	int pad = 70 - len;
	printf("| ");
	printf("%s", str);
	for (int i = 0; i < pad; i++)
		printf(" ");
	printf("|\n");
}


void initPoints() {
	for (int i = 0; i < POINT_COUNT; i++) {
		P[i].x = rand() % WIDTH;
		P[i].y = rand() % HEIGHT;
	}
}

// calculates the edges between the points
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

void calcEdges() {
	// calculate length, save to edge list;
	int numEdges = 0;

	for (int i = 0; i < POINT_COUNT; i++) {
		for (int j = i+1; j < POINT_COUNT; j++) {
			// calculate the distance between the two points
			int dx = P[j].x - P[i].x;
			int dy = P[j].y - P[i].y;
			unsigned int length = sqrt((dx * dx) + (dy * dy));
			// printf("Edge #%i from i = P[%i] to j = P[%i]\tlength: %i\n", numEdges, i, j, length);
			Edge e = {P[i], P[j], length};
			EdgeList[numEdges] = e;		// load all possible edges into EdgeList array
			numEdges++;
		}
	}

	printf("Edges: %i\n", numEdges);
	
	// sort edge vector in order of least-greatest edge length (takes nlogn time)
	std::sort(EdgeList.begin(), EdgeList.end(), [](const Edge& a, const Edge& b) {
		return a.length < b.length;
	});

	// this part will be used for printing all the lengths 
	for (Edge& e : EdgeList) {
		printf("length: %i\n", e.length);
	}

	// (FILTER) Find all edges with intersections and disregard them when inserting edges into TriList
}

void drawEdges() {
	// render the edges
	glColor3f(0.3, 0.72, 0.56);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (Edge& e : EdgeList) {
		glVertex2i(e.p1.x, e.p1.y);
		glVertex2i(e.p2.x, e.p2.y);
	}
	glEnd();
	glutSwapBuffers();
	
}

// menu
void menu(int id) {
	switch (id) {
	case 1:
		initPoints();
		drawPoints();
		break;
	case 2:
		calcEdges();
		drawEdges();
		break;
	case 3:
		exit(0);
		break;
	}
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D   TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| R: Reset Points                                         D: Draw Edges |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'q': // quit with q
		exit(0);
		break;
	case 27: // quit with esc
		exit(0);
		break;
	case 'r': // reset display
		initPoints();
		drawPoints();
		break;
	case 'd':
		calcEdges();
		drawEdges();
		break;
	case 'h': // h - help
		showcmds();
		break;
	case 't':
		calcEdges2();
		break;
	}
}

// // commenting this out because we might need this later, but I doubt it.
// void display(void) {
//    glClear(GL_COLOR_BUFFER_BIT);

// 	// Set dot color to white
// 	glColor3f(1.0, 1.0, 1.0);
// 	drawPoints(10);	// argument should be some number greater than 3 so that atleast one triangle can be formed
// 					// ... unless we don't wanna do that (?)

//    glutSwapBuffers();
// }

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2); // makes the window appear in the center of the screen
	glutCreateWindow("2D Triangulation");

	glutCreateMenu(menu);
	glutAddMenuEntry("Reset Points", 1);
	glutAddMenuEntry("Draw Edges", 2);
	glutAddMenuEntry("Quit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

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
	glutKeyboardFunc(keyboard);

	// initialize all the points to be rendered
	drawPoints();

	glEnable(GL_BLEND); // attempt to smooth out points 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	// initialize all the edges to be rendered

	glutMainLoop();
	return 0;
}