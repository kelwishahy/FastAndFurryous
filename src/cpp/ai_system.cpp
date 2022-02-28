// internal
#include "../src/hpp/ai_system.hpp"
#include <stdio.h>
#include "glm/ext.hpp"
#include <hpp/game_controller.hpp>
#include <hpp/tiny_ecs_registry.hpp>


using namespace glm;

//initialize stuff here
void AISystem::init() {
	ComponentContainer<AI>& ai_container = registry.ais;

	// player entity reference 
	// -- used for the direction of horizontal motion of the ai or aiming of the shooting
	Entity& player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);
	vec2 player_pos = player_motion.position;

	Entity& entity = ai_container.entities[0];

	ShootingSystem shootingSystem;

	BehaviorTree::Selector selector;
	AILeft* moveLeft = new AILeft(player_pos, entity, ai_mode);
	AIRight* moveRight = new AIRight(player_pos, entity, ai_mode);
	selector.addChild(moveLeft);
	selector.addChild(moveRight);
	// adding sequence of actions here -- so far only move
	//AIMove* move = new AIMove(player_pos, entity);
	AIShoot* shoot = new AIShoot(shootingSystem, entity);
	//sequence[0].addChild(move);
	root.addChild(&selector);
	root.addChild(shoot);
}
enum TURN_CODE {
	PLAYER1,
	PLAYER2,
	AI,
	NPCAI,
	END
};
void AISystem::step(float elapsed_time)
{
	if (turn_possesion == TURN_CODE::AI) {
		if (timer > 1500.f) {
			ai_mode = AI_MODE::MOVING;
		}
		else if (timer > 0.f) {
			ai_mode = AI_MODE::SHOOTING;
		}
		else {
			ai_mode = AI_MODE::IDLE;
		}
		root.run();
		root.clear();

		init();
		timer -= elapsed_time;
	}
}

void AISystem::updateGameState(uint new_turn) {
	turn_possesion = new_turn;
}