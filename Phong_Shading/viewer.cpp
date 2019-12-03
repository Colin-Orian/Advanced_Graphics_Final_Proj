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
#include "Model.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <random>

struct Light {
	glm::vec3 lightPos;
	glm::vec4 lightColour;
	float intensity;
};

struct Particle {
	glm::mat4 transform;
	glm::vec3 colour;
	double timeAlive;
};

float eyex, eyey, eyez;	// current user position
float lightx, lighty, lightz;
double theta, phi;		// user's position  on a sphere centered on the object
double r;				// radius of the sphere

GLuint preProssProg;
GLuint postProssProg;
GLuint partProg;
GLuint shaftProg;

//Pre process buffer
GLuint hdrBuff;
GLuint hdrDepth; //depth buffer
GLuint hdrColor; //Colour texture
GLuint hdrBright; //Bloom texture



GLuint shaftBuff;
GLuint sunShaft; //Sun Shaft texture
GLuint shaftDepth;
glm::mat4 projection;	// projection matrix

std::vector<Light> lights;
int numLights = 0;
std::vector<Mesh> meshes;
std::vector<Model> models;
Mesh planeMesh;

const double TIME_TO_KILL = 0.5; 
std::vector<Particle> particles;
double prevSecond;

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
	
	srand(time(0));
	int fs;
	int vs;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0, 0, WIDTH, HEIGHT);

	projection = glm::perspective(0.7f, 1.0f, 1.0f, 100.0f);

	vs = buildShader(GL_VERTEX_SHADER, (char*)"Assign2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"Assign2.fs");
	preProssProg = buildProgram(vs, fs, 0);
	dumpProgram(preProssProg, (char*)"HDR Shader");

	std::cout << std::endl;
	vs = buildShader(GL_VERTEX_SHADER, (char*)"outVert.hlsl");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"outFrag.hlsl");
	postProssProg = buildProgram(vs, fs, 0);
	dumpProgram(postProssProg, (char*)"Post Process Program");


	std::cout << std::endl;
	vs = buildShader(GL_VERTEX_SHADER, (char*)"particleVert.hlsl");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"particleFrag.hlsl");
	partProg = buildProgram(vs, fs, 0);
	dumpProgram(partProg, (char*)"Particle");

	std::cout << std::endl;
	vs = buildShader(GL_VERTEX_SHADER, (char*)"shaftVert.hlsl");
	fs = buildShader(GL_FRAGMENT_SHADER, (char*)"shaftFrag.hlsl");
	shaftProg = buildProgram(vs, fs, 0);
	dumpProgram(shaftProg, (char*)"Shaft Program");

	glGenFramebuffers(1, &hdrBuff);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrBuff);
	createFramebufferTexture(&hdrDepth, WIDTH, HEIGHT, GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT);
	createFramebufferTexture(&hdrColor, WIDTH, HEIGHT, GL_RGBA32F, GL_COLOR_ATTACHMENT0);
	createFramebufferTexture(&hdrBright, WIDTH, HEIGHT, GL_RGBA32F, GL_COLOR_ATTACHMENT1);


	glGenFramebuffers(1, &shaftBuff);
	glBindFramebuffer(GL_FRAMEBUFFER, shaftBuff);
	createFramebufferTexture(&sunShaft, WIDTH, HEIGHT, GL_RGBA32F, GL_COLOR_ATTACHMENT0);
	createFramebufferTexture(&shaftDepth, WIDTH, HEIGHT, GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT);

	//Create the meshes
	Mesh sphere("sphere");
	planeMesh = Mesh(WIDTH, HEIGHT);
	Mesh dragon("dragon");
	//Store the meshes to be rendered
	meshes.push_back(sphere);
	meshes.push_back(dragon);

	Mesh plane = Mesh("cube"); //Create a plane for particles

	meshes.push_back(plane);
	Model partModel = Model(plane);
	partModel.setColour(glm::vec3(1.0f, 1.0f, 1.0f));

	Model sunModel = Model(sphere);
	sunModel.setColour(glm::vec3(1.0f, 1.0f, 1.0f));
	sunModel.translate(glm::vec3(0.0f, 4.0f, 4.0f));
	sunModel.toggleEmiter();

	Model dragonModel = Model(dragon);
	dragonModel.setColour(glm::vec3(0.0f, 0.0f, 1.0f));
	dragonModel.scale(glm::vec3(0.25, 0.25, 0.25));

	models.push_back(sunModel);
	models.push_back(dragonModel);
	

}

