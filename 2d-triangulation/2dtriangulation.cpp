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
#include <set>

// This program will preform 2D triangulation on a set of points

// Alec Ames 68643577
// Julian Geronimo 6756597
// COSC 3P98 - Computer Graphics - Assignment 2

const int WIDTH = 800;
const int HEIGHT = 800;
const float POINT_RADIUS = 6.0f;
const int POINT_COUNT = 4; // "n-points", hardcoded for now, maybe we can ask the user how many they want; -/+ an amount.

struct Point
{
	int x, y;
	bool operator==(const Point &other) const { return x == other.x && y == other.y; }
	bool operator!=(const Point &other) const { return x != other.x || y != other.y; }
	bool operator<(const Point &other) const { return x < other.x || (x == other.x && y < other.y); }
	bool operator>(const Point &other) const { return x > other.x || (x == other.x && y > other.y); }
};

struct Edge
{
	Point p1;
	Point p2;
	unsigned int length;
	bool operator<(const Edge &other) const { return length < other.length; }
	bool operator>(const Edge &other) const { return length > other.length; }
	bool operator==(const Edge &other) const { return p1 == other.p1 && p2 == other.p2; }
	float slope() const { return (float)(p2.y - p1.y) / (float)(p2.x - p1.x); }
	bool intersects(Edge &other)
	{
		// check if the two lines are parallel
		if (slope() == other.slope())
			return false;

		// check if the two lines are vertical
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

	Point intersection(const Edge &other) const
	{
		// check if the two lines are parallel
		if (slope() == other.slope())
			return Point{-1, -1};

		// check if the two lines are vertical
		if (p1.x == p2.x || other.p1.x == other.p2.x)
			return Point{-1, -1};

		// check if the two lines are horizontal
		if (p1.y == p2.y || other.p1.y == other.p2.y)
			return Point{-1, -1};

		// check if the two lines intersect
		float x = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.x - other.p2.x) - (p1.x - p2.x) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));
		float y = ((p1.x * p2.y - p1.y * p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x * other.p2.y - other.p1.y * other.p2.x)) / ((p1.x - p2.x) * (other.p1.y - other.p2.y) - (p1.y - p2.y) * (other.p1.x - other.p2.x));

		// check if the intersection point is within the bounds of the two lines
		if (x < std::min(p1.x, p2.x) || x > std::max(p1.x, p2.x))
			return Point{-1, -1};
		if (x < std::min(other.p1.x, other.p2.x) || x > std::max(other.p1.x, other.p2.x))
			return Point{-1, -1};
		if (y < std::min(p1.y, p2.y) || y > std::max(p1.y, p2.y))
			return Point{-1, -1};
		if (y < std::min(other.p1.y, other.p2.y) || y > std::max(other.p1.y, other.p2.y))
			return Point{-1, -1};

		return Point{int(x), int(y)};
	}
};

struct Triangle
{
	Point p1;
	Point p2;
	Point p3;
	bool operator==(const Triangle &other) const { return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) || (p1 == other.p1 && p2 == other.p3 && p3 == other.p2) || (p1 == other.p2 && p2 == other.p1 && p3 == other.p3) || (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) || (p1 == other.p3 && p2 == other.p1 && p3 == other.p2) || (p1 == other.p3 && p2 == other.p2 && p3 == other.p1); }
	bool operator!=(const Triangle &other) const { return !(*this == other); }
	bool operator<(const Triangle &other) const { return p1 < other.p1 || (p1 == other.p1 && p2 < other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 < other.p3); }
	bool operator>(const Triangle &other) const { return p1 > other.p1 || (p1 == other.p1 && p2 > other.p2) || (p1 == other.p1 && p2 == other.p2 && p3 > other.p3); }
	// check if the triangle contains the point
	bool contains(const Point &p) const
	{
		// check if the point is on the same side of the line as the other two points
		Edge e1{p1, p2};
		Edge e2{p2, p3};
		Edge e3{p3, p1};
		if (e1.slope() == e2.slope() && e2.slope() == e3.slope())
			return false;
		if (e1.slope() == e2.slope() && e1.slope() != e3.slope())
			return false;
		if (e1.slope() == e3.slope() && e1.slope() != e2.slope())
			return false;
		if (e2.slope() == e3.slope() && e2.slope() != e1.slope())
			return false;

		// check if the point is within the bounds of the triangle
		if (p.x < std::min(p1.x, std::min(p2.x, p3.x)) || p.x > std::max(p1.x, std::max(p2.x, p3.x)))
			return false;
		if (p.y < std::min(p1.y, std::min(p2.y, p3.y)) || p.y > std::max(p1.y, std::max(p2.y, p3.y)))
			return false;

		return true;
	}

	bool intersects(const Triangle &other) const
	{
		// check if the two triangles share an edge
		Edge e1{p1, p2};
		Edge e2{p2, p3};
		Edge e3{p3, p1};
		Edge e4{other.p1, other.p2};
		Edge e5{other.p2, other.p3};
		Edge e6{other.p3, other.p1};
		if (e1 == e4 || e1 == e5 || e1 == e6 || e2 == e4 || e2 == e5 || e2 == e6 || e3 == e4 || e3 == e5 || e3 == e6)
			return false;

		// check if the two triangles intersect
		if (other.contains(p1) || other.contains(p2) || other.contains(p3))
			return true;
		if (contains(other.p1) || contains(other.p2) || contains(other.p3))
			return true;

		return false;
	}
};

