#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <string>
#include <GLFW/glfw3.h>
#include <vector>
#include "tiny_obj_loader.h"
#include <glm/glm.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

struct VertexData {
	GLfloat *vert;
	GLfloat *norm;
	GLfloat *indicies;
	
};


void loadFile(std::string fileName, GLfloat *vertices, GLfloat *normals, GLuint *indices);

