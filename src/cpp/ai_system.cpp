// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;
;

void AISystem::step(float elapsed_ms, int turn, Entity *selected_ai, Entity last_player) {

	blackboard->turn = turn;
	if (ai_team.size() < 1) {
		return;
	}
	if (blackboard->turn == TURN_CODE::PLAYER1) {
		//blackboard->shot = false
		blackboard->timer = 3000.f;
		bool randbool = rand() & 1;
		blackboard->left = randbool;
		printf("left is %i \n", blackboard->left);

	}
	if (blackboard->turn == TURN_CODE::NPCAI) {
		change_to_next_ai();
		decrementTimer(elapsed_ms);
		//*selected_ai = blackboard->selected_ai;
		//if ((calculateDistance(blackboard->prev_pos, blackboard->motion->position) > 200)) {
		//	blackboard->motion->velocity.x = 0.f;
		//	blackboard->motion->velocity.y = 350.f;
		//	if (blackboard->shot) {
		//		ShootingSystem::shoot(*blackboard->entity);
		//	}
		//	//blackboard->shot = true;
		//	//blackboard->shootingSystem->shoot(*blackboard->entity);
		//	blackboard->turn = TURN_CODE::PLAYER1;
		//}
	}
	//checkJump();
	if (!blackboard->c->state_machine.getCurrentState()->next_turn) {
		decisionTree->traverse();
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
	auto leftRight = new LeftorRight;
	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(timeEnd); // If ai turn, check if moving timer has ended

	timeEnd->addFalseConditionNode(leftRight);
	timeEnd->addTrueConditionNode(shoot);

	leftRight->addFalseConditionNode(moveLeft);
	leftRight->addTrueConditionNode(moveRight);
}

double AISystem::calculateDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

void AISystem::change_to_next_ai()
{
	// change the selected ai to move 
	if (blackboard->ai_index == ai_team.size()) {
		blackboard->ai_index = 0;
	}
	blackboard->selected_ai = ai_team[blackboard->ai_index];
	Character* character = registry.characters.get(ai_team[blackboard->ai_index]);
	blackboard->c = character;
	blackboard->done_move = false;
	
	//blackboard->timer = 3000.f;
	//*blackboard->c = &registry.characters.get(ai_team[0]);
	// position where the ai ended its turn
}

void AISystem::decrementTimer(float elapsed_ms)
{
	blackboard->timer -= elapsed_ms;

	if (blackboard->timer <= 0.f) {
		blackboard->done_move = true;
	}
}


