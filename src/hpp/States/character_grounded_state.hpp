#pragma once
#include "hpp/States/character_state.hpp"

class CharacterGroundedState :public CharacterState {

public:
	CharacterGroundedState(Entity e);
	~CharacterGroundedState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override {}
	void on_mouse_move(glm::vec2 mouse_pos) override {}
	void on_mouse_click(int button, int action, int mods) override {}

};

class CharacterIdleState : public CharacterGroundedState {

public:
	CharacterIdleState(Entity e);
	~CharacterIdleState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

};

class CharacterMoveState : public CharacterGroundedState {

public:
	CharacterMoveState(Entity e);
	~CharacterMoveState() override;

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

	float speed = 90.0f;

};

class CharacterMoveLeftState : public CharacterMoveState {

public:
	CharacterMoveLeftState(Entity e);

	void enter() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

};

class CharacterMoveRightState : public CharacterMoveState {

public:
	CharacterMoveRightState(Entity e);

	void enter() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;

};

class CharacterAimState : public CharacterGroundedState {

public:
	CharacterAimState(Entity e);

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

	void on_player_key(int key, int, int action, int mod) override;
	void on_mouse_move(glm::vec2 mouse_pos) override;
	void on_mouse_click(int button, int action, int mods) override;
	void on_mouse_scroll(double xoffset, double yoffset) override;

	

};

class CharacterWaitForBulletFrozenState : public CharacterGroundedState {

public:
	CharacterWaitForBulletFrozenState(Entity e);

	void enter() override;
	void exit() override;
	void step(float elapsed_ms) override;
	void doChecks() override;

};