std::vector<Point> P(POINT_COUNT);
std::vector<Edge> EdgeList(POINT_COUNT *(POINT_COUNT - 1) / 2);
std::set<Edge> TriEdge;
std::vector<Triangle> Triangles(999);

// formats the prints with the table edges
void padprint(const char *str)
{
	int len = strlen(str);
	int pad = 70 - len;
	printf("| ");
	printf("%s", str);
	for (int i = 0; i < pad; i++)
		printf(" ");
	printf("|\n");
}

// a) generate N random unique points on the plane
void initPoints()
{
	for (int i = 0; i < POINT_COUNT; i++)
	{
		P[i].x = rand() % WIDTH;
		P[i].y = rand() % HEIGHT;
	}
}

// a) render these points
void drawPoints()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	glPointSize(POINT_RADIUS);
	glBegin(GL_POINTS);
	for (Point &p : P)
	{
		glVertex2i(p.x, p.y);
	}
	glEnd();
	glutSwapBuffers();
}

// b) naive triangulation algorithm
void calcEdges()
{
	// calculate length, save to edge list;
	int numEdges = 0;

	// finding ALL possible edges
	for (int i = 0; i < POINT_COUNT; i++)
	{
		for (int j = i + 1; j < POINT_COUNT; j++)
		{
			// calculate the distance between the two points
			// Point 1
			int x0 = P[i].x;
			int y0 = P[i].y;
			// Point 2
			int x1 = P[j].x;
			int y1 = P[j].y;
			// Distance between 2 points (x0, y0) and (x1, y1): d = sqrt((x1-x0)^2 + (y1-y0)^2)
			unsigned int length = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

			// printf("Edge #%i from i = P[%i] to j = P[%i]\tlength: %i\n", numEdges, i, j, length);

			// b) save the triangles in a data structure
			Edge e = {P[i], P[j], length};
			EdgeList[numEdges] = e;
			numEdges++;
		}
	}

	// b) count and print the number of edges created
	printf("Edges: %i\n", numEdges);

	// sort edge vector in order of least-greatest edge length (takes nlogn time)
	std::sort(EdgeList.begin(), EdgeList.end());

	// (FILTER) Find all edges with intersections and disregard them when inserting edges into TriList.
	// This loop will compare every combination of edges and determine which ones have intersections with each other.
	bool intersection;
	TriEdge.clear();

	for (int i = 0; i < numEdges; i++)
	{
		Edge L1 = EdgeList[i];
		intersection = false; // initially assume edge encounters no intersections

		// compare L1 with each L2 in Tri Edge
		for (Edge L2 : TriEdge)
		{
			if (&L1 && &L2)
				continue;

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

			int D = (xb - xa) * (yd - yc) - (yb - ya) * (xd - xc);

			if (D != 0)
			{
				printf("D != 0\n");
				int ta;
				int tb;

				// (i) L2 is not vertical
				if ((xd - xc) != 0)
				{
					printf("not vertical\n");
					ta = ((xc - xa) * (yd - yc) - (yc - ya) * (xd - xc)) / D;
					tb = (xa - xc + (xb - xa) * ta) / (xd - xc);

					if ((ta >= 0 && ta <= 1) && (tb >= 0 && tb <= 1))
					{
						intersection = true;
						break;
					}
				}
				// (ii) L2 is vertical
				else if ((xd - xc) == 0)
				{
					printf("is vertical\n");
					ta = (xc - xa) / (xb - xa);
					tb = (ya + ta * (yb - ya) - yc) / (yd - yc);

					if (ta >= 0 && ta <= 1 && tb >= 0 && tb <= 1)
					{
						intersection = true;
						break;
					}
				}
			}
			//	in D == 0, they might overlap, but we'll not need to deal with this
		} // for-each

		if (!intersection)
		{ // if there is no intersection, then we can add it to TriEdge
			TriEdge.insert(L1);
		}

	} // for

	// b) print number of TriEdges
	printf("TriEdges: %i\n", TriEdge.size());

	printf("Tri-Length: ");
	for (Edge e : TriEdge)
	{
		printf("%i ", e.length);
	}
	printf("\n");
}

