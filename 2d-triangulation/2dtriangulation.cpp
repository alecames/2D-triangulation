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

// calculates the edges between the points
void calcEdges() {
	int numEdges = 0;
	for (int i = 0; i < POINT_COUNT; i++) {
		int nearest1 = -1;
		int nearest2 = -1;
		int shortestLen1 = 999999;
		int shortestLen2 = 999999;
		for (int j = 0; j < POINT_COUNT; j++) {
			if (i != j) {
				int dx = P[j].x - P[i].x;
				int dy = P[j].y - P[i].y;
				int dist = dx * dx + dy * dy;
				if (dist < shortestLen1) {
					shortestLen2 = shortestLen1;
					nearest2 = nearest1;
					shortestLen1 = dist;
					nearest1 = j;
				}
				else if (dist < shortestLen2) {
					shortestLen2 = dist;
					nearest2 = j;
				}
			}
		}
		Edge e1 = { P[i], P[nearest1], std::sqrt(shortestLen1) };
		EdgeList[numEdges++] = e1;
		Edge e2 = { P[i], P[nearest2], std::sqrt(shortestLen2) };
		EdgeList[numEdges++] = e2;
		printf("Edge #%i\tfrom (%i,%i) to (%i,%i)\tlength: %i\n", i, EdgeList[i].p1.x, EdgeList[i].p1.y, EdgeList[i].p2.x, EdgeList[i].p2.y, EdgeList[i].length);
	}
}

void calcEdges2() {
	
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