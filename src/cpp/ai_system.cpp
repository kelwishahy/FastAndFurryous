// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;
;

void AISystem::step(float elapsed_ms, int turn, Entity *selected_ai) {

	blackboard->turn = turn;
	timer -= elapsed_ms;
	blackboard->timer = timer;

	if (timer <= 0.f) {
		blackboard->shot = true;
		timer = 5000.f;
	}
	if (blackboard->turn == TURN_CODE::PLAYER1) {
		decideAction();
		//blackboard->shot = false
	}
	*selected_ai = *blackboard->entity;

	if (blackboard->turn == TURN_CODE::NPCAI) {
		checkJump();

		if ((calculateDistance(blackboard->prev_pos, blackboard->motion->position) > 200)) {
			blackboard->motion->velocity.x = 0.f;
			blackboard->motion->velocity.y = 350.f;
			if (blackboard->shot) {
				blackboard->shootingSystem->shoot(*blackboard->entity);
			}
			//blackboard->shot = true;
			//blackboard->shootingSystem->shoot(*blackboard->entity);
			blackboard->turn = TURN_CODE::PLAYER1;
		}
	}
	//checkJump();

	decisionTree->traverse();
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem, std::vector<Entity> team) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	player1_team = team;
	jumped = false;
	timer = 3000.f;
	direction = 1;
	jumpdelay = 2000.f;
	aiTurnTime = 5000.f;
	// Decision tree
	blackboard = new Blackboard;
	blackboard->velocity = 60.f;
	blackboard->shootingSystem = &this->shootingSystem;
	blackboard->lowerhp = false;
	decisionTree = new IsAITurn;

	// Tasks
	auto move = new Move;
	auto endTurn = new EndTurn;
	auto switchDirection = new SwitchDirection;
	auto charge = new Charge;
	auto runAway = new RunAway;
	// Decisions
	auto moving = new IsMoving;
	auto movingRight = new IsMovingRight;
	auto didTimeEnd = new DidTimeEnd;
	auto hpLower = new HpLower;

	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(moving); // If ai turn, check if moving

	moving->addFalseConditionNode(hpLower); // If not moving, check if hp is lower than 50
	moving->addTrueConditionNode(didTimeEnd); // If moving, check if the timer ended
	
	hpLower->addFalseConditionNode(runAway); // if hp is lower than 50, run away from the player
	hpLower->addTrueConditionNode(charge); // if h is higher than 50, get closer to the player

	didTimeEnd->addFalseConditionNode(nullptr); // If time hasn't ended, check if to shoot
	didTimeEnd->addTrueConditionNode(movingRight); // If time ended check which direction ai was moving

	//movingRight->addTrueConditionNode(switchDirection); // If ai was moving left, move right
}

double AISystem::calculateDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

void AISystem::decideAction()
{

	// decide which ai to move based on position
	ComponentContainer<AI>& ai_container = registry.ais;

	if (ai_container.size() < 1) {
		return;
	}
	Entity& closest_ai = ai_container.entities[0];
	Entity& player = player1_team[0];
	Motion& player_motion = registry.motions.get(player);
	blackboard->player = &player;
	blackboard->shot = false;
	// find the closest ai to the player
	for (uint i = 0; i < ai_container.components.size(); i++) {
		Entity& entity = ai_container.entities[i];
		Motion& motion = registry.motions.get(entity);
		Motion& closest_motion = registry.motions.get(closest_ai);
		double distance_1 = calculateDistance(closest_motion.position, player_motion.position);
		double distance_2 = calculateDistance(motion.position, player_motion.position);
		if (distance_1 > distance_2) {
			closest_ai = entity;
		}
	}

	// assign the closest ai to move for the turn
	blackboard->entity = &closest_ai;
	Motion& motion = registry.motions.get(closest_ai);
	blackboard->motion = &motion;

	if (registry.health.get(closest_ai).hp < 50) {
		blackboard->lowerhp = true;
		blackboard->attack = false;
	}
	else {
		blackboard->lowerhp = false;
		blackboard->attack = true;

	}

	// position where the ai ended its turn
	blackboard->prev_pos = motion.position;
}

void AISystem::checkJump()
{
	Motion& motion = *blackboard->motion;
	
	// check if the ai is alreadyjumping
	if (motion.velocity.x == 0 || motion.velocity.y < 0) {
		motion.velocity.y = 35.f;
		return;
	}

	// check if the ai is moving horizontally:
	// if the position has not changed and there is a tile blocking its way, make it jump
	if (motion.velocity.x > 0) {
		if (blackboard->prev_pos.x >= motion.position.x) {
			motion.velocity.y = -150.f;
		}
	}
	else if (motion.velocity.x < 0) {
		if (blackboard->prev_pos.x <= motion.position.x) {
			motion.velocity.y = -150.f;
		}
	}
}


