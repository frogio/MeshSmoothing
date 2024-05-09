#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <glm.hpp>

#include "glut.h"

using namespace std;

struct Vertex;

struct Face {
	Vertex* vertices[3];

	Face(Vertex * v1, Vertex * v2, Vertex * v3) {
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
	}

	int getIndex(Vertex* v) {					// ���ڷ� ���� v�� Face�� ���° index���� Ȯ���Ѵ�. (3���� Vertex�̹Ƿ� 0, 1, 2 �� �ϳ�)

		for (int i = 0; i < 3; i++) {

			if (vertices[i] == v)
				return i;

		}

		return -1;
	}
};

struct Vertex {
	glm::vec3 Position;
	vector<Face*> nbFaces;
	vector<Vertex*> nbVertices;

	Vertex(double x, double y, double z) {
		Position.x = x;
		Position.y = y;
		Position.z = z;
	}

	bool isDuplicatedVertex(Vertex* v) {

		for (int i = 0; i < nbVertices.size(); i++) {

			if (v == nbVertices[i])
				return true;

		}

		return false;

	}

};


class Model {
public:
	Model(const char * _path) : path(_path){
		loadModel(path);
	}
	void drawModel();
	void simpleSmoothing();
	void laplacianSmoothing(float shrink);
	void taubinSmoothing(float shrink, float expand);


private:
	vector<Face *>			faces;
	vector<Vertex *>		vertices;
	vector<glm::vec3 *>		normals;				// face�� ���� �븻��
	const char* path;
	
	void loadModel(const char * path);
	void buildAdjacency();
	void CalculFaceNormal();

};

#endif