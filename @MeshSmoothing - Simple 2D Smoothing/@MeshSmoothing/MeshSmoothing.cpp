#include <stdio.h>
#include <vector>
#include <math.h>
#include "glut.h"

#define PI		3.141592

using namespace std;

typedef struct Pos {
	double x;
	double y;
} Position;

typedef struct vertex {
	Position pos;
	vector<Position> nbVertices;
} Vertex;

vector<Vertex> vertices;

void glDisplay();
void glReshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void smoothing();
void InitVertices();
void InitNeighbours();

void main() {

	InitVertices();
	printf("press '1' to simple smoothing\n");

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Mesh Smoothing 2D");
	glutReshapeFunc(glReshape);
	glutDisplayFunc(glDisplay);
	glutKeyboardFunc(keyboard);
	glutMainLoop();


}

void InitVertices() {

	// 톱니바퀴 모양의 도형을 만들어낸다.
	printf("Init Vertices...\n");
	for (int i = 0; i < 72; i++) {
		double radian = 2 * PI * (double)i / 72;
		double x1 = cos(radian) * 1.2f,
			y1 = sin(radian) * 1.2f;

		double x2 = 0.8f * cos(radian + 0.1f),
			y2 = 0.8f * sin(radian + 0.1f);

		//double x3 = 0.7f * cos(radian + 0.07f),
		//	y3 = 0.7f * sin(radian + 0.07f);

		Vertex vertex1 = { x1, y1 };
		Vertex vertex2 = { x2, y2 };
		//Vertex vertex3 = { x3, y3 };

		vertices.push_back(vertex1);
		//vertices.push_back(vertex3);
		vertices.push_back(vertex2);

	}

	InitNeighbours();

}

void InitNeighbours() {
	printf("setup negibour infos...\n");
	// setup neighbor vertices

	// 이웃 버텍스는 무조건 자신 이전, 이후 두개밖에 없다.
	// 143, 0, 1	pivot : 0, prev : 143, after : 1
	// 0, 1, 2		pivot : 1, prev : 0, after : 2
	// 1, 2, 3		pivot : 2, prev : 1, after : 3
	for (int i = 0; i < vertices.size(); i++) {

		Vertex prev_v = vertices[(vertices.size() + i - 1) % vertices.size()];
		Vertex after_v = vertices[(i + 1) % vertices.size()];

		vertices[i].nbVertices.clear();
		vertices[i].nbVertices.push_back(prev_v.pos);
		vertices[i].nbVertices.push_back(after_v.pos);
	}

}


void glReshape(int w, int h) {
	glLoadIdentity();						// 단위행렬
	glViewport(0, 0, w, h);					// 뷰포트 지정 (윈도우에 그릴 영역)
	gluOrtho2D(-2.f, 2.f, -2.f, 2.f);		// 좌표계 재지정, X : 0 ~ 100, Y : 0 ~ 100
}

void glDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glBegin(GL_LINE_LOOP);
		
		// 도형을 그린다.
		for (int i = 0; i < vertices.size(); i++) {
			Position pos = vertices[i].pos;
			glVertex2d(pos.x, pos.y);
		}

	glEnd();
	glFlush();


}

void smoothing() {
	// Simple Mesh Smoothing
	// Simple Mesh Smoothing은 이웃 버텍스들의 위치와 자신의 위치를 합하여 평균을 내고
	// 그 평균을 자신과 이웃의 위치로 삼으면 간단히 구할 수 있는 알고리즘이다.

	for (int i = 0; i < vertices.size(); i++) {

		Vertex v = vertices[i];
		Position newPos = v.pos;

		for (int k = 0; k < v.nbVertices.size(); k++) {
			newPos.x += v.nbVertices[k].x;
			newPos.y += v.nbVertices[k].y;
		}

		newPos.x /= v.nbVertices.size() + 1;
		newPos.y /= v.nbVertices.size() + 1;		// 자기 자신을 포함하므로 + 1
		// 평균을 구한다.
		
		vertices[i].pos = newPos;
		// 새로운 위치는 자신과 이웃의 평균 위치이다.

	}
	InitNeighbours();
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
		case '1':
			printf("Smoothing...\n");
			smoothing();
		break;
	
	}

	glutPostRedisplay();

}