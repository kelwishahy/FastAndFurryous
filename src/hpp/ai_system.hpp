#pragma once

#include <random>
#include "behaviour_tree.hpp"

class AISystem
{
public:
	void step(float elapsed_ms);
	void init();

private:
	float jumpdelay;
	float timer;
	int direction;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	Node* behaviourTree;
};