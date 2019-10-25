#include "Mesh.h"

#include <iostream>
Mesh::Mesh() {

}
Mesh::Mesh(GLfloat * vertices, GLfloat * normals, GLuint * indices,
		   int numVert, int numNorms, int numIndices){
	this->vertices = vertices;
	this->normals = normals;
	this->indices = indices;

	this->numVert = numVert;
	this->numNorm = numNorms;
	this->numIndices = numIndices;
	triangles = numIndices / 3;
	this->combined = new GLfloat[numVert + numNorm];
	int k = 0;
	for (int i = 0; i < numVert / 3; i++) {
		this->combined[k++] = vertices[3 * i];
		this->combined[k++] = vertices[3 * i + 1];
		this->combined[k++] = vertices[3 * i + 2];
		this->combined[k++] = normals[3 * i];
		this->combined[k++] = normals[3 * i + 1];
		this->combined[k++] = normals[3 * i + 2];
		
	}
}



void Mesh::sendToGPU(GLuint program){
	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	 //WORKS
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (numVert + numNorm) * sizeof(GLfloat), this->combined, GL_STATIC_DRAW);
	glUseProgram(program);

	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof GLfloat), 0);
	glEnableVertexAttribArray(vPosition);


	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 
							6 * (sizeof GLfloat), (void*)(this->combined + 3 * (sizeof GLfloat)));
	glEnableVertexAttribArray(vNormal);
	//WORKS



	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

GLuint Mesh::getiBuffer()
{
	return ibuffer;
}

int Mesh::getTriangles() {
	return triangles;
}
