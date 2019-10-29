#include "Mesh.h"

#include <iostream>
Mesh::Mesh() {

}
Mesh::Mesh(GLfloat * vertices, GLfloat * normals, GLfloat * tex, GLuint * indices,
		   int numVert, int numNorms, int numIndices, int numTex){
	this->vertices = new GLfloat[numVert];
	this->normals = new GLfloat[numNorms];
	this->textures = new GLfloat[numTex];

	this->vertices = vertices;
	this->normals = normals;
	this->indices = indices;
	this->textures = tex;

	this->numTex = numTex;
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


void Mesh::loadAttrib(GLuint program) {

	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVert * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);

	vTex = glGetAttribLocation(program, "vTex");
	glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, (void*)((numVert + numTex) * sizeof(GLfloat)));
	glEnableVertexAttribArray(vTex);
}

void Mesh::sendToGPU(GLuint program){
	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (numVert + numNorm + numTex) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, numVert * sizeof(GLfloat), vertices); // Verticies

	glBufferSubData(GL_ARRAY_BUFFER, numVert * sizeof(GLfloat), numNorm * sizeof(GLfloat), normals);

	glBufferSubData(GL_ARRAY_BUFFER, (numVert + numNorm) * sizeof(GLfloat), numTex * sizeof(GLfloat), textures);



	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void Mesh::setTBuffer(GLuint tBuffer){
	this->tBuffer = tBuffer;
}

GLuint* Mesh::getTBufferPointer(){
	return &tBuffer;
}
GLuint Mesh::getTBuffer() {
	return tBuffer;
}

GLuint Mesh::getiBuffer()
{
	return ibuffer;
}

int Mesh::getTriangles() {
	return triangles;
}
