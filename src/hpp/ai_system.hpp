#pragma once

#include <random>
#include "decision_tree.hpp"
#include "Game_Mechanics/shooting_system.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, int turn, Entity *selected_ai, Entity last_player, std::vector<Entity> team);
	void init(ShootingSystem& shootingSystem, std::vector<Entity> team);
	double calculateDistance(vec2 v1, vec2 v2);
	void change_to_next_ai(Entity *selected_ai);
	void decrementTimer(float elapsed_ms);
	void checkJump();
	void allyNearby();
	void handle_collisions();

private:
	float jumpdelay;
	std::vector<Entity> ai_team;
	float aiTurnTime;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	Node* decisionTree;
	ShootingSystem shootingSystem;
	Mix_Chunk* gunshot;
};
