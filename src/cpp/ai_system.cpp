// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>
#include "glm/ext.hpp"
#include <hpp/game_controller.hpp>
#include <hpp/tiny_ecs_registry.hpp>


using namespace glm;

//initialize stuff here
void AISystem::init(GameState gamestate) {

	gameState = gamestate;
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

	ShootingSystem shootingSystem;
	
	BehaviorTree::Selector selector;
	AILeft* moveLeft = new AILeft(player_pos, entity);
	AIRight* moveRight = new AIRight(player_pos, entity);
	selector.addChild(moveLeft);
	selector.addChild(moveRight);
	// adding sequence of actions here -- so far only move
	//AIMove* move = new AIMove(player_pos, entity);
	AIShoot* shoot = new AIShoot(shootingSystem, entity);
	//sequence[0].addChild(move);
	sequence.addChild(&selector);
	//sequence.addChild(shoot);
	//Sequence* sequence1 = new Sequence;
	
	while (!sequence.run()) {
		//printf("------------------------------\n");
	}
	// delete the actions done from the decision tree
	sequence.clear();

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