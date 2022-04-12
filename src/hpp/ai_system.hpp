#pragma once

#include <random>
#include "decision_tree.hpp"
#include "Game_Mechanics/shooting_system.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, int turn, Entity *selected_ai, Entity last_player);
	void init(ShootingSystem& shootingSystem, std::vector<Entity> team);
	double calculateDistance(vec2 v1, vec2 v2);
	void decideAction();
	void checkJump();

private:
	float jumpdelay;
	float timer;
	std::vector<Entity> ai_team;
	float aiTurnTime;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	Node* decisionTree;
	ShootingSystem shootingSystem;
	Mix_Chunk* gunshot;
};
