// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 3500;

void AISystem::step(float elapsed_ms)
{
	ComponentContainer<AI>& ai_container = registry.ais;
	for (uint i = 0; i < ai_container.components.size(); i++) {
		
		AI& ai = ai_container.components[i];
		Entity& entity = ai_container.entities[i];

		Motion& motion = registry.motions.get(entity);

		if (timer <= 0) {
			motion.velocity.x = direction * uniform_dist(rng) * 100;
			timer = VELOCITY_CHANGE_DELAY;
			direction *= -1;
		}
		if (jumpdelay <= 0) {
			motion.velocity.y = -300.f;
			jumpdelay = uniform_dist(rng) * 100000;
		}
		timer -= elapsed_ms;
		jumpdelay -= elapsed_ms;

	}
	
}

//initialize stuff here
void AISystem::init() {
	timer = 0;
	direction = 1;
	jumpdelay = 2000;
}