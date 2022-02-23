#pragma once

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"

// stlib
#include <vector>
#include <random>

#include <hpp/render_system.hpp>

using namespace glm;

#include <glm/vec2.hpp>	
#include <hpp/tiny_ecs_registry.hpp>

class GameController
{
public:
	GameController();

	// Releases all associated resources
	~GameController();

	void init(RenderSystem* renderer, GLFWwindow* window);

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	bool inAGame;

	GLFWwindow* window;


private:
	RenderSystem* renderer;

	// restart level it was in the private 
	void restart_current_match();

	void build_map();

	void init_player_teams();

	void on_player_key(int key, int, int action, int mod);

	void next_turn();

	std::vector<Entity> player1_team;
	std::vector<Entity> player2_team;
	std::vector<Entity> ai_team;
	std::vector<Entity> npcai_team;

	//Turn System stuff
	enum TURN_CODE {
		PLAYER1,
		PLAYER2,
		AI,
		NPCAI,
		END
	};
	struct GameState {
		float timer = 0;
		uint turn_number = 0;
		uint turn_possesion = TURN_CODE::PLAYER1;
	};
	GameState game_state;

	Entity curr_selected_char;

	// OpenGL window handle
	//GLFWwindow* window;
};