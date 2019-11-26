#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
class Model {
private:
	Mesh mesh;
	glm::mat4x4 transMat;
	glm::vec3 colour;

	glm::vec3 velocity;
	float sightDistance;
	int emiter;
public:
	Model();
	Model(Mesh mesh);

	void setColour(glm::vec3 colour);

	glm::vec3 getColour();

	void setTrans(glm::mat4x4 transMat);

	glm::mat4x4 getTrans();

	void setVelocity(glm::vec3 velocity);

	glm::vec3 getVelocity();

	glm::vec3 getPos();

	void translate(glm::vec3 translateVec);

	void scale(glm::vec3 scaleVec);

	int isEmiter();

	void toggleEmiter();
};