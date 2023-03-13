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
const int POINT_COUNT = 8; // "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

struct Point {
	float x, y;
	bool operator==(const Point& other) const { return (x == other.x) && (y == other.y); }
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
	bool operator==(const Edge& other) const { return (p1 == other.p1) && (p2 == other.p2) && (length == other.length); }
	float slope() const { return (float)(p2.y - p1.y) / (float)(p2.x - p1.x); }
	bool intersects(Edge& other) {
		// check if the two lines are parallel
		if (slope() == other.slope())
			return false;

		// vertical lines
		if (p1.x == p2.x || other.p1.x == other.p2.x)
			return false;

		// check if the two lines are horizontal
		if (p1.y == p2.y || other.p1.y == other.p2.y)
			return false;

		// check if the two lines intersect
		float x = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.x - other.p2.x) - (p1.x - p2.x) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));
		float y = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));

		// check if the intersection point is within the bounds of the two lines
		if (x < std::min(p1.x, p2.x) || x > std::max(p1.x, p2.x))
			return false;
		if (x < std::min(other.p1.x, other.p2.x) || x > std::max(other.p1.x, other.p2.x))
			return false;
		if (y < std::min(p1.y, p2.y) || y > std::max(p1.y, p2.y))
			return false;
		if (y < std::min(other.p1.y, other.p2.y) || y > std::max(other.p1.y, other.p2.y))
			return false;

		return true;
	}

	Point intersection(const Edge& other) const {
		// check if the two lines are parallel 
		if (slope() == other.slope())
			return Point{ -1, -1 };

		// vertical lines
		if (p1.x == p2.x || other.p1.x == other.p2.x)
			return Point{ -1, -1 };

		// check if the two lines are horizontal
		if (p1.y == p2.y || other.p1.y == other.p2.y)
			return Point{ -1, -1 };

		// check if the two lines intersect
		float x = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.x - other.p2.x) - (p1.x - p2.x) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));
		float y = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));

		// check if the intersection point is within the bounds of the two lines
		if (x < std::min(p1.x, p2.x) || x > std::max(p1.x, p2.x))
			return Point{ -1, -1 };
		if (x < std::min(other.p1.x, other.p2.x) || x > std::max(other.p1.x, other.p2.x))
			return Point{ -1, -1 };
		if (y < std::min(p1.y, p2.y) || y > std::max(p1.y, p2.y))
			return Point{ -1, -1 };
		if (y < std::min(other.p1.y, other.p2.y) || y > std::max(other.p1.y, other.p2.y))
			return Point{ -1, -1 };

		return Point{ float(x), float(y) };
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

};

std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT* (POINT_COUNT - 1) / 2);
std::set<Edge> TriEdge;
std::vector<Triangle> Triangles(999);

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
	glEnd();
	glutSwapBuffers();
}

// ! temp function for debugging
void printEdge(const Edge& e) {
    printf("\tp1.x: %f    p1.y: %f\n", e.p1.x, e.p1.y);
    printf("\tp2.x: %f    p2.y: %f\n", e.p2.x, e.p2.y);
    // printf("\tlength: %d\n", e.length);
}

// ! not in use
// returns true if 2 line segments intersect at their start/endpoints
bool atEndPoint(int xa, int ya, int xb, int yb, int xc, int yc, int xd, int yd) {
	if (xa == xc && ya == yc) { // check if L1 start point matches L2 start point
        return true;
    }
    if (xa == xd && ya == yd) { // check if L1 start point matches L2 end point
        return true;
    }
    if (xb == xc && yb == yc) { // check if L1 end point matches L2 start point
        return true;
    }
    if (xb == xd && yb == yd) { // check if L1 end point matches L2 end point
        return true;
    }
    return false; // no end points matched
}

void calcTriangles() {
	// Step 4:
	
}