void update() {
	//Create particle
	Particle particle;
	particle.transform = glm::mat4();
	particle.transform = glm::scale(particle.transform, glm::vec3(0.15f, 0.15f, 0.15f));
	
	float x = rand() % 10;
	float y = rand() % 10;
	float z = rand() % 10;
	particle.transform = glm::translate(particle.transform, glm::vec3(x, y, z));

	particle.colour = glm::vec3(1.0f, 0.0f, 0.0f);
	particle.timeAlive = 0.0f;
	double currentTime = getSeconds();
	double deltaTime = currentTime - prevSecond;
	prevSecond = currentTime;
	for (int i = 0; i < particles.size(); i++) {
		particles[i].timeAlive += deltaTime;
	}
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].timeAlive >= TIME_TO_KILL) {
			particles.erase(particles.begin() + i);
		}
	}

	for (int i = 0; i < particles.size(); i++) {
		particles[i].transform = glm::translate(particles[i].transform, glm::vec3(0.005f, 0.0f, 0.0f));
	}
	particles.push_back(particle);
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

void render(Model model, int numTri, GLuint program) {
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	loadUniformMat4(program, "view", view);
	loadUniformMat4(program, "transMat", model.getTrans());
	loadUniformMat4(program, "projection", projection);
	loadUniform3f(program, "Eye", eyex, eyey, eyez);
	loadUniform1i(program, "numLights", 1);
	loadUniform1i(program, "isEmiter", model.isEmiter()); 
	loadUniform4f(program, "base", model.getColour().x, model.getColour().y, model.getColour().z, 1.0f);
	for (int i = 0; i < numLights; i++) {
		std::string varName = "lightPos[" + std::to_string(i) + "]";
		loadUniform3f(program, varName, lights[i].lightPos.x, lights[i].lightPos.y, lights[i].lightPos.z);
		varName = "lightColor[" + std::to_string(i) + "]";
		loadUniform4f(program, varName, lights[i].lightColour.x, lights[i].lightColour.y, lights[i].lightColour.z, lights[i].lightColour.a);
		varName = "intensity[" + std::to_string(i) + "]";
		loadUniform1i(program, varName, lights[i].intensity);
	}


	loadUniform1i(program, "hdr_on", HDR_ON);

	

	double t1 = getSeconds();
	glDrawElements(GL_TRIANGLES, 3 * numTri, GL_UNSIGNED_INT, NULL);

}

void renderParticles() {
	glUseProgram(partProg);
	meshes[2].loadAttrib(partProg);
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	
	for (int i = 0; i < particles.size(); i++) {
		loadUniformMat4(partProg, "view", view);
		loadUniformMat4(partProg, "transMat", particles[i].transform);
		loadUniformMat4(partProg, "projection", projection);
		loadUniform3f(partProg, "base", particles[i].colour.x, particles[i].colour.y, particles[i].colour.z);
		glDrawElements(GL_TRIANGLES, 3 * meshes[2].getTriangles(), GL_UNSIGNED_INT, NULL);
	}
	
}

void display(void) {
	glEnable(GL_DEPTH);
	//Load the scene to a frame buffer and render it to a texture.
	glBindFramebuffer(GL_FRAMEBUFFER, hdrBuff);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(preProssProg);
	GLenum buff[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glViewport(0, 0, WIDTH, HEIGHT);
	glDrawBuffers(2, buff);
	GLuint status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << status << std::endl;
	}

	//render the spheres
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[0].objVAO);
	meshes[0].loadAttrib(preProssProg);
	render(models[0], meshes[0].getTriangles(), preProssProg);

	//Render the Dragon
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[1].objVAO);
	meshes[1].loadAttrib(preProssProg);
	render(models[1], meshes[1].getTriangles(), preProssProg);
	

	renderParticles();

	glBindFramebuffer(GL_FRAMEBUFFER, shaftBuff);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaftProg);
	GLenum shaftBuff[] = { GL_COLOR_ATTACHMENT0 };
	glViewport(0, 0, WIDTH, HEIGHT);
	glDrawBuffers(2, shaftBuff);

	meshes[0].loadAttrib(shaftProg);
	render(models[0], meshes[0].getTriangles(), shaftProg);

	meshes[1].loadAttrib(shaftProg);
	render(models[1], meshes[1].getTriangles(), shaftProg);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH); //depth buffer isn't needed for second render
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

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sunShaft);
	loadUniform1i(postProssProg, "shaftTex", 2);


	loadUniform1i(postProssProg, "hdr_on", HDR_ON);
	loadUniform1i(postProssProg, "bloom_on", BLOOM_ON);

	glm::mat4 view;
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 lightPosScreen = (projection * view * glm::vec4(models[0].getPos(), 1.0f)); //Convert the light's position into screen space
	loadUniform3f(postProssProg, "lightPos", lightPosScreen.x, lightPosScreen.y, lightPosScreen.z);

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
	Light sun;
	sun.lightPos = models[0].getPos();
	sun.lightColour = glm::vec4(models[0].getColour(), 1.0f);
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
	prevSecond = getSeconds();
	while (!glfwWindowShouldClose(window)) {
		update();
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteFramebuffers(1, &hdrBuff);
	glfwTerminate();

}