// c) triangle polygon extaction - extract triangle polygons from the edges
void extractTriangles()
{
	std::set<Edge> edgeSet(EdgeList.begin(), EdgeList.end());

	int numTriangles = 0;

	for (auto it = edgeSet.begin(); it != edgeSet.end(); ++it)
	{
		Edge e1 = *it;
		auto jt = it;
		for (++jt; jt != edgeSet.end(); ++jt)
		{
			Edge e2 = *jt;
			if (e1.intersects(e2))
			{
				Point p = e1.intersection(e2);
				if (p != e1.p1 && p != e1.p2 && p != e2.p1 && p != e2.p2)
				{
					Triangle t = {e1.p1, e1.p2, p};
					Triangles[numTriangles] = t;
					numTriangles++;
					t = {e1.p1, e2.p1, p};
					Triangles[numTriangles] = t;
					numTriangles++;
					t = {e1.p2, e2.p2, p};
					Triangles[numTriangles] = t;
					numTriangles++;
				}
			}
		}
	}

	printf("Triangles: %i\n", numTriangles);
}

// b) render the edges
void drawEdges()
{
	glColor3f(0.3, 0.72, 0.56);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (Edge &e : EdgeList)
	{
		glVertex2i(e.p1.x, e.p1.y);
		glVertex2i(e.p2.x, e.p2.y);
	}
	glEnd();
	glutSwapBuffers();
}

// c) render the triangles as solid polygons
void drawTriangles()
{
	glLineWidth(2.0f);
	// get random color
	glBegin(GL_TRIANGLES);
	for (Triangle &t : Triangles)
	{
		float r = (float)rand() / (float)RAND_MAX;
		float g = (float)rand() / (float)RAND_MAX;
		float b = (float)rand() / (float)RAND_MAX;
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

// menu
void menu(int id)
{
	switch (id)
	{
	case 1:
		initPoints();
		drawPoints();
		break;
	case 2:
		calcEdges();
		drawEdges();
		break;
	case 3:
		extractTriangles();
		drawTriangles();
		Triangles.clear();
		break;
	case 4:
		exit(0);
		break;
	default:
		break;
	}
}

// prints controls to terminal
void showcmds()
{
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                  2D   TRIANGULATION             ESC / Q: Quit |\n");
	printf("|-----------------------------------------------------------------------|\n");
	printf("| R: Reset Points                                         D: Draw Edges |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
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
	case 't':
		extractTriangles();
		drawTriangles();
		break;
	case 'c':
		// cleanup();
		drawTriangles();
		break;
	case 'h': // h - help
		showcmds();
		break;
	default:
		break;
	}
}

// commenting this out because we might need this later, but I doubt it.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Set dot color to white
	glColor3f(1.0, 1.0, 1.0);

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2); // makes the window appear in the center of the screen
	glutCreateWindow("2D Triangulation");

	glutCreateMenu(menu);
	glutAddMenuEntry("Reset Points", 1);
	glutAddMenuEntry("Draw Edges", 2);
	glutAddMenuEntry("Draw Triangles", 3);
	glutAddMenuEntry("Quit", 4);
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