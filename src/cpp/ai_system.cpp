// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/decision_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 3500;
//float counter_ms = 3000.f;

void AISystem::step(float elapsed_ms, int turn) {
	ComponentContainer<AI>& ai_container = registry.ais;

	//assert(ai_container.size() != 0);

	blackboard->turn = turn;
	timer -= elapsed_ms;
	blackboard->timer = timer;

	if (timer <= 0.f) {
		timer = 3000.f;
	}
	if (blackboard->turn == TURN_CODE::PLAYER1_TURN) {
		decideAction();
	}
	if (blackboard->turn == TURN_CODE::NPCAI_TURN) {
		checkJump();
		if (calculateDistance(blackboard->prev_pos, blackboard->motion->position) > 600) {
			blackboard->motion->velocity.x = 0.f;
			blackboard->motion->velocity.y = 350.f;
			blackboard->turn = TURN_CODE::PLAYER1_TURN;
		}
	}
	//checkJump();

	decisionTree->traverse();
}

//initialize stuff here
void AISystem::init(ShootingSystem& shootingSystem, std::vector<Entity> team, std::vector<Entity> npcai_team) {
	this->shootingSystem = shootingSystem;
	this->gunshot = gunshot;
	player1_team = team;
	this->npcai_team = npcai_team;
	timer = 3000.f;
	direction = 1;
	jumpdelay = 2000;
	aiTurnTime = 5000.f;
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
	auto hpLower = new HpLower;
	auto moveLeft = new MoveLeft;
	auto moveRight = new MoveRight;
	// Build the tree
	decisionTree->addFalseConditionNode(endTurn); // If not ai turn, skip
	decisionTree->addTrueConditionNode(moving); // If ai turn, check if moving

	moving->addFalseConditionNode(hpLower); // If not moving, move left
	moving->addTrueConditionNode(didTimeEnd); // If moving, check if the timer ended


	//isshot->addFalseConditionNode(hpLower);
	//isshot->addTrueConditionNode(endTurn);

	hpLower->addFalseConditionNode(moveLeft);
	hpLower->addTrueConditionNode(moveRight);

	//move->addTrueConditionNode(isHurt);


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
	/*ComponentContainer<AI>& ai_container = registry.ais;

	Entity& lowest_ai = ai_container.entities[0];*/
	printf("ai 1 \n");

	Entity& lowest_ai = npcai_team[0];
	printf("ai 2\n");

	// find the lowest hp cat
	for (Entity entity : npcai_team) {
		if (registry.health.get(lowest_ai).hp > registry.health.get(entity).hp) {

			lowest_ai = entity;
		}
	}
	/*for (uint i = 0; i < ai_container.components.size(); i++) {
		Entity& entity = ai_container.entities[i];
		if (registry.health.get(lowest_ai).hp > registry.health.get(entity).hp) {

			lowest_ai = entity;
		}
	}*/

	blackboard->entity = &lowest_ai;
	printf("ai 3 \n");

	// decide whether to move the ai away or to the player
	Motion& motion = registry.motions.get(lowest_ai);
	blackboard->motion = &motion;

	/*ComponentContainer<Player>& player_container = registry.players;
	Entity& closest_player = player_container.entities[0];*/
	Entity& closest_player = player1_team[0];


	for (uint j = 0; j < player1_team.size(); j++) {
		Entity& player = player1_team[j];
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

	blackboard->prev_pos = motion.position;
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
			motion.velocity.y = -500.f;
		}
	}
	else if (motion.velocity.x < 0) {
		if (blackboard->prev_pos.x <= motion.position.x) {
			motion.velocity.y = -500.f;
		}
	}
}

void AISystem::endAITurn(float elapsed_ms)
{
	uint timePerTurnSec = uint(aiTurnTime / 1000);
	if (aiTurnTime <= 0) {
		blackboard->turn = TURN_CODE::PLAYER1_TURN;
		aiTurnTime = 5000.f;
	}
	else {
		aiTurnTime -= elapsed_ms;
	}
}

