#pragma once

#include <chrono>

#include "tiny_ecs.hpp"
#include "hpp/common.hpp"

#include "GLFW/glfw3.h"


class CharacterState {

public:

	CharacterState(Entity e, GLFWwindow* window);
	virtual ~CharacterState();

	virtual void enter();
	virtual void exit();
	virtual void step(float elapsed_ms);
	virtual void doChecks();

	virtual void on_player_key(int key, int, int action, int mod) {}
	virtual void on_mouse_move(glm::vec2 mouse_pos) {}
	virtual void on_mouse_click(int button, int action, int mods) {}

	Entity character;
	GLFWwindow* window;
	std::chrono::time_point<std::chrono::system_clock> start_time;

};
