/*************************************************
 *
 *                 texture.h
 *
 *  Simple texture loader.  Data structure and
 *  procedure declarations
 *
 *************************************************/
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
struct Texture {
	int width;
	int height;
	int depth;
	int size;
	unsigned char *data;
};

struct Cube {
	int width;
	int height;
	int depth;
	unsigned char *data[6];
};
Texture * loadTexture(const char * filename, const char * fileFormat);
struct Cube *loadCube(const char *basename);

void createFramebufferTexture(GLuint *tex, unsigned int WIDTH, unsigned int HEIGHT, GLenum type, GLenum target);

void createTexture(GLuint * texLoc, Texture texture);