void calcTriEdges() {
	// Step 2/3: iterate through all known edges, and for all edges that do not intersect with the set TriEdge, is added to that very same set. 

	int numEdges = EdgeList.size();
	bool intersection;
	TriEdge.clear();

	for (int i = 0; i < numEdges; i++) {	// Not using for-each so we can keep track of iterations
		Edge L1 = EdgeList[i];
		intersection = false; // initially assume edge encounters no intersections
		// printf("==== FOR [i : %i] ====\n", i);
		
		// compare L1 with each L2 in Tri Edge
		for (Edge L2 : TriEdge) {
			// if (L1 == L2) {	// * find duplicate
			// 	// printf("Duplicate: L1 == L2 at [i: %i]\n", i);	// ! debugging
			// 	continue;
			// }

			// Line Segment 1 (L1)
			Point pointA = L1.p1;
			float xa = pointA.x;
			float ya = pointA.y;
			Point pointB = L1.p2;
			float xb = pointB.x;
			float yb = pointB.y;

			// Line Segment 2 (L2)
			Point pointC = L2.p1;
			float xc = pointC.x;
			float yc = pointC.y;
			Point pointD = L2.p2;
			float xd = pointD.x;
			float yd = pointD.y;

			setx.insert(xa);
			setx.insert(xb);
			setx.insert(xc);
			setx.insert(xd);

			sety.insert(ya);
			sety.insert(yb);
			sety.insert(yc);
			sety.insert(yd);


			float D = ((xb - xa) * (yd - yc)) - ((yb - ya) * (xd - xc));
			// printf("D: %f\n", D);

			if(D == 0) {	// if parallel, we skip
				printf("Lines are parallel: D == 0\n");
				continue;
			}

			float ta = (((xc - xa) * (yd - yc)) - ((yc - ya) * (xd - xc))) / D;
			float tb = (xa - xc + (xb - xa) * ta) / (xd - xc);
			// printf("[ta: %f]  [tb: %f]\n", ta, tb);

			// printf("L1");
			// printEdge(L1);
			// printf("L2");
			// printEdge(L2);

			if((ta == 0 || ta == 1) && (tb == 0 || tb == 1)) {
    			// printf("Intersects at start/end point. ADD EDGE\n");
			}
			else if((ta >= 0 && ta <= 1) && (tb >= 0 && tb <= 1)) {
				// printf("[ta: %f]  [tb: %f]\n", ta, tb);
				// printf("L1");
				// printEdge(L1);
				// printf("L2");
				// printEdge(L2);
				// printf("Intersects somewhere (not start/endpoint). IGNORE EDGE\n");
				intersection = true;
			}

		} // for-each
		if (!intersection) { // if there is no intersection, then we can add it to TriEdge
			TriEdge.insert(L1);
		}
		// printf("\n");

	} // for

	// b) print number of TriEdges
	printf("# of TriEdges: %i\n", TriEdge.size());
	// Step 2/3: done

	calcTriangles();
}

// b) naive triangulation algorithm
void calcEdges() {
	// calculate length, save to edge list;
	int numEdges = 0;

	// Step 1: find all possible edges
	// printf("\t\t\t<> Finding ALL possible edges <>\n");
	for (int i = 0; i < POINT_COUNT; i++) {
		for (int j = i + 1; j < POINT_COUNT; j++) {
			// calculate the distance between the two points
			// Point 1
			int x0 = P[i].x;
			int y0 = P[i].y;
			// Point 2
			int x1 = P[j].x;
			int y1 = P[j].y;
			// Distance between 2 points (x0, y0) and (x1, y1): d = sqrt((x1-x0)^2 + (y1-y0)^2)
			unsigned int length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

			// b) save the triangles in a data structure
			Edge e = { P[i], P[j], length };
			EdgeList[numEdges] = e;
			numEdges++;
		}
	}
	// b) count and print the number of edges created
	printf("# of Edges: %i\n", numEdges);
	std::sort(EdgeList.begin(), EdgeList.end());	// sort edge vector in order of least-greatest edge length (takes nlogn time)
	// Step 1: done

	calcTriEdges();
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
	glEnd();
	glutSwapBuffers();
}

