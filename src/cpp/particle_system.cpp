#include <hpp/particle_system.hpp>
#include <random>
#include "hpp/tiny_ecs_registry.hpp"

using namespace glm;

void ParticleSystem::init() {
	// Generate the reusable particles
	for (int i = 0; i < MAX_PARTICLES; i++) {
		auto particle = Entity();
		registry.particles.emplace(particle);
		registry.motions.emplace(particle);
		// transformations.push_back(mat4(1));
	}
	particleIndex = 0;
	glGenBuffers(1, &transformationsVBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * MAX_PARTICLES, transformations.data(), GL_STREAM_DRAW);
	glHasError();
	
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 0));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glEnableVertexAttribArray(9);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
}

void ParticleSystem::emit(int numParticles, vec2 position, vec2 velocity) {
	std::random_device rseed;
	std::uniform_real_distribution<float> dist(0, 1);
	std::uniform_real_distribution<float> dist2(-1, 0);
	std::mt19937 rng(rseed());
	for (int i = 0; i < numParticles; i++) {
		auto& particleEnt = registry.particles.entities[particleIndex];
		auto& particle = registry.particles.get(particleEnt);
		auto& motion = registry.motions.get(particleEnt);

		// Increment particle index
		particleIndex = ++particleIndex % MAX_PARTICLES;

		// Something went wrong, this particle is still in use
		if (particle.active) {
			continue;
		}

		// Set the velocity & position of the particle
		motion.velocity = { velocity.x * dist(rng), velocity.y * dist2(rng)};
		motion.position = position;

		// Set the particle to active
		particle.active = true;
		numActive++;
	}
}

void ParticleSystem::step(float elapsed_ms) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		auto& particleEnt = registry.particles.entities[i];
		auto& particle = registry.particles.get(particleEnt);
	
		if (particle.active) {
			// decrement timer
			particle.timer -= elapsed_ms;

			auto& motion = registry.motions.get(particleEnt);
			transformations.push_back(transform(
				motion.position,
				scaleToScreenResolution({ 5.f, 5.f }),
				1.f, 0));
		}
	
		// Particle has reached end of life
		if (particle.timer <= 0) {
			deactivate(i);
			particleIndex = i; // This is now a free particle
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numActive * sizeof(mat4), transformations.data());

	transformations.clear();
}

void ParticleSystem::deactivate(int index) {
	auto& particleEnt = registry.particles.entities[index];
	auto& particle = registry.particles.get(particleEnt);
	particle.active = false;
	particle.timer = 1000; // reset timer
	numActive--;
}

