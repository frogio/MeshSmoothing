#include "Model.h"

#pragma warning(disable:4996)

using namespace std;

void Model::loadModel(const char * path) {

	FILE* fp = fopen(path, "r");
	char buffer[100] = { 0, };
	int vertices_count = 0, faces_count = 0;

	// v �Ӽ��� �о�� (���ؽ� �ϳ��� ��ġ�� Vector�� ǥ��)
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		if (buffer[0] == 'v') {
			double x, y, z;

			strtok(buffer, " ");
			x = atof(strtok(NULL, " "));
			y = atof(strtok(NULL, " "));
			z = atof(strtok(NULL, " "));

			vertices.push_back(new Vertex(x, y, z));
			vertices_count++;
		}
		else if (buffer[0] == 'f') {
			int v1, v2, v3;

			strtok(buffer, " ");
			v1 = atoi(strtok(NULL, " ")) - 1;
			v2 = atoi(strtok(NULL, " ")) - 1;
			v3 = atoi(strtok(NULL, " ")) - 1;
			// index zero������ - 1

			faces.push_back(new Face(vertices[v1], vertices[v2], vertices[v3]));
			faces_count++;
		}

	}

	fclose(fp);
	
	printf("vertices count : %d\n", vertices_count);
	printf("face count : %d\n", faces_count);
	printf("calculate faces normals...\n");
	CalculFaceNormal();

	printf("model load completed...\n");

	buildAdjacency();
}

void Model::CalculFaceNormal() {

	normals.clear();

	for(int i = 0; i < faces.size(); i++){
		Face* face = faces[i];
		
		glm::vec3 vec1, vec2;
		vec1 = face->vertices[1]->Position - face->vertices[0]->Position;
		vec2 = face->vertices[2]->Position - face->vertices[0]->Position;

		glm::vec3 * normal = new glm::vec3();
		*normal = glm::normalize(glm::cross(vec1, vec2));
		normals.push_back(normal);

	}

}

void Model::drawModel() {

	for (int i = 0; i < faces.size(); i++) {
		glBegin(GL_POLYGON);
		Face * face = faces[i];
		glNormal3f(normals[i]->x, normals[i]->y, normals[i]->z);

		for (int k = 0; k < 3; k++)
			glVertex3f(face->vertices[k]->Position.x, face->vertices[k]->Position.y, face->vertices[k]->Position.z);
		glEnd();
	}



}

void Model::buildAdjacency() {

	// v-f, ���ؽ��� �̿��� ���� ����
	for (int i = 0; i < faces.size(); i++) {
		for (int k = 0; k < 3; k++) {
			faces[i]->vertices[k]->nbFaces.push_back(faces[i]);
		}
	}

	// v-v, ���ؽ��� �̿��� ���ؽ��� ����
	
	for (int v = 0; v < vertices.size(); v++) {
		
		Vertex * curVertex = vertices[v];		// ���� ���ؽ��� ������
		for (int f = 0; f < curVertex->nbFaces.size(); f++) {
			
			// pivot : 0: 1, 2 
			// pivot : 1: 2, 0
			// pivot : 2: 0, 1			
			Face * curFace = curVertex->nbFaces[f];			// ���� ���ؽ��� �̿� �� �� �ϳ��� ������
			int pivot_idx = curFace->getIndex(curVertex),	// ���� �̿��鿡�� �ڽ��� ���° �ε������� Ȯ��
				other_idx1 = (pivot_idx + 1) % 3,
				other_idx2 = (pivot_idx + 2) % 3;
			// �̿����� �ٸ� �� ���ؽ��� �ε����� ������

			if (curVertex->isDuplicatedVertex(curFace->vertices[other_idx1]) == false)		// ���� ���� ���� �̿� ���ؽ����
				curVertex->nbVertices.push_back(curFace->vertices[other_idx1]);				// �����Ѵ�.

			if (curVertex->isDuplicatedVertex(curFace->vertices[other_idx2]) == false)		// ���� ���� ���� �̿� ���ؽ����
				curVertex->nbVertices.push_back(curFace->vertices[other_idx2]);				// �����Ѵ�.
		
			// ��� �̿����� ���ؽ��� Ȯ���ϸ� ���� �̿� ���� ����
		}
	
	}


	printf("building adjacency completed...\n");
}

void Model::simpleSmoothing() {

	vector<glm::vec3> newPos;

	for (int v = 0; v < vertices.size(); v++) {
		Vertex* curVertex = vertices[v];
		glm::vec3 pos = curVertex->Position;

		for (int nv = 0; nv < curVertex->nbVertices.size(); nv++) {
			Vertex * nbVertex = curVertex->nbVertices[nv];
			pos += nbVertex->Position;
		}

		pos /= (double)curVertex->nbVertices.size() + 1;
		newPos.push_back(pos);
		// ���ؽ� ��ġ�� ����� ���ο� ��ġ��.
	}
	
	for (int v = 0; v < vertices.size(); v++)
		vertices[v]->Position = newPos[v];
	
	CalculFaceNormal();

}

void Model::laplacianSmoothing(float shrink) {

	vector<glm::vec3> newPos;

	for (int v = 0; v < vertices.size(); v++) {

		Vertex* curVertex = vertices[v];
		glm::vec3 curPos = vertices[v]->Position, avgPos;
		int nbCount = curVertex->nbVertices.size();
		avgPos.x = avgPos.y = avgPos.z = 0;

		for (int nv = 0; nv < nbCount; nv++)
			avgPos += curVertex->nbVertices[nv]->Position;

		glm::vec3 shrinkPos = curPos + (avgPos / (float)nbCount - curPos) * shrink;
		newPos.push_back(shrinkPos);
	}

	for (int v = 0; v < vertices.size(); v++)
		vertices[v]->Position = newPos[v];

	CalculFaceNormal();

}
void Model::taubinSmoothing(float shrink, float expand) {

	vector<glm::vec3> newPos;
	
	for (int v = 0; v < vertices.size(); v++) {
	
		Vertex* curVertex = vertices[v];
		glm::vec3 curPos = curVertex->Position, avgPos;
		avgPos.x = avgPos.y = avgPos.z = 0;
		int nbCount = curVertex->nbVertices.size();
	
		for (int nv = 0; nv < nbCount; nv++)
			avgPos += curVertex->nbVertices[nv]->Position;
		
		glm::vec3 shrinkPos = curPos + (avgPos / (float)nbCount - curPos) * shrink;
		glm::vec3 expandedPos = shrinkPos + (avgPos / (float)nbCount - shrinkPos) * expand;

		newPos.push_back(expandedPos);
	}

	for (int v = 0; v < vertices.size(); v++)
		vertices[v]->Position = newPos[v];

	CalculFaceNormal();
}