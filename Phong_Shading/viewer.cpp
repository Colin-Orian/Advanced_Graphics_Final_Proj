/************************************************************
*                   CSCI 4110 Lab 2
*
*  Basic OpenGL program that shows how to set up a
*  VAO and some basic shaders.  This program draws
*  a cube or sphere depending upon whether CUBE or
*  SPHERE is defined.
*
**********************************************************/
#define GLM_FORCE_RADIANS
#define _CRT_SECURE_NO_WARNINGS

#define GLFW_DLL
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "Shaders.h"
#include "Mesh.h"
#include "tiny_obj_loader.h"
#include "CreateGeometry.h"
#include "Texture.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <windows.h>
#include <vector>

float eyex, eyey, eyez;	// current user position
float lightx, lighty, lightz;
double theta, phi;		// user's position  on a sphere centered on the object
double r;				// radius of the sphere

GLuint program;
GLuint skyboxProgam;
GLuint shadowProgram;
glm::mat4 projection;	// projection matrix

std::vector<Mesh> meshes;
int isQuad;


glm::mat4 shadowMatrix;
unsigned int WIDTH = 512;
unsigned int HEIGHT = 512;
double getSeconds() {
	LARGE_INTEGER freq, val;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&val);
	return (double)val.QuadPart / (double)freq.QuadPart;
}

void init() {

	
	int fs;
	int vs;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 1.0, 1.0, 1.0);
	glViewport(0, 0, WIDTH, HEIGHT);

	projection = glm::perspective(0.7f, 1.0f, 1.0f, 100.0f);

	vs = buildShader(GL_VERTEX_SHADER, (char*)"Assign2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"Assign2.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, (char*)"Lab 2 shader program");

	Mesh mesh("sphere");
	Mesh cube("cube");
	
	
	meshes.push_back(mesh);
	meshes.push_back(cube);
}

void framebufferSizeCallback(GLFWwindow *window, int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0f * w / h;

	glfwMakeContextCurrent(window);

	glViewport(0, 0, w, h);

	projection = glm::perspective(0.7f, ratio, 1.0f, 100.0f);

}

void render(Mesh mesh, bool isSphere) {

	float radius = 0.2;
	float step = 1.5;
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	//isSphere = true;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.objVAO);


	glm::mat4 transMat = glm::mat4(1.0f);
	if (isSphere) {
		transMat = glm::translate(transMat, glm::vec3(4.0f, 0.0f, 0.0f));
	}

	glUseProgram(program);
	loadUniformMat4(program, "view", view);
	loadUniformMat4(program, "transMat", transMat);
	loadUniformMat4(program, "projection", projection);
	loadUniform3f(program, "Eye", eyex, eyey, eyez);
	loadUniform3f(program, "light", lightx, lighty, lightz);

	mesh.loadAttrib(program);
	
	double t1 = getSeconds();
	glDrawElements(GL_TRIANGLES, 3 * mesh.getTriangles(), GL_UNSIGNED_INT, NULL);

}
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	bool isSphere = true;
		
	for (Mesh mesh : meshes) {
		render(mesh, isSphere);
		isSphere = !isSphere;
	}
	
	//renderSky();
	glFinish();

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		phi -= 0.1;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		phi += 0.1;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		theta += 0.1;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		theta -= 0.1;
	
	eyex = (float)(r*sin(theta)*cos(phi));
	eyey = (float)(r*sin(theta)*sin(phi));
	eyez = (float)(r*cos(theta));

}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv) {

	GLFWwindow *window;

	// start by setting error callback in case something goes wrong

	glfwSetErrorCallback(error_callback);

	// initialize glfw

	if (!glfwInit()) {
		fprintf(stderr, "can't initialize GLFW\n");
	}

	// create the window used by our application

	window = glfwCreateWindow(WIDTH, HEIGHT, "Laboratory Two", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// establish framebuffer size change and input callbacks

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, key_callback);

	// now initialize glew our extension handler

	glfwMakeContextCurrent(window);

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	init();

	lightx = eyex = -1.0;
	lightz = eyez = 0.0;
	lighty = eyey = 10.0;



	theta = 1.5;
	phi = 1.5;
	r = 10.0;

	glfwSwapInterval(1);

	// GLFW main loop, display model, swapbuffer and check for input

	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

}
