#pragma once

// internal
#include "tiny_ecs.hpp"

// stlib
#include <vector>
#include <random>
#include "game_controller.hpp"
#include <glm/vec2.hpp>
#include "orthographic_camera.hpp"
#include "text_manager.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Releases all associated resources
	~WorldSystem();

	void init(GLFWwindow* window);

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	// Check for collisions
	void handle_collisions();

	// Input callback functions
	void on_key(int button, int action, int mods);
	void on_mouse_move(glm::vec2 pos);
	void on_mouse_click(int button, int action, int mods);
	void set_user_input_callbacks();

	void check_for_button_presses();

	void play_tutorial();

	void play_select();

	void play_startscreen();

// change option timer type
	void play_options(float newtimer, int newPlayers);

	void play_levels();

	void remove_components();

	glm::vec2 mouse_pos;

	GameController& getCurrentGame() { return this->current_game; }

	MapSystem& getMapSystem() { return this->mapSystem; }

	OrthographicCamera& getCamera() { return this->camera; }

	TextManager& getTextManager() { return this->textManager; }

	Game level_one();
	Game level_two();
	Game level_three();
	Game multiplayer(float newtimer);

private:
	// restart level it was in the private 
	void restart_game(Game level);

	void init_main_menu();

	Entity player_cat;
	float current_speed;

	//AI stuff
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	std::default_random_engine rng;
	float next_ai_spawn;
	Entity ai_cat;

	//Game Controller
	GameController current_game;

	// OpenGL window handle
	GLFWwindow* window;

	MapSystem mapSystem;

	TextManager textManager;

	OrthographicCamera camera;
};
