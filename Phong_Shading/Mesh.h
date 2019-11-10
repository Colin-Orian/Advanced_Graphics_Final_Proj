#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CreateGeometry.h";
class Mesh {
private:
	GLfloat *vertices;
	GLfloat *normals;
	GLfloat * textures;

	GLuint *indices;
	GLfloat *combined;


	GLuint vbuffer;
	
	GLint vNormal;
	GLint vTex;
	GLuint nbuffer;
	GLuint ibuffer;			// index buffer identifier
	GLuint tBuffer;


	int numVert;
	int numNorm;
	int numTex;
	int numIndices;
	int triangles;
public:
	GLuint objVAO;
	GLint vPosition;
public:
	Mesh(std::string fileName);
	void loadAttrib(GLuint program);
	void sendToGPU();

	int getTriangles();
};