#pragma once

// internal
#include "tiny_ecs.hpp"
#include "map.hpp"

// stlib
#include <vector>

#include "ai_system.hpp"
#include "orthographic_camera.hpp"

#include <hpp/Game_Mechanics/shooting_system.hpp>
#include <hpp/ui_system.hpp>

#include "text_manager.hpp"
#include "GLFW/glfw3.h"

class GameController
{
public:
	GameController();

	// Releases all associated resources
	~GameController();

	void init(GLFWwindow* window, MapSystem::Map& map, OrthographicCamera& camera, TextManager& textManager);

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	bool inAGame;

	GLFWwindow* window;

	MapSystem::Map& getGameMap() { return gameMap; }
	OrthographicCamera& getCamera() { return *camera; }

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

	// Return the current selected character
	Entity& getSelectedCharacter() { return curr_selected_char; }

private:
	OrthographicCamera* camera;
	MapSystem::Map gameMap;
	TextManager textManager;
	// restart level it was in the private 
	void restart_current_match();

	void build_map();

	void init_player_teams();

	void on_player_key(int key, int, int action, int mod);
	void on_mouse_move(glm::vec2 mouse_pos);
	void on_mouse_click(int button, int action, int mods);
	void set_user_input_callbacks();

	void next_turn();

	void handle_collisions();

	void decrementTurnTime(float elapsed_ms);

	std::vector<Entity> player1_team;
	std::vector<Entity> player2_team;
	std::vector<Entity> ai_team;
	std::vector<Entity> npcai_team;
	std::vector<std::vector<Entity>> teams;
	

	enum class PLAYER_MODE {
		MOVING,
		SHOOTING
	};
	PLAYER_MODE player_mode;

	GameState game_state;

	Entity turnIndicator;

	Entity timeIndicator;

	Entity curr_selected_char;

	ShootingSystem shooting_system;

	AISystem ai;

	UISystem ui;

	uint numPlayersInTeam;
	float timePerTurnMs;
};
