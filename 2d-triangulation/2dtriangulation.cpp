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
const int POINT_COUNT = 100; // "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

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
		// check if the two lines are parallel
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
		if (p1.y == p2.y || other.p1.y == other.p2.y) return Point{ -1, -1 };
		
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
	Point p3;
	bool operator==(const Triangle& other) const { return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) || (p1 == other.p1 && p2 == other.p3 && p3 == other.p2) || (p1 == other.p2 && p2 == other.p1 && p3 == other.p3) || (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) || (p1 == other.p3 && p2 == other.p1 && p3 == other.p2) || (p1 == other.p3 && p2 == other.p2 && p3 == other.p1); }
	bool operator!=(const Triangle& other) const { return !(*this == other); }
	bool operator<(const Triangle& other) const { return p1 < other.p1 || (p1 == other.p1 && p2 < other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 < other.p3); }
	bool operator>(const Triangle& other) const { return p1 > other.p1 || (p1 == other.p1 && p2 > other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 > other.p3); }
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

std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT* (POINT_COUNT - 1) / 2);
std::set<Edge> TriEdge;
std::set<Triangle> Triangles;

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

// e) generate lattice points
void initLatticePoints() {
    int numRows = std::sqrt(POINT_COUNT * HEIGHT / WIDTH);  // number of rows in the grid
    int numCols = POINT_COUNT / numRows;  // number of columns in the grid
    int xgap = WIDTH / numCols;  // spacing between columns
    int ygap = HEIGHT / numRows;  // spacing between rows
    
    // generates lattice points
    int index = 0;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            int x = (j + 0.5) * xgap;
            int y = (i + 0.5) * ygap;
            P[index++] = { x, y };
        }
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
	printf("Points: %i \n", POINT_COUNT);
	glEnd();
	glutSwapBuffers();
}

// b) naive triangulation algorithm
void calcEdges() {
	// calculate length, save to edge list;
	int numEdges = 0;

	// Step 1: find all possible edges
	for (int i = 0; i < POINT_COUNT; i++) {
		for (int j = i + 1; j < POINT_COUNT; j++) {
			// calculate the distance between the two points
			// Point 1
			int x0 = P[i].x;
			int y0 = P[i].y;
			// Point 2
			int x1 = P[j].x;
			int y1 = P[j].y;
			// distance formula
			unsigned int length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

			// b) save the triangles in a data structure
			Edge e = { P[i], P[j], length };
			EdgeList[numEdges] = e;
			numEdges++;
		}
	}

	// sort edge vector in order of least-greatest edge length (takes nlogn time)
	std::sort(EdgeList.begin(), EdgeList.end());
	
	bool intersection;
	TriEdge.clear();

	for (int i = 0; i < numEdges; i++) {	// Not using for-each so we can keep track of iterations
		Edge L1 = EdgeList[i];
		intersection = false; // initially assume edge encounters no intersections

		// compare L1 with each L2 in Tri Edge
		for (Edge L2 : TriEdge) {
			
			// Line Segment 1 (L1)
			Point pointA = L1.p1;
			int xa = pointA.x;
			int ya = pointA.y;
			Point pointB = L1.p2;
			int xb = pointB.x;
			int yb = pointB.y;

			// Line Segment 2 (L2)
			Point pointC = L2.p1;
			int xc = pointC.x;
			int yc = pointC.y;
			Point pointD = L2.p2;
			int xd = pointD.x;
			int yd = pointD.y;

			float D = ((xb - xa) * (yd - yc)) - ((yb - ya) * (xd - xc));

			if (D == 0) {	// if parallel, we skip
				continue;
			}

			float ta = (((xc - xa) * (yd - yc)) - ((yc - ya) * (xd - xc))) / D;
			float tb = (xa - xc + (xb - xa) * ta) / (xd - xc);

			if ((ta == 0 || ta == 1) && (tb == 0 || tb == 1)) {
			}
			else if ((ta >= 0 && ta <= 1) && (tb >= 0 && tb <= 1)) {
				intersection = true;
				break;
			}

		} 
		if (!intersection) { // if there is no intersection, then we can add it to TriEdge
			TriEdge.insert(L1);
		}

	} 
}

// b) render the edges
void drawEdges() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.3, 0.72, 0.56);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (const Edge e : TriEdge) {
		glVertex2i(e.p1.x, e.p1.y);
		glVertex2i(e.p2.x, e.p2.y);
	}
	// b) count and print the number of edges created
	printf("Edges: %i \n", TriEdge.size());
	glEnd();
	glutSwapBuffers();
}

// c) triangle polygon extaction - extract triangle polygons from the edges
void extractTriangles() {
	// create triangles from the TriEdge set
	Triangles.clear();
	for (const Edge e1 : TriEdge) {
		for (const Edge e2 : TriEdge) {
			if (e1 == e2) continue;
			if (e1.p1 == e2.p1) {
				Triangle t = { e1.p2, e1.p1, e2.p2 };
				Triangles.insert(t);
			}
			else if (e1.p1 == e2.p2) {
				Triangle t = { e1.p2, e1.p1, e2.p1 };
				Triangles.insert(t);
			}
			else if (e1.p2 == e2.p1) {
				Triangle t = { e1.p1, e1.p2, e2.p2 };
				Triangles.insert(t);
			}
			else if (e1.p2 == e2.p2) {
				Triangle t = { e1.p1, e1.p2, e2.p1 };
				Triangles.insert(t);
			}
		}
	}
}

// c) render the triangles as solid polygons
void drawTriangles() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(0.0f);
	// get random color
	glBegin(GL_TRIANGLES);
	for (const Triangle& t : Triangles) {
		float r = float(rand()) / RAND_MAX;
		float g = float(rand()) / RAND_MAX;
		float b = float(rand()) / RAND_MAX;
		glColor3f(r, g, b);
		glVertex2i(t.p1.x, t.p1.y);
		glVertex2i(t.p2.x, t.p2.y);
		glVertex2i(t.p3.x, t.p3.y);
	}
	// c) count and print the number of triangles created
	printf("Triangles: %i \n", Triangles.size());
	glEnd();
	glutSwapBuffers();
}

// d) cleanup Implement the cleanup algorithm discussed in class
void cleanup() {

}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D   TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| 1: Draw Points   | 2: Draw Edges   | 3: Draw Polygons   | 4: Clean Up |\n");
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
	case 't': // 1st step - generate points (lattice)
		initLatticePoints();
		drawPoints();
		glutPostRedisplay();
		break;
	case '2': // 2nd step - generate edges
		calcEdges();
		drawEdges();
		glutPostRedisplay();
		break;
	case '3': // 3rd step - extract triangles
		extractTriangles();
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

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutSwapBuffers();

	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutMainLoop();
	return 0;
}