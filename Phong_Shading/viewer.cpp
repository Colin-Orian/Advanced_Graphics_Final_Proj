/************************************************************
A Base for all by OpenGL projects.
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
#include "viewer.h"


struct Light {
	glm::vec3 lightPos;
	glm::vec4 lightColour;
	float intensity;
};

float eyex, eyey, eyez;	// current user position
float lightx, lighty, lightz;
double theta, phi;		// user's position  on a sphere centered on the object
double r;				// radius of the sphere

GLuint program;
GLuint postProssProg;
GLuint hdrBuff;
GLuint hdrDepth;
GLuint hdrColor;
GLuint hdrBright;
glm::mat4 projection;	// projection matrix

std::vector<Light> lights;
int numLights = 0;
std::vector<Mesh> meshes;
Mesh planeMesh;

unsigned int WIDTH = 512;
unsigned int HEIGHT = 512;

int HDR_ON = 1;
int BLOOM_ON = 1;
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

	std::cout << std::endl;
	vs = buildShader(GL_VERTEX_SHADER, (char*)"outVert.hlsl");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"outFrag.hlsl");
	postProssProg = buildProgram(vs, fs, 0);
	dumpProgram(postProssProg, (char*)"Post Process Program");

	glGenFramebuffers(1, &hdrBuff);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrBuff);
	glGenTextures(1, &hdrDepth);
	glBindTexture(GL_TEXTURE_2D, hdrDepth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, WIDTH, HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, hdrDepth, 0);

	glGenTextures(1, &hdrColor);
	glBindTexture(GL_TEXTURE_2D, hdrColor);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, WIDTH, HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, hdrColor, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &hdrBright);
	glBindTexture(GL_TEXTURE_2D, hdrBright);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, WIDTH, HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, hdrBright, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Create the meshes
	Mesh mesh("sphere");
	planeMesh = Mesh(WIDTH, HEIGHT);
	Mesh cube("dragon");

	//Store the meshes to be rendered
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.objVAO);


	glm::mat4 transMat = glm::mat4(1.0f);
	if (isSphere) {
		transMat = glm::translate(transMat, glm::vec3(4.0f, 0.0f, 0.0f));
	}
	else {
		transMat = glm::scale(transMat, glm::vec3(0.3));
	}

	
	loadUniformMat4(program, "view", view);
	loadUniformMat4(program, "transMat", transMat);
	loadUniformMat4(program, "projection", projection);
	loadUniform3f(program, "Eye", eyex, eyey, eyez);
	loadUniform1i(program, "numLights", 1);
	for (int i = 0; i < numLights; i++) {
		std::string varName = "lightPos[" + std::to_string(i) + "]";
		loadUniform3f(program, varName, lights[i].lightPos.x, lights[i].lightPos.y, lights[i].lightPos.z);
		varName = "lightColor[" + std::to_string(i) + "]";
		loadUniform4f(program, varName, lights[i].lightColour.x, lights[i].lightColour.y, lights[i].lightColour.z, lights[i].lightColour.a);
		varName = "intensity[" + std::to_string(i) + "]";
		loadUniform1i(program, varName, lights[i].intensity);
	}


	loadUniform1i(program, "hdr_on", HDR_ON);

	mesh.loadAttrib(program);

	double t1 = getSeconds();
	glDrawElements(GL_TRIANGLES, 3 * mesh.getTriangles(), GL_UNSIGNED_INT, NULL);

}
void display(void) {

	//Load the scene to a frame buffer and render it to a texture.
	glBindFramebuffer(GL_FRAMEBUFFER, hdrBuff);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	GLenum buff[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glViewport(0, 0, WIDTH, HEIGHT);
	glDrawBuffers(2, buff);
	GLuint status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << status << std::endl;
	}
	bool isSphere = true;

	//Render each mesh
	for (Mesh mesh : meshes) {
		render(mesh, isSphere);
		isSphere = !isSphere;
	}

	//Bind the default frame buffer and then render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind the scene texture and render the texture to the screne
	glUseProgram(postProssProg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrColor);
	loadUniform1i(postProssProg, "tex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hdrBright);
	loadUniform1i(postProssProg, "bloomTex", 1);


	loadUniform1i(postProssProg, "hdr_on", HDR_ON);
	loadUniform1i(postProssProg, "bloom_on", BLOOM_ON);

	glViewport(0, 0, WIDTH, HEIGHT);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeMesh.objVAO);
	planeMesh.loadPlane(postProssProg);
	glDrawElements(GL_TRIANGLES, 3 * planeMesh.getTriangles(), GL_UNSIGNED_INT, NULL);

	glFinish();

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		if (HDR_ON == 1) {
			printf("HDR_OFF\n");
			HDR_ON = 0;
			//lights[1].intensity = 0.5f;
		}
		else {
			printf("HDR_ON\n");
			HDR_ON = 1;
			//lights[1].intensity = 1.5f;
		}
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		if (BLOOM_ON == 1) {
			printf("BLOOM_OFF\n");
			BLOOM_ON = 0;
			//lights[1].intensity = 0.5f;
		}
		else {
			printf("BLOOM_ON\n");
			BLOOM_ON = 1;
			//lights[1].intensity = 1.5f;
		}
	}

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
	Light flashLight;
	flashLight.lightPos = glm::vec3(lightx, lighty, lightz);
	flashLight.lightColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	flashLight.intensity = 1.0f;
	lights.push_back(flashLight);
	numLights++;
	Light sun;
	sun.lightPos = glm::vec3(lightx, lighty - 5.0f, lightz - 5.0f);
	sun.lightColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	sun.intensity = 1.5f;
	lights.push_back(sun);
	numLights++;

	theta = 1.5;
	phi = 1.5;
	r = 10.0;
	glfwSwapInterval(1);

	// GLFW main loop, display model, swapbuffer and check for input
	printf("Press 1 to toggle HDR. HDR is currently ON\n");
	
	printf("Press 2 to toggle Bloom. Bloom is currently ON\n");
	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteFramebuffers(1, &hdrBuff);
	glfwTerminate();

}
