#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
	Mesh();
	Mesh(GLfloat * vertices, GLfloat * normals, GLfloat * tex, GLuint * indices, int numVert, int numNorms, int numIndices, int numTex);
	void loadAttrib(GLuint program);
	void sendToGPU(GLuint program);
	void setTBuffer(GLuint tBuffer);
	GLuint* getTBufferPointer();
	GLuint getTBuffer();
	GLuint getiBuffer();
	int getTriangles();
};