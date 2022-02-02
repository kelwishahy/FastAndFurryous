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

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level it was in the private 
	void restart_game();

	RenderSystem* renderer;
	float current_speed;

	// OpenGL window handle
	//GLFWwindow* window;

	Entity player_cat;
};
