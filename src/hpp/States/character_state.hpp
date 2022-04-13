#pragma once

#include <chrono>

#include "hpp/tiny_ecs.hpp"
#include "hpp/common.hpp"

class CharacterState {

public:

	CharacterState(Entity e);
	virtual ~CharacterState();

	virtual void enter();
	virtual void exit();
	virtual void step(float elapsed_ms);
	virtual void doChecks();

	virtual void on_player_key(int key, int, int action, int mod) {}
	virtual void on_mouse_move(glm::vec2 mouse_pos) {}
	virtual void on_mouse_click(int button, int action, int mods) {}
	virtual void on_mouse_scroll(double xoffset, double yoffset) {}

	bool should_increment_turn() const {
		return next_turn;
	}

	void set_A_key_state(int state) {
		A_key_state = state;
	}
	void set_D_key_state(int state) {
		D_key_state = state;
	}


	Entity character;
	std::chrono::time_point<std::chrono::system_clock> start_time;
	bool next_turn;
	int A_key_state = -1;
	int D_key_state = -1;
	std::string stringtoformat;
};
