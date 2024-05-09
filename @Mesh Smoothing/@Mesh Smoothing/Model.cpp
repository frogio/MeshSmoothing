#include "Model.h"

#pragma warning(disable:4996)

using namespace std;

void Model::loadModel(const char * path) {

	FILE* fp = fopen(path, "r");
	char buffer[100] = { 0, };
	int vertices_count = 0, faces_count = 0;

	// v 속성값 읽어옴 (버텍스 하나의 위치값 Vector로 표현)
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
			// index zero때문에 - 1

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

	// v-f, 버텍스와 이웃한 면의 정보
	for (int i = 0; i < faces.size(); i++) {
		for (int k = 0; k < 3; k++) {
			faces[i]->vertices[k]->nbFaces.push_back(faces[i]);
		}
	}

	// v-v, 버텍스와 이웃한 버텍스의 정보
	
	for (int v = 0; v < vertices.size(); v++) {
		
		Vertex * curVertex = vertices[v];		// 현재 버텍스를 가져옴
		for (int f = 0; f < curVertex->nbFaces.size(); f++) {
			
			// pivot : 0: 1, 2 
			// pivot : 1: 2, 0
			// pivot : 2: 0, 1			
			Face * curFace = curVertex->nbFaces[f];			// 현재 버텍스의 이웃 면 중 하나를 가져옴
			int pivot_idx = curFace->getIndex(curVertex),	// 현재 이웃면에서 자신은 몇번째 인덱스인지 확인
				other_idx1 = (pivot_idx + 1) % 3,
				other_idx2 = (pivot_idx + 2) % 3;
			// 이웃면의 다른 두 버텍스의 인덱스를 가져옴

			if (curVertex->isDuplicatedVertex(curFace->vertices[other_idx1]) == false)		// 만약 넣지 않은 이웃 버텍스라면
				curVertex->nbVertices.push_back(curFace->vertices[other_idx1]);				// 삽입한다.

			if (curVertex->isDuplicatedVertex(curFace->vertices[other_idx2]) == false)		// 만약 넣지 않은 이웃 버텍스라면
				curVertex->nbVertices.push_back(curFace->vertices[other_idx2]);				// 삽입한다.
		
			// 모든 이웃면의 버텍스를 확인하면 다음 이웃 면을 본다
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
		// 버텍스 위치의 평균이 새로운 위치값.
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