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
	}
	particleIndex = 0;
}

void ParticleSystem::emit(int numParticles, vec2 position, vec2 velocity) {
	std::random_device rseed;
	std::uniform_real_distribution<float> dist(-1, 1);
	std::mt19937 rng(rseed());
	for (int i = 0; i < numParticles; i++) {
		auto& particleEnt = registry.particles.entities[particleIndex];
		auto& particle = registry.particles.get(particleEnt);
		auto& motion = registry.motions.get(particleEnt);

		// Increment particle index
		particleIndex = (particleIndex < MAX_PARTICLES - 1) ? particleIndex + 1 : 0;

		// Something went wrong, this particle is still in use
		if (particle.active) {
			continue;
		}

		// Set the velocity & position of the particle
		motion.velocity = { velocity.x, velocity.y * dist(rng) };
		motion.position = position;

		// Set the particle to active
		particle.active = true;
	}
}

void ParticleSystem::step(float elapsed_ms) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		auto& particleEnt = registry.particles.entities[i];
		auto& particle = registry.particles.get(particleEnt);
	
		if (particle.active) {
			// decrement timer
			particle.timer -= elapsed_ms;
		}
	
		// Particle has reached end of life
		if (particle.timer <= 0) {
			deactivate(particle);
			particleIndex = i; // This is now a free particle
		}
	}
}

void ParticleSystem::deactivate(Particle& particle) {
	particle.active = false;
	particle.timer = 800; // reset timer
}

