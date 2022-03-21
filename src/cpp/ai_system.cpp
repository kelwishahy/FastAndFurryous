// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 3500;
//float counter_ms = 3000.f;

void AISystem::step(float elapsed_ms, int turn) {
	ComponentContainer<AI>& ai_container = registry.ais;

	blackboard->turn = turn;
	timer -= elapsed_ms;
	blackboard->timer = timer;

	decideAction();
	if (timer <= 0.f) {
		timer = 3000.f;
	}

	//checkJump();

	decisionTree->traverse();
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	timer = 3000.f;
	direction = 1;
	jumpdelay = 2000;

	// Decision tree
	blackboard = new Blackboard;
	blackboard->velocity = 60.f;
	blackboard->shootingSystem = &this->shootingSystem;
	blackboard->lowerhp = false;
	//blackboard->
	decisionTree = new IsAITurn;

	// Tasks
	auto move = new Move;
	auto endTurn = new EndTurn;
	auto shoot = new Shoot;
	auto switchDirection = new SwitchDirection;
	// Decisions
	auto moving = new IsMoving;
	//auto movingLeft = new IsMovingLeft;
	auto movingRight = new IsMovingRight;
	auto didTimeEnd = new DidTimeEnd;
	//auto isNearEnemy = new IsNearEnemy;
	auto isHurt = new IsHurt;
	auto isshot = new Shot;

	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(moving); // If ai turn, check if moving

	moving->addFalseConditionNode(move); // If not moving, move left
	moving->addTrueConditionNode(didTimeEnd); // If moving, check if the timer ended


	//isshot->addFalseConditionNode(hpLower);
	//isshot->addTrueConditionNode(endTurn);

	//hpLower->addFalseConditionNode(charge);
	//hpLower->addTrueConditionNode(runAway);

	move->addTrueConditionNode(isHurt);


	didTimeEnd->addFalseConditionNode(isHurt); // If time hasn't ended, check if hurt
	didTimeEnd->addTrueConditionNode(movingRight); // If time ended check which direction ai was moving

	isHurt->addFalseConditionNode(nullptr); // If not hurt, do nothing
	isHurt->addTrueConditionNode(shoot); // If hurt, shoot

	shoot->addFalseConditionNode(endTurn);
	shoot->addTrueConditionNode(endTurn); // After shooting, end turn

	movingRight->addFalseConditionNode(isHurt); // If ai was moving right, move left
	movingRight->addTrueConditionNode(switchDirection); // If ai was moving left, move right

	//switchDirection->addTrueConditionNode(shoot);
}

double AISystem::calculateDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

void AISystem::decideAction()
{

	// decide which ai to move based on hp -> position
	ComponentContainer<AI>& ai_container = registry.ais;

	Entity& lowest_ai = ai_container.entities[0];
	// find the lowest hp cat
	for (uint i = 0; i < ai_container.components.size(); i++) {
		Entity& entity = ai_container.entities[i];
		if (registry.health.get(lowest_ai).hp > registry.health.get(entity).hp) {

			lowest_ai = entity;
		}
	}

	blackboard->entity = &lowest_ai;

	// decide whether to move the ai away or to the player
	Motion& motion = registry.motions.get(lowest_ai);
	blackboard->motion = &motion;

	ComponentContainer<Player>& player_container = registry.players;
	Entity& closest_player = player_container.entities[0];


	for (uint j = 0; j < player_container.components.size(); j++) {
		Entity& player = player_container.entities[j];
		Motion& player_motion = registry.motions.get(player);
		
	
		Motion& closest_motion = registry.motions.get(closest_player);

		double distance_1 = calculateDistance(closest_motion.position, motion.position);
		double distance_2 = calculateDistance(player_motion.position, motion.position);

		if (distance_2 < distance_1) {
			closest_player = player;
		}
	}
	blackboard->player = &closest_player;
	if (registry.health.get(lowest_ai).hp < registry.health.get(closest_player).hp) {
		blackboard->lowerhp = true;
	}
	else {
		blackboard->lowerhp = false;
	}
}

void AISystem::checkJump()
{
	Motion& motion = *blackboard->motion;

	if (motion.velocity.x == 0 || motion.velocity.y < 0) {
		motion.velocity.y = 35.f;
		return;
	}

	if (motion.velocity.x > 0) {
		if (blackboard->prev_pos.x >= motion.position.x) {
			motion.velocity.y = -50.f;
		}
	}
	else if (motion.velocity.x < 0) {
		if (blackboard->prev_pos.x <= motion.position.x) {
			motion.velocity.y = -50.f;
		}
	}
}

