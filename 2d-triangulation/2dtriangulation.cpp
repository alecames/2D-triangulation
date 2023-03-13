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

// COSC 3P98 - Computer Graphics - Assignment 2
// This program will preform 2D triangulation on a set of points

// Alec Ames 		6843577
// Julian Geronimo 	6756597


const int WIDTH = 800;
const int HEIGHT = 800;
const float POINT_RADIUS = 6.0f;
const int POINT_COUNT = 30;

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
	bool connected(const Edge& other) const {
		return (p1 == other.p1 || p1 == other.p2 || p2 == other.p1 || p2 == other.p2);
	}
};

struct Triangle {
	Edge e1, e2, e3;
};

// Global (data) structures
std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT* (POINT_COUNT - 1) / 2);
std::set<Edge> TriEdge;
std::vector<Triangle> Triangles;
std::set<Edge> ConvexHull;

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
	int rows = std::sqrt(POINT_COUNT * HEIGHT / WIDTH);
	int cols = POINT_COUNT / rows;
	int xgap = WIDTH / cols;
	int ygap = HEIGHT / rows;

	// generates lattice points
	int index = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			float x = (j + 0.5) * xgap;
			float y = (i + 0.5) * ygap;
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
	printf("| Points: %i \n", POINT_COUNT);
	glEnd();
	glutSwapBuffers();
}

bool duplicateEdge(Edge L1, Edge L2) {
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

	// if they share the exact same points, they are duplicates
	if((xa == xc) && (ya == yc) && (xb == xd) && (yb == yd))	
		return true;

	return false;
}

bool checkConvexHull(Edge L1) {
	// iterate through all line segments in the convex hull, then confirm if L1 is an edge of the convex hull (is part of the set ConvexHull)
	for(Edge L2 : ConvexHull) {
		if(duplicateEdge(L1, L2))	// if they are the same
			return true;
	} 

	return false;
}


// initializes the set of Convex hulls based off all the edges in TriEdge
void initConvexHull() {
	// Using TriEdges and retrieving the size from TriEdge.size(), there should be (n*(n-1)/2 = approx(n*n)) amount of of edges 
	// for each edge defined by pairs (Pi, Pj) of points in P (n*(n-1)/2 = approx(n*n) of them)
	// compute signed (fast) distance d for all points(less Pi, Pj) from this edge
	// if all points have same sign (>0, <0, =0), then that pair might define an edge
	// Only add (Pi,Pj) if any point with d=0 is not in between these points (check X, Y between them)
	// --> add edge (Pi, Pj) to convex hull
}

// Calculates all the triangles and adds them to the Triangle structure
void calcTriangles() {
	initConvexHull();
	std::set <Edge> triangleDone;	// keep track of edges that form triangles

	// Step 4:
	for(Edge L1 : TriEdge) {

		// initial edge we want to find shortest edges towards
		Point pa = L1.p1;
		Point pb = L1.p2;
		int numTriangle = 0;
		bool isConvexHull = false;	// "is part of the convex hull", initially assume that the edge is inside the convex hull

		for(Edge L2 : TriEdge) {	// search through the set of edges; TriEdges is already sorted in increasing order by length
			if(duplicateEdge(L1, L2)) 	// if the edges are complete duplicates, do not check
				continue;

			// next closest line segment
			Point pc = L2.p1;
			Point pd = L2.p2;

			// Step a)
			if (pa == pc || pa == pd || pb == pc || pb == pd) {
                numTriangle++;
                if (numTriangle == 2 || checkConvexHull(L1)) {
                    triangleDone.insert(L1);
                    break;
                }
            }

			// Step b)
			// For each edge in TriEdge, search through TriEdge for 2 edges that both respectively connect to Pa and Pb and is connected by Pc
			// These points Pa Pb Pc are the points forming a triangle and is inserted into the Triangles set by Triangles.insert({Pa, Pb, Pc});
			// is added to Triangle data structure

			// Step c)
			// For each new triangle containing 3 points (Pa, Pb, Pc), search through TriEdge to find another 2 edges that connect to Pa and Pb and is connected by Pd
			// These points Pa Pb Pd are the points forming a 2nd triangle and is inserted into the Triangles set by Triangles.insert({Pa, Pb, Pd});
			// is added to Triangle data structure
		}
	}

	// Step d) : given that all the triangles edges to be removed from TriEdges is kepted inside the set triangleDone, we can perform the last step here
	for(Edge e : triangleDone) {	// loop through all edges with triangles
		TriEdge.erase(e);			// set.erase, targeted triangle edge
	}
}

