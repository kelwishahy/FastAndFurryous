// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>
#include "../behavior_tree/sequence.hpp"
#include "glm/ext.hpp"

using namespace glm;

//initialize stuff here
void AISystem::init() {

	timer = 0;
	jumpdelay = 2000;
}

const uint VELOCITY_CHANGE_DELAY = 3500;

void AISystem::step(float elapsed_ms)
{
	ComponentContainer<AI>& ai_container = registry.ais;

	Entity& player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);
	vec2 player_pos = player_motion.position;

	Entity& entity = ai_container.entities[0];
	//Sequence* sequence1 = new Sequence;
	AIMove* move = new AIMove(player_pos, entity);

	sequence[0].addChild(move);

	while (!sequence->run()) {
		//printf("------------------------------\n");
	}

	//for (uint i = 0; i < ai_container.components.size(); i++) {

	//	// AI& ai = ai_container.components[i];
	//	Entity& entity = ai_container.entities[i];
	//	//Sequence* sequence1 = new Sequence;
	//	AIMove* move = new AIMove(player_pos, entity);

	//	sequence[0].addChild(move);

	//	while (!sequence->run()) {
	//		//printf("------------------------------\n");
	//	}
	//}
}