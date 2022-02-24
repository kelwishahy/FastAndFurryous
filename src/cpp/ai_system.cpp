// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>
#include "../behavior_tree/sequence.hpp"
#include "glm/ext.hpp"

using namespace glm;

//initialize stuff here
void AISystem::init() {

	/*timer = 0;
	jumpdelay = 2000;*/
}

const uint VELOCITY_CHANGE_DELAY = 3500;

void AISystem::step(float elapsed_ms)
{
	ComponentContainer<AI>& ai_container = registry.ais;

	// player entity reference 
	// -- used for the direction of horizontal motion of the ai or aiming of the shooting
	Entity& player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);
	vec2 player_pos = player_motion.position;

	Entity& entity = ai_container.entities[0];
	//Sequence* sequence1 = new Sequence;
	// adding sequence of actions here -- so far only move
	AIMove* move = new AIMove(player_pos, entity);

	sequence[0].addChild(move);

	// runs each child in sequence till it fails or runs thru each child
	while (!sequence->run()) {
		//printf("------------------------------\n");
	}

	// delete the actions done from the decision tree
	sequence[0].clear();

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