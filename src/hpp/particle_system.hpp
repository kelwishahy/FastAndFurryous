#pragma once
#include "components.hpp"

class ParticleSystem {
public:
	ParticleSystem(){}
	~ParticleSystem(){}
	void init();
	void emit(int numParticles, glm::vec2 position, glm::vec2 velocity);
	void step(float elapsed_ms);
	void deactivate(Particle& particle, int index);
	std::vector<glm::mat4> transformations;
	int getNumActive() { return this->numActive; }
private:
	unsigned int transformationsVBO;
	int MAX_PARTICLES = 300;
	int particleIndex = 0; // Index to first free particle
	int numActive = 0;
};
