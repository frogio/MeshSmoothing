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

	// ��Ϲ��� ����� ������ ������.
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

	// �̿� ���ؽ��� ������ �ڽ� ����, ���� �ΰ��ۿ� ����.
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
	glLoadIdentity();						// �������
	glViewport(0, 0, w, h);					// ����Ʈ ���� (�����쿡 �׸� ����)
	gluOrtho2D(-2.f, 2.f, -2.f, 2.f);		// ��ǥ�� ������, X : 0 ~ 100, Y : 0 ~ 100
}

void glDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glBegin(GL_LINE_LOOP);
		
		// ������ �׸���.
		for (int i = 0; i < vertices.size(); i++) {
			Position pos = vertices[i].pos;
			glVertex2d(pos.x, pos.y);
		}

	glEnd();
	glFlush();


}

void smoothing() {
	// Simple Mesh Smoothing
	// Simple Mesh Smoothing�� �̿� ���ؽ����� ��ġ�� �ڽ��� ��ġ�� ���Ͽ� ����� ����
	// �� ����� �ڽŰ� �̿��� ��ġ�� ������ ������ ���� �� �ִ� �˰����̴�.

	for (int i = 0; i < vertices.size(); i++) {

		Vertex v = vertices[i];
		Position newPos = v.pos;

		for (int k = 0; k < v.nbVertices.size(); k++) {
			newPos.x += v.nbVertices[k].x;
			newPos.y += v.nbVertices[k].y;
		}

		newPos.x /= v.nbVertices.size() + 1;
		newPos.y /= v.nbVertices.size() + 1;		// �ڱ� �ڽ��� �����ϹǷ� + 1
		// ����� ���Ѵ�.
		
		vertices[i].pos = newPos;
		// ���ο� ��ġ�� �ڽŰ� �̿��� ��� ��ġ�̴�.

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