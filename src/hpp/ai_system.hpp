#pragma once

#include <random>
#include "decision_tree.hpp"
#include "Game_Mechanics/shooting_system.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, int turn);
	void init(ShootingSystem& shootingSystem, std::vector<Entity> team, std::vector<Entity> npcai_team);
	double calculateDistance(vec2 v1, vec2 v2);
	void decideAction();
	void checkJump();
	void endAITurn(float elapsed_ms);
private:
	float jumpdelay;
	float timer;
	int direction;
	std::vector<Entity> player1_team;
	std::vector<Entity> npcai_team;

	float aiTurnTime;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	Node* decisionTree;
	ShootingSystem shootingSystem;
	Mix_Chunk* gunshot;
};
