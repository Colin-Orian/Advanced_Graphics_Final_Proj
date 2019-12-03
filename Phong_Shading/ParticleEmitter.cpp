#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() {

}

ParticleEmitter::ParticleEmitter(glm::vec3 pos, glm::vec3 dir){
	srand(time(0));
	this->pos = pos;
	this->dir = dir;
	this->originalTransform = glm::scale(glm::mat4(), glm::vec3(0.15, 0.15, 0.15));
	this->originalTransform = glm::translate(this->originalTransform, this->pos);
}

std::vector<Particle> ParticleEmitter::getParticles()
{
	return particles;
}

void ParticleEmitter::createParticles(){
	Particle particle;
	particle.transform = originalTransform;
	float x = rand() % 10 /5.0f;
	float y = rand() % 10 / 5.0f;
	float z = rand() % 10 / 5.0f;
	particle.transform = glm::translate(particle.transform, glm::vec3(x, y, z));
	particle.colour = glm::vec3(1.0f, 0.0f, 0.0f);
	particle.timeAlive = 0.0f;
	particles.push_back(particle);
}

void ParticleEmitter::moveParticles(){
	for (int i = 0; i < particles.size(); i++) {
		float remainingTime = TIME_TO_KILL - particles[i].timeAlive;
		//As the particle spends more time 
		float variationX = rand() % (int)(1.0f / remainingTime) - 5.0f;
		float variationZ = rand() % (int)(1.0f / remainingTime) - 5.0f;
		glm::vec3 variation = glm::vec3(variationX, 0.0f, variationZ);
		particles[i].transform = glm::translate(particles[i].transform, this->dir + variation);
		particles[i].colour = glm::vec3(1.0f, 0.0f, 0.0f) * glm::vec3((TIME_TO_KILL - particles[i].timeAlive));
	}
}

void ParticleEmitter::deleteParticles(double deltaTime){
	for (int i = 0; i < particles.size(); i++) {
		particles[i].timeAlive += deltaTime;
	}
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].timeAlive >= TIME_TO_KILL) {
			particles.erase(particles.begin() + i);
		}
	}
}
