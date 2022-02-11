#pragma once

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"

// stlib
#include <vector>
#include <random>

using namespace glm;

#include <glm/vec2.hpp>	

class GameController
{
public:
	GameController();

	// Releases all associated resources
	~GameController();

	void init();

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	int testint;

private:
	// restart level it was in the private 
	void restart_current_match();

	std::vector<Entity> player1_team;
	std::vector<Entity> player2_team;
	std::vector<Entity> ai_team;
	std::vector<Entity> npcai_team;

	//Turn System stuff
	enum class TURN_CODE {
		PLAYER1,
		PLAYER2,
		AI,
		NPCAI
	};
	struct game_state {
		float timer = 0;
		uint turn_number = 0;
		TURN_CODE turn_possesion = TURN_CODE::PLAYER1;
	};

	// OpenGL window handle
	//GLFWwindow* window;
};