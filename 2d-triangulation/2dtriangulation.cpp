#include <freeglut.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>
#include <set>

// This program will preform 2D triangulation on a set of points

// Alec Ames 68643577
// Julian Geronimo 6756597
// COSC 3P98 - Computer Graphics - Assignment 2

const int WIDTH = 800;
const int HEIGHT = 800;
const float POINT_RADIUS = 6.0f;
const int POINT_COUNT = 10;

struct Point {
	int x, y;
	bool operator==(const Point& other) const { return x == other.x && y == other.y; }
	bool operator!=(const Point& other) const { return x != other.x || y != other.y; }
	bool operator<(const Point& other) const { return x < other.x || (x == other.x && y < other.y); }
	bool operator>(const Point& other) const { return x > other.x || (x == other.x && y > other.y); }
};

struct Edge {
	Point p1;
	Point p2;
	unsigned int length;
	bool operator<(const Edge& other) const { return length < other.length; }
	bool operator>(const Edge& other) const { return length > other.length; }
	bool operator==(const Edge& other) const { return p1 == other.p1 && p2 == other.p2; }
	bool operator!=(const Edge& other) const { return p1 != other.p1 || p2 != other.p2; }
	float slope() const {
		if (p2.x == p1.x) { return std::numeric_limits<float>::infinity(); }
		return (float)(p2.y - p1.y) / (float)(p2.x - p1.x);
	}
	bool connected(const Edge& other) const { return p1 == other.p1 || p1 == other.p2 || p2 == other.p1 || p2 == other.p2; }
	bool intersects(Edge& other) {
		if (slope() == other.slope()) return false;
		if (p1.x == p2.x || other.p1.x == other.p2.x) return false;
		if (p1.y == p2.y || other.p1.y == other.p2.y) return false;

		// check if the two lines intersect
		float x = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.x - other.p2.x) - (p1.x - p2.x) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));
		float y = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));

		// check if the intersection point is within the bounds of the two lines
		if (x < std::min(p1.x, p2.x) || x > std::max(p1.x, p2.x)) return false;
		if (x < std::min(other.p1.x, other.p2.x) || x > std::max(other.p1.x, other.p2.x)) return false;
		if (y < std::min(p1.y, p2.y) || y > std::max(p1.y, p2.y)) return false;
		if (y < std::min(other.p1.y, other.p2.y) || y > std::max(other.p1.y, other.p2.y)) return false;
		return true;
	}

	Point intersect(const Edge& other) const {
		// check if the two lines are parallel
		if (slope() == other.slope()) return Point{ -1, -1 };
		if (p1.x == p2.x || other.p1.x == other.p2.x) return Point{ -1, -1 };

		// check if the two lines are horizontal
		if (p1.y == p2.y || other.p1.y == other.p2.y) return Point{ -1, -1 };

		// check if the two lines intersect
		float x = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.x - other.p2.x) - (p1.x - p2.x) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));
		float y = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));

		// check if the intersection point is within the bounds of the two lines
		if (x < std::min(p1.x, p2.x) || x > std::max(p1.x, p2.x)) return Point{ -1, -1 };
		if (x < std::min(other.p1.x, other.p2.x) || x > std::max(other.p1.x, other.p2.x)) return Point{ -1, -1 };
		if (y < std::min(p1.y, p2.y) || y > std::max(p1.y, p2.y)) return Point{ -1, -1 };
		if (y < std::min(other.p1.y, other.p2.y) || y > std::max(other.p1.y, other.p2.y)) return Point{ -1, -1 };
		return Point{ int(x), int(y) };
	}
};

struct Triangle {
	Point p1;
	Point p2;
	Point p3;	bool operator==(const Triangle& other) const { return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) || (p1 == other.p1 && p2 == other.p3 && p3 == other.p2) || (p1 == other.p2 && p2 == other.p1 && p3 == other.p3) || (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) || (p1 == other.p3 && p2 == other.p1 && p3 == other.p2) || (p1 == other.p3 && p2 == other.p2 && p3 == other.p1); }
	bool operator!=(const Triangle& other) const { return !(*this == other); }
	bool operator<(const Triangle& other) const { return p1 < other.p1 || (p1 == other.p1 && p2 < other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 < other.p3); }
	bool operator>(const Triangle& other) const { return p1 > other.p1 || (p1 == other.p1 && p2 > other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 > other.p3); }
	// check if triangle intersects with another triangle
	// TODO: needs to be tweaked to allow for adjacent triangles
	bool intersects(const Triangle& other) const {
		Edge e1 = { p1, p2 };
		Edge e2 = { p2, p3 };
		Edge e3 = { p3, p1 };
		Edge e4 = { other.p1, other.p2 };
		Edge e5 = { other.p2, other.p3 };
		Edge e6 = { other.p3, other.p1 };
		if (e1.intersects(e4) || e1.intersects(e5) || e1.intersects(e6) ||
			e2.intersects(e4) || e2.intersects(e5) || e2.intersects(e6) ||
			e3.intersects(e4) || e3.intersects(e5) || e3.intersects(e6)) {
			return true;
		}
		return false;
	}
};

