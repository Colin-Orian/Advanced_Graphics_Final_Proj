#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
/**************************************************
 *
 *                 Shaders.h
 *
 *  Utility functions that make constructing shaders
 *  a bit easier.
 *
 ***************************************************/

int buildShader(int type, char *filename);
int buildProgram(int first, ...);
void dumpProgram(int program, char *description);

void loadUniformMat4(GLuint program, std::string varName, glm::mat4 matrix);

void loadUniform1i(GLuint program, std::string varName, int num);

void loadUniform3f(GLuint program, std::string varName, float x, float y, float z);

void loadUniform1f(GLuint program, std::string varName, float var);

void loadUniform4f(GLuint program, std::string varName, float x, float y, float z, float a);
