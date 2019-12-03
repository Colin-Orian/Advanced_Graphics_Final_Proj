#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>
#include <ctime>
struct Particle {
	glm::mat4 transform;
	glm::vec3 colour;
	double timeAlive;
};
class ParticleEmitter {
private:
	const double TIME_TO_KILL = 0.5;
	glm::vec3 pos;
	glm::vec3 dir;
	std::vector<Particle> particles;
	glm::mat4 originalTransform;
public:
	ParticleEmitter();
	ParticleEmitter(glm::vec3 pos, glm::vec3 dir);
	std::vector<Particle> getParticles();
	void createParticles();
	void moveParticles();
	void deleteParticles(double detlaTime);
};