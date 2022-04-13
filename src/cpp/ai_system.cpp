// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;
;

void AISystem::step(float elapsed_ms, int turn, Entity *selected_ai, Entity last_player, std::vector<Entity> team) {

	blackboard->turn = turn;
	blackboard->player = &last_player;
	this->ai_team = team;
	if (ai_team.size() < 1) {
		return;
	}
	if (blackboard->turn == TURN_CODE::PLAYER1) {
		//blackboard->shot = false
		blackboard->timer = 3000.f;
		checkJump();
	}

	if (blackboard->turn == TURN_CODE::NPCAI) {
		decrementTimer(elapsed_ms);
		
	}
	//checkJump();

	if (!blackboard->c->state_machine.getCurrentState()->next_turn) {
		decisionTree->traverse();
		change_to_next_ai(selected_ai);
	}
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem, std::vector<Entity> team) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	ai_team = team;

	// Decision tree
	blackboard = new Blackboard;
	blackboard->timer = 3000.f;

	if (ai_team.size() < 1) {
		return;
	}

	blackboard->selected_ai = ai_team[0];
	Character *character = registry.characters.get(ai_team[0]);
	blackboard->c = character;
	blackboard->ai_index = 0;
	decisionTree = new IsAITurn;

	auto endTurn = new EndTurn;
	auto moveRight = new MoveRight;
	auto moveLeft = new MoveLeft;
	auto timeEnd = new DidTimeEnd;
	auto shoot = new Shoot;

	auto hpLower = new HpLower;
	auto charge = new Charge;
	auto runaway = new RunAway;
	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(timeEnd); // If ai turn, check if moving timer has ended

	timeEnd->addFalseConditionNode(hpLower);
	timeEnd->addTrueConditionNode(shoot);

	hpLower->addFalseConditionNode(charge);
	hpLower->addTrueConditionNode(runaway);
}

double AISystem::calculateDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

void AISystem::change_to_next_ai(Entity *selected_ai)
{
	// change the selected ai to move 
	//blackboard->ai_index+=1;
	if (blackboard->ai_index >= (int)ai_team.size()) {
		blackboard->ai_index = 0;

	}
	blackboard->selected_ai = ai_team[blackboard->ai_index];
	Character* character = registry.characters.get(ai_team[blackboard->ai_index]);
	blackboard->c = character;
	blackboard->done_move = false;
	//blackboard->jump = false;
	blackboard->friendly = false;
	//Motion& motion = registry.motions.get(blackboard->selected_ai);
	//blackboard->motion = &motion;

	*selected_ai = ai_team[blackboard->ai_index];

}

void AISystem::decrementTimer(float elapsed_ms)
{
	blackboard->timer -= elapsed_ms;

	if (blackboard->timer <= 0.f) {
		blackboard->done_move = true;
	}
}

void AISystem::checkJump() {
	bool randbool = rand() & 1;
	blackboard->jump = randbool;
}

void AISystem::handle_collisions() {
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.ais.has(entity)) {
			if (registry.ais.has(entity_other)) {
				blackboard->jump = true;
			}
		}
	}
}

//void AISystem::allyNearby() {
//	WeaponBase& weapon = registry.weapons.get(blackboard->selected_ai);
//	Motion& motion = *blackboard->motion;
//
//	float x_end;
//	//right facing
//	if (!registry.animations.get(blackboard->selected_ai).facingLeft) {
//		x_end = motion.position.x + 100.f;
//		//x_begin = motion.position.x + weapon.distance - weapon.area;
//		
//		for (Entity entity : ai_team) {
//			Motion& other_motion = registry.motions.get(entity);
//			if (blackboard->selected_ai == entity) {
//				continue;
//			}
//			if (x_end > other_motion.position.x) {
//				blackboard->friendly = true;
//				break;
//			}
//		}
//	}
//	//left facing
//	else {
//		x_end = motion.position.x - 100.f;
//		//x_begin = motion.position.x - weapon.distance + weapon.area;
//		for (Entity entity : ai_team) {
//			if (blackboard->selected_ai == entity) {
//				continue;
//			}
//			Motion& other_motion = registry.motions.get(entity);
//
//			if (x_end < other_motion.position.x) {
//				blackboard->friendly = true;
//			}
//		}
//	}
//
//}


