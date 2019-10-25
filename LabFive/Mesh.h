#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class Mesh {
private:
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	GLfloat *combined;

	GLuint vbuffer;
	
	GLint vNormal;

	GLuint ibuffer;			// index buffer identifier

	int numVert;
	int numNorm;
	int numIndices;
	int triangles;
public:
	GLuint objVAO;
	GLint vPosition;
public:
	Mesh();
	Mesh(GLfloat* vertices, GLfloat* normals, GLuint* indices,
		 int numVert, int numNorm, int numIndices);
	void sendToGPU(GLuint program);

	GLuint getiBuffer();
	int getTriangles();
};