#pragma once

#include <random>
#include "decision_tree.hpp"
#include "Game_Mechanics/shooting_system.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, int turn);
	void init(ShootingSystem& shootingSystem);

private:
	float jumpdelay;
	float timer;
	int direction;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	Node* decisionTree;
	ShootingSystem shootingSystem;
	Mix_Chunk* gunshot;
};