void calcTriEdges() {
	// Step 2/3: iterate through all known edges, and for all edges that do not intersect with the set TriEdge, is added to that very same set. 
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
			// Distance between 2 points (x0, y0) and (x1, y1): d = sqrt((x1-x0)^2 + (y1-y0)^2)
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

	for (int i = 0; i < numEdges; i++) { // Not using for-each so we can keep track of iterations
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

			// if parallel, we skip
			if (D == 0) continue;

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
	printf("| Edges: %i \n", TriEdge.size());
	glEnd();
	glutSwapBuffers();
}

// c) triangle polygon extaction - extract triangle polygons from the edges
void extractTriangles() {
	for (Edge e1 : TriEdge) {
		for (Edge e2 : TriEdge) {
			for (Edge e3 : TriEdge) {
				// checks if e1, e2, e3 make triangle
				if (e1.p1 == e2.p1 && e1.p2 == e3.p1 && e2.p2 == e3.p2) {
					Triangle t = { e1, e2, e3 };
					Triangles.push_back(t);
					Triangles.resize(Triangles.size() + 1);
				}
			}
		}
	}
}

// c) render the triangles as solid polygons
void drawTriangles() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(0.0f);
	glBegin(GL_TRIANGLES);
	for (const Triangle& t : Triangles) {
		float r = float(rand()) / RAND_MAX;
		float g = float(rand()) / RAND_MAX;
		float b = float(rand()) / RAND_MAX;
		glColor3f(r, g, b);
		glVertex2i(t.e1.p1.x, t.e1.p1.y);
		glVertex2i(t.e1.p2.x, t.e1.p2.y);
		glVertex2i(t.e2.p2.x, t.e2.p2.y);
	}
	// c) count and print the number of triangles created
	printf("| Triangles: %i \n", Triangles.size());
	glEnd();
	glutSwapBuffers();
}

// Function to help determine if 2 triangles share the same edge
bool sharedEdge(Triangle t1, Triangle t2, Edge& shared_edge) {
    // Check if any two edges are identical
    if (duplicateEdge(t1.e1, t2.e1)) {
        shared_edge = t1.e1;
        return true;
    }
    if (duplicateEdge(t1.e1, t2.e2)) {
        shared_edge = t1.e1;
        return true;
    }
    if (duplicateEdge(t1.e1, t2.e3)) {
        shared_edge = t1.e1;
        return true;
    }
    if (duplicateEdge(t1.e2, t2.e1)) {
        shared_edge = t1.e2;
        return true;
    }
    if (duplicateEdge(t1.e2, t2.e2)) {
        shared_edge = t1.e2;
        return true;
    }
    if (duplicateEdge(t1.e2, t2.e3)) {
        shared_edge = t1.e2;
        return true;
    }
    if (duplicateEdge(t1.e3, t2.e1)) {
        shared_edge = t1.e3;
        return true;
    }
    if (duplicateEdge(t1.e3, t2.e2)) {
        shared_edge = t1.e3;
        return true;
    }
    if (duplicateEdge(t1.e3, t2.e3)) {
        shared_edge = t1.e3;
        return true;
    }
    // No shared edges found
	shared_edge = t1.e1;
    return false;
}

// d) cleanup Implement the cleanup algorithm discussed in class
void cleanup() {
	bool change = true;
	// repeatedly optimize each triangle 
	while(change) {
		// find 2 triangles that share an edge
		for(int i = 0; i < Triangles.size(); i++) {
			for(int j = i + 1;  j < Triangles.size(); j++) {
				Triangle t1 = Triangles[i];
				Triangle t2 = Triangles[j];

				Edge shared_edge;
				int d1, d2;
				// determine the distance of d1 and d2:
				// d1: the edge length of the shared edge between both triangles (L1 can be used here)
				// if: there exists a sharedEdge() between t1 and t2, then we can obtain what the shared edge is exactly and use .length to retrieve the length for d1

				// d2: the edge length of the 2 points of both L1 & L2 that does not intersect at Pa or Pb endpoints (the opposite edge from d1)
				// if: obtain the 2 points that do not have an edge together and measure the length of it, which will then become the value for d2

				// if: |d2| < |d1| AND the d1_edge and d2_edge are opposite sides of a line, then we found an optimized triangle to use
				// else: change = false as there are no more optimizations to make 
				change = false;

				// replace triangles so that the shared edge is instead the same edge used to obtain d2, and the previous shared edge is gone
			}
		}
	}	
}

// prints controls to terminal
void showcmds() {
	printf("|------------------------------------------------------------------------------|\n");
	printf("| H: Help                      2D   TRIANGULATION                ESC / Q: Quit |\n");
	printf("|------------------------------------------------------------------------------|\n");
	printf("| 1: Draw Points | 2: Draw Edges | 3: Draw Polygons | 4: Clean Up | L: Lattice |\n");
	printf("|------------------------------------------------------------------------------|\n");
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
	case 'l': // 1st step - generate points (lattice)
		initLatticePoints();
		drawPoints();
		glutPostRedisplay();
		break;
	case '2': // 2nd step - generate edges
		TriEdge.clear();
		calcTriEdges();
		drawEdges();
		glutPostRedisplay();
		break;
	case '3': // 3rd step - extract triangles
		Triangles.clear();
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
	glutSwapBuffers();

	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutMainLoop();
	return 0;
}