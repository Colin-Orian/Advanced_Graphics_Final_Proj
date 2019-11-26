#include "Model.h"

Model::Model() {
}

Model::Model(Mesh mesh) {
	this->mesh = mesh;
	transMat = glm::mat4x4(1.0f);
	sightDistance = 2.0f;
	this->emiter = 0;
}

void Model::setColour(glm::vec3 colour) {
	this->colour = colour;
}

glm::vec3 Model::getColour()
{
	return colour;
}

void Model::setTrans(glm::mat4x4 transMat) {
	this->transMat = transMat;
}

glm::mat4x4 Model::getTrans() {
	return transMat;
}

void Model::setVelocity(glm::vec3 velocity) {
	this->velocity = velocity;
}

glm::vec3 Model::getVelocity()
{
	return velocity;
}

glm::vec3 Model::getPos() {

	return glm::vec3(transMat[3]);
}

//Transformation functions
void Model::translate(glm::vec3 translateVec) {
	transMat = glm::translate(transMat, translateVec);
}

void Model::scale(glm::vec3 scaleVec) {
	transMat = glm::scale(transMat, scaleVec);
}

int Model::isEmiter()
{
	return emiter;
}

void Model::toggleEmiter()
{
	if (emiter == 1) {
		emiter = 0;
	}
	else {
		emiter = 1;
	}
}
