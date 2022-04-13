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
#include "game_system.hpp"
#include "particle_system.hpp"

class GameController
{
public:
	GameController();

	// Releases all associated resources
	~GameController();

	void init(GLFWwindow* window, MapSystem::Map& map, OrthographicCamera& camera, TextManager& textManager, Game game_data,
	          ParticleSystem& particleSystem);

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	bool inAGame;
	bool quit = false;
	bool gameOver = false;

	GLFWwindow* window{};

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

	std::vector<GameController::TURN_CODE> playingTeams;
	std::vector<std::string> playingTeamNames;

	struct GameState {
		float timer = 0;
		uint turn_number = 0;
		uint turn_possesion = TURN_CODE::PLAYER1;
		int p1_team_curr_player = 0;
		int p2_team_curr_player = 0;
		int npc_team_curr_player = 0;
		int npcai_team_curr_player = 0;
	};

	TURN_CODE getTurnCode() { return (TURN_CODE)this->game_state.turn_possesion; }

	// Return the current selected character
	Entity& getSelectedCharacter() { return curr_selected_char; }

private:
	OrthographicCamera* camera;
	MapSystem::Map gameMap;
	TextManager textManager;
	Game game_data;
	ParticleSystem* particleSystem;
	Entity quitButton;

	glm::vec2 mousePosition{};
	float mouseDeadzone{};
	glm::vec2 mouseTriggerArea{};
	void moveCamera();

	// restart level it was in the private 
	void restart_current_match();

	void build_map();

	void init_player_teams();

	void on_player_key(int key, int, int action, int mod);
	void on_mouse_move(glm::vec2 mouse_pos);
	void on_mouse_click(int button, int action, int mods);
	void on_mouse_scroll(double xoffset, double yoffset);
	void set_user_input_callbacks();

	void next_turn();

	void handle_collisions();

	void check_out_of_screen_entities();

	void decrementTurnTime(float elapsed_ms);

	std::vector<std::vector<Entity>> teams;

	void change_selected_state(Entity e, bool state);
	void change_curr_selected_char(Entity e);
	void change_to_next_char_on_team();
	
	void createTutorialText();

	enum class PLAYER_MODE {
		MOVING,
		SHOOTING
	};
	PLAYER_MODE player_mode;

	GameState game_state;

	Entity curr_selected_char;

	ShootingSystem shooting_system;

	AISystem ai;
	Entity selected_ai;
	UISystem ui;

	float timePerTurnMs{};

	// Turn indicators & turn timer
	Entity timeIndicator;
	float timerScale{};

	Entity turnIndicator;
	float turnIndicatorScale{};
	glm::vec3 redColor = { 1.0, 0.0f, 0.0f };
	glm::vec3 blueColor = { 0.0, 0.0f, 1.0f };
	glm::vec3 darkGreenColor = { 0.0f, 0.4f, 0.0f };
	glm::vec2 turnPosition{};
};
