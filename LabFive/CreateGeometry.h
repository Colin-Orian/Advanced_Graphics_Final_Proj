#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <string>

#include <vector>
#include "tiny_obj_loader.h"
#include <glm/glm.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
Mesh loadFile(GLuint program, std::string fileName);

Mesh createQuad(GLuint program);