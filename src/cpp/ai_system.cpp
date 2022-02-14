// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 500;

void AISystem::step(float elapsed_ms)
{
	ComponentContainer<AI>& ai_container = registry.ais;
	
	// Assuming 1 player entity
	Entity& player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);
	vec2 player_pos = player_motion.position;

	for (uint i = 0; i < ai_container.components.size(); i++) {
		
		// AI& ai = ai_container.components[i];
		Entity& entity = ai_container.entities[i];
		Motion& motion = registry.motions.get(entity);

		
		motion.angle = atan2(motion.position.x - player_pos.x, motion.position.y - player_pos.y);

		if (timer <= 0) {
			// checks minimum horizontal distance
			if (abs(motion.position.x - player_pos.x) > MIN_DISTANCE) {
				motion.velocity.x = 100 * -sin(motion.angle);
			}
			else {
				motion.velocity.x = 0;
			}

			timer = VELOCITY_CHANGE_DELAY;
		}
		if (jumpdelay <= 0) {
			motion.velocity.y = -100.f;
			jumpdelay = uniform_dist(rng) * 100000;
		}
		timer -= elapsed_ms;
		jumpdelay -= elapsed_ms;

	}
	
}

//initialize stuff here
void AISystem::init() {
	timer = 0;
	MIN_DISTANCE = 200;
	jumpdelay = 2000;
}