// c) triangle polygon extaction - extract triangle polygons from the edges
void extractTriangles() {
	std::set<Edge> edgeSet(EdgeList.begin(), EdgeList.end());

	int numTriangles = 0;

	for (auto it = edgeSet.begin(); it != edgeSet.end(); ++it) {
		Edge e1 = *it;
		auto jt = it;
		for (++jt; jt != edgeSet.end(); ++jt) {
			Edge e2 = *jt;
			if (e1.intersects(e2)) {
				Point p = e1.intersection(e2);
				if (p != e1.p1 && p != e1.p2 && p != e2.p1 && p != e2.p2) {
					Triangle t = { e1.p1, e1.p2, p };
					Triangles[numTriangles] = t;
					numTriangles++;
					t = { e1.p1, e2.p1, p };
					Triangles[numTriangles] = t;
					numTriangles++;
					t = { e1.p2, e2.p2, p };
					Triangles[numTriangles] = t;
					numTriangles++;
				}
			}
		}
	}

	printf("Triangles: %i\n", numTriangles);
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
	glEnd();
	glutSwapBuffers();
}

// d) cleanup Implement the cleanup algorithm discussed in class. It is invoked by a "cleanup" command key (or menu item). It will take the original triangulation from (c), and repeatedly optimize pairs of triangles with shared edges.
// void cleanup() {
//     // iterate over all triangles
//     for (int i = 0; i < Triangles.size(); i++) {
//         Triangle& t1 = Triangles[i];
//         for (int j = i + 1; j < Triangles.size(); j++) {
//             Triangle& t2 = Triangles[j];
//             // check if triangles share an edge
//             int sharedEdgeCount = 0;
//             for (int k = 0; k < 3; k++) {
//                 for (int l = 0; l < 3; l++) {
//                     // if (t1.edges[k] == t2.edges[l]) {
//                     //     sharedEdgeCount++;
//                     // }
//                 }
//             }
//             // if they share exactly one edge, try to optimize
//             if (sharedEdgeCount == 1) {
//                 Edge sharedEdge;
//                 // find shared edge
//                 for (int k = 0; k < 3; k++) {
//                     for (int l = 0; l < 3; l++) {
//                         // if (t1.edges[k] == t2.edges[l]) {
//                         //     sharedEdge = t1.edges[k];
//                         //     break;
//                         // }
//                     }
//                     if (sharedEdge.p1.x != -1) {
//                         break;
//                     }
//                 }
//             }
//         }
//     }
//     // redraw triangles and edges
//     drawEdges();
//     drawTriangles();
// }

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
		// initPoints();
		// drawPoints();
		glutPostRedisplay();
		break;
	case 'd':
		initPoints();
		drawPoints();
		calcEdges();
		drawEdges();
		// glutPostRedisplay();
		break;
	case 't':
		initPoints();
		calcEdges();
		extractTriangles();
		drawTriangles();
		glutPostRedisplay();
		break;
	case 'c':
		break;
	case 'h': // h - help
		showcmds();
		break;
	default:
		break;
	}
}

void display(void) {
	// glClear(GL_COLOR_BUFFER_BIT);
	// // glutSwapBuffers();
	// glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2); // makes the window appear in the center of the screen
	glutCreateWindow("2D Triangulation");

	// glutCreateMenu(menu);
	// glutAddMenuEntry("Reset Points", 1);
	// glutAddMenuEntry("Draw Edges", 2);
	// glutAddMenuEntry("Draw Triangles", 3);
	// glutAddMenuEntry("Quit", 4);
	// glutAttachMenu(GLUT_RIGHT_BUTTON);

	// initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT); // creates drawable projection

	// show print controls
	showcmds();

	// Set seed for rand()
	srand(time(NULL));

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	// initialize all the points to be rendered
	drawPoints();

	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutMainLoop();
	return 0;
}