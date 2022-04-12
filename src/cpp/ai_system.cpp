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
	if (blackboard->turn == TURN_CODE::NPCAI) {
		decideAction();
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

	decisionTree->traverse();
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem, std::vector<Entity> team) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	ai_team = team;
	aiTurnTime = 5000.f;
	// Decision tree
	blackboard = new Blackboard;

	if (ai_team.size() < 1) {
		return;
	}

	blackboard->selected_ai = ai_team[0];
	Character *character = registry.characters.get(ai_team[0]);
	blackboard->c = character;
	decisionTree = new IsAITurn;

	auto endTurn = new EndTurn;
	auto moveRight = new MoveRight;
	auto moveLeft = new MoveLeft;

	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(moveRight); // If ai turn, check if moving
}

double AISystem::calculateDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

void AISystem::decideAction()
{

	// change the selected ai to move 
	for (int i = 0; i < ai_team.size(); i++) {
		if (blackboard->selected_ai == ai_team[i]) {
			if (i + 1 == ai_team.size()) {
				blackboard->selected_ai = ai_team[0];
			}
			else {
				blackboard->selected_ai = ai_team[i + 1];
			}
		}
	}

	//*blackboard->c = &registry.characters.get(ai_team[0]);
	// position where the ai ended its turn
}

void AISystem::checkJump()
{

}


