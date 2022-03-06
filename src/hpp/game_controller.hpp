#pragma once

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "map.hpp"

// stlib
#include <vector>
#include <random>

#include <hpp/render_system.hpp>

#include "ai_system.hpp"

using namespace glm;

#include <glm/vec2.hpp>	
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/Game_Mechanics/shooting_system.hpp>

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

	Map getGameMap() { return gameMap; }

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

	TURN_CODE getTurnCode() { return (TURN_CODE)this->game_state.turn_possesion; }


private:
	RenderSystem* renderer;

	// restart level it was in the private 
	void restart_current_match();

	void build_map();

	void init_player_teams();

	void on_player_key(int key, int, int action, int mod);
	void GameController::on_mouse_move(glm::vec2 mouse_pos);

	void next_turn();

	void handle_collisions();

	void decrementTurnTime(float elapsed_ms);

	std::vector<Entity> player1_team;
	std::vector<Entity> player2_team;
	std::vector<Entity> ai_team;
	std::vector<Entity> npcai_team;
	std::vector<std::vector<Entity>> teams;
	Map gameMap;

	enum class PLAYER_MODE {
		MOVING,
		SHOOTING
	};
	PLAYER_MODE player_mode;

	GameState game_state;

	Entity curr_selected_char;

	ShootingSystem shooting_system;

	AISystem ai;

	uint numPlayersInTeam;
	float timePerTurnMs;

};