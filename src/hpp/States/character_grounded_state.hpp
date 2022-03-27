#pragma once
#include "hpp/character_state.hpp"

class CharacterGroundedState :public CharacterState {

public:
	CharacterGroundedState(Entity e, GLFWwindow* win);
	~CharacterGroundedState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void set_user_callbacks() override {}
	void on_player_key(int key, int, int action, int mod) override {}
	void on_mouse_move(glm::vec2 mouse_pos) override {}
	void on_mouse_click(int button, int action, int mods) override {}

};

class CharacterIdleState : public CharacterGroundedState {

	CharacterIdleState(Entity e, GLFWwindow* w);
	~CharacterIdleState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void set_user_callbacks() override;
	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

};

class CharacterMoveState : public CharacterGroundedState {

	CharacterMoveState(Entity e, GLFWwindow* w);
	~CharacterMoveState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void set_user_callbacks() override;
	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

};