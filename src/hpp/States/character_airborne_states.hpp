#pragma once
#include "hpp/States/character_state.hpp"

class CharacterAirborneState :public CharacterState {

public:
	CharacterAirborneState(Entity e);
	~CharacterAirborneState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override {}
	void on_mouse_click(int button, int action, int mods) override {}

	bool isJumping = false;
	float air_strafe_speed = 80.0f;

};