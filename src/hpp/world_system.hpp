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

	void play_tutorial(std::vector<std::function<void()>> callbacks);

	void play_select(std::vector<std::function<void()>> callbacks);

	void play_startscreen(std::vector<std::function<void()>> callbacks);

	void play_options(std::vector<std::function<void()>> callbacks);

	void play_levels(std::vector<std::function<void()>> callbacks);

	glm::vec2 mouse_pos;

	GameController& getCurrentGame() { return this->current_game; }

	MapSystem& getMapSystem() { return this->mapSystem; }

	OrthographicCamera camera;

	TextManager& getTextManager() { return this->textManager; }

private:
	// restart level it was in the private 
	void restart_game();

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
};