// global variables
std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT* (POINT_COUNT - 1) / 2);
std::set<Edge> TriEdge;
std::vector<Triangle> Triangles;

// a) generate N random unique points on the plane
void initPoints() {
	std::set<Point> S;
	while (S.size() < POINT_COUNT) {
		int x = rand() % WIDTH;
		int y = rand() % HEIGHT;
		Point p = { x, y };
		if (S.find(p) == S.end()) {
			S.insert(p);
		}
	}

	// saves to points vector
	int i = 0;
	for (const auto& p : S) {
		P[i] = p;
		i++;
	}
}

// a) render these points
void drawPoints() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	glPointSize(POINT_RADIUS);
	glBegin(GL_POINTS);
	for (Point& p : P) {
		glVertex2i(p.x, p.y);
	}
	printf("Points: %d \n", POINT_COUNT);
	glEnd();
	glutSwapBuffers();
}

// b) naive triangulation algorithm
void calcEdges() {
	int i = 0;
	for (int j = 0; j < POINT_COUNT; j++) {
		for (int k = j + 1; k < POINT_COUNT; k++) {
			Edge e = { P[j], P[k] };
			EdgeList[i] = e;
			i++;
		}
	}
}

// c) triangle polygon extaction - extract triangle polygons from the edges
void extractPolygons() {
	int i = 0;
	for (int j = 0; j < POINT_COUNT; j++) {
		for (int k = j + 1; k < POINT_COUNT; k++) {
			for (int l = k + 1; l < POINT_COUNT; l++) {
				Triangle t = { P[j], P[k], P[l] };
				// set size of Triangles vector
				if (i == 0) {
					Triangles.resize(POINT_COUNT * (POINT_COUNT - 1) * (POINT_COUNT - 2) / 6);
				}
				Triangles[i] = t;
				i++;
			}
		}
	}
}

// removes triangles that overlap with other triangles
void cleanup() {
	for (int i = 0; i < Triangles.size(); i++) {
		for (int j = i + 1; j < Triangles.size(); j++) {
			if (Triangles[i].intersects(Triangles[j])) {
				Triangles.erase(Triangles.begin() + j);
				j--;
			}
		}
	}
}

// b) render the edges
void drawEdges() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.3, 0.72, 0.56);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (Edge& e : EdgeList) {
		glVertex2i(e.p1.x, e.p1.y);
		glVertex2i(e.p2.x, e.p2.y);
	}
	printf("Edges: %d \n", EdgeList.size());
	glEnd();
	glutSwapBuffers();
}

// c) render the triangles as solid polygons
void drawTriangles() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(2.0f);
	// get random color
	glBegin(GL_TRIANGLES);
	for (Triangle& t : Triangles) {
		float r = float(rand()) / RAND_MAX;
		float g = float(rand()) / RAND_MAX;
		float b = float(rand()) / RAND_MAX;
		glColor3f(r, g, b);
		glVertex2i(t.p1.x, t.p1.y);
		glVertex2i(t.p2.x, t.p2.y);
		glVertex2i(t.p3.x, t.p3.y);
	}
	printf("Triangles: %d \n", Triangles.size());
	glEnd();
	glutSwapBuffers();
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D   TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| 1: Generate Points | 2: Draw Edges | 3: Create Polygons | 4: Clean Up |\n");
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
	case '1': // 1st step - generate points
		initPoints();
		drawPoints();
		glutPostRedisplay();
		break;
	case '2': // 2nd step - generate edges
		calcEdges();
		drawEdges();
		glutPostRedisplay();
		break;
	case '3': // 3rd step - extract triangles
		extractPolygons();
		drawTriangles();
		glutPostRedisplay();
		break;
	case '4': // 4th step - cleanup
		cleanup();
		drawTriangles();
		glutPostRedisplay();
		break;
	case 'h': // h - help
		showcmds();
		break;
	default:
		break;
	}
}

void display(void) {

}

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

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutSwapBuffers();

	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutMainLoop();
	return 0;
}