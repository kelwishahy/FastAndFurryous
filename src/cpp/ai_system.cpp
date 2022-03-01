// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 3500;
//float counter_ms = 3000.f;

void AISystem::step(float elapsed_ms, int turn) {
	ComponentContainer<AI>& ai_container = registry.ais;
	for (uint i = 0; i < ai_container.components.size(); i++) {
		Entity& entity = ai_container.entities[i];
		Motion& motion = registry.motions.get(entity);

		blackboard->entity = &entity;
		blackboard->motion = &motion;
		blackboard->turn = turn;
		timer -= elapsed_ms;
		blackboard->timer = timer;

		if (timer <= 0.f) {
			timer = 1800.f;
		}

		decisionTree->traverse();
	}
	
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem, Mix_Chunk* gunshot) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	timer = 1800.f;
	direction = 1;
	jumpdelay = 2000;

	
	// Decision tree
	blackboard = new Blackboard;
	blackboard->velocity = 60.f;
	blackboard->shootingSystem = &this->shootingSystem;
	blackboard->gunshot = this->gunshot;
	decisionTree = new IsAITurn;

	// Tasks
	auto moveLeft = new MoveLeft;
	auto moveRight = new MoveRight;
	auto endTurn = new EndTurn;
	auto shoot = new Shoot;
	auto switchDirection = new SwitchDirection;

	// Decisions
	auto moving = new IsMoving;
	auto movingLeft = new IsMovingLeft;
	auto movingRight = new IsMovingRight;
	auto didTimeEnd = new DidTimeEnd;
	auto isNearEnemy = new IsNearEnemy;
	auto isHurt = new IsHurt;

	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(moving); // If ai turn, check if moving

	moving->addFalseConditionNode(moveLeft); // If not moving, move left
	moving->addTrueConditionNode(didTimeEnd); // If moving, check if the timer ended

	didTimeEnd->addFalseConditionNode(isHurt); // If time hasn't ended, check if hurt
	didTimeEnd->addTrueConditionNode(movingLeft); // If time ended check which direction ai was moving

	isHurt->addFalseConditionNode(nullptr); // If not hurt, do nothing
	isHurt->addTrueConditionNode(shoot); // If hurt, shoot

	shoot->addFalseConditionNode(nullptr);
	shoot->addTrueConditionNode(endTurn); // After shooting, end turn

	movingLeft->addFalseConditionNode(switchDirection); // If ai was moving right, move left
	movingLeft->addTrueConditionNode(switchDirection); // If ai was moving left, move right
}