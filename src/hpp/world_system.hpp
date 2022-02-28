#pragma once

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "game_controller.hpp"

using namespace glm;

#include <glm/vec2.hpp>				// vec2

//#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Releases all associated resources
	~WorldSystem();

	void init(RenderSystem* renderer, GLFWwindow* window);

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	// Check for collisions
	void handle_collisions();

	// Input callback functions
	void on_key(int button, int action, int mods);
	void on_mouse_move(vec2 pos);

	void check_for_button_presses();

	void play_tutorial(std::vector<std::function<void()>> callbacks);

	glm::vec2 mouse_pos;

	GameController getCurrentGame() { return this->current_game; }

private:
	// restart level it was in the private 
	void restart_game();

	void init_main_menu();

	RenderSystem* renderer;
	Entity player_cat;
	float current_speed;

	//AI stuff
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	std::default_random_engine rng;
	float next_ai_spawn;
	Entity ai_cat;

	//Game Controller
	GameController current_game;

	//Music references
	Mix_Music* background_music;

	// OpenGL window handle
	GLFWwindow* window;

	//Mouse press cooldown - I need to remove this later
	float cooldown = 0;
};
