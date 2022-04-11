#pragma once
#include "components.hpp"

class ParticleSystem {
public:
	ParticleSystem(){}
	~ParticleSystem(){}
	void init();
	void emit(int numParticles, glm::vec2 position, glm::vec2 velocity);
	void step(float elapsed_ms);
	void deactivate(Particle& particle);
private:
	int MAX_PARTICLES = 100;
	int particleIndex = 0; // Index to first free particle
};
