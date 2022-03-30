#include "hpp/States/character_grounded_state.hpp"

#include "GLFW/glfw3.h"
#include "hpp/tiny_ecs_registry.hpp"
#include "hpp/ui_system.hpp"
#include "hpp/Game_Mechanics/shooting_system.hpp"

#pragma region
//Super class 
CharacterGroundedState::CharacterGroundedState(Entity e) : CharacterState(e) {

}

CharacterGroundedState::~CharacterGroundedState() = default;


void CharacterGroundedState::enter() {
	CharacterState::enter();
}

void CharacterGroundedState::exit() {
	CharacterState::exit();
}

void CharacterGroundedState::step(float elapsed_ms) {
	CharacterState::step(elapsed_ms);
}

void CharacterGroundedState::doChecks() {
	CharacterState::doChecks();
}
#pragma endregion Super-CharacterGroundedState

#pragma region
//Idle Sub class
CharacterIdleState::CharacterIdleState(Entity e) : CharacterGroundedState(e) {

}

CharacterIdleState::~CharacterIdleState() = default;


void CharacterIdleState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_idle();
	UISystem::hide_crosshair(character);
}

void CharacterIdleState::exit() {
	CharacterGroundedState::exit();
}

void CharacterIdleState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
}

void CharacterIdleState::doChecks() {
	CharacterGroundedState::doChecks();
	Character* chara = registry.characters.get(character);
	if (chara->state_machine.isSelected()) {
		chara->state_machine.changeState(chara->aim_state);
	}
}

void CharacterIdleState::on_player_key(int key, int, int action, int mod) {


	if (action == GLFW_PRESS) {

		if (key == GLFW_KEY_W) {
			//TODO
		}
		if (key == GLFW_KEY_D) {

		}
		if (key == GLFW_KEY_A) {

		}

	}
}
void CharacterIdleState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterIdleState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion Sub-CharacterIdleState


//Move sub class
#pragma region
CharacterMoveState::CharacterMoveState(Entity e) : CharacterGroundedState(e) {

}

CharacterMoveState::~CharacterMoveState() = default;


void CharacterMoveState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_walk();
}

void CharacterMoveState::exit() {
	CharacterGroundedState::exit();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = 0.0f;
}

void CharacterMoveState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);

}

void CharacterMoveState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterMoveState::on_player_key(int key, int, int action, int mod) {

}
void CharacterMoveState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveState

#pragma region
CharacterMoveLeftState::CharacterMoveLeftState(Entity e) : CharacterMoveState(e) {

}

void CharacterMoveLeftState::enter() {
	CharacterMoveState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = -speed;
}

void CharacterMoveLeftState::step(float elapsed_ms) {
	CharacterMoveState::step(elapsed_ms);
	if (registry.motions.get(character).velocity.x == 0.0f) {
		Character* chara = registry.characters.get(character);
		chara->state_machine.changeState(chara->aim_state);
	}

}

void CharacterMoveLeftState::doChecks() {
	CharacterMoveState::doChecks();
}

void CharacterMoveLeftState::on_player_key(int key, int, int action, int mod) {
	CharacterMoveState::on_player_key(key, 0, action, mod);
	Character* c = registry.characters.get(character);

	if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_A) {
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = 0.0f;
		}
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			c->state_machine.changeState(c->airborne_move_right);
		}
	}
}
void CharacterMoveLeftState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveLeftState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveLeftState

#pragma region
CharacterMoveRightState::CharacterMoveRightState(Entity e) : CharacterMoveState(e) {

}

void CharacterMoveRightState::enter() {
	CharacterMoveState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = speed;

}


void CharacterMoveRightState::step(float elapsed_ms) {
	CharacterMoveState::step(elapsed_ms);
	if (registry.motions.get(character).velocity.x == 0.0f) {
		Character* chara = registry.characters.get(character);
		chara->state_machine.changeState(chara->aim_state);
	} 

}

void CharacterMoveRightState::doChecks() {
	CharacterMoveState::doChecks();
}

void CharacterMoveRightState::on_player_key(int key, int, int action, int mod) {
	CharacterMoveState::on_player_key(key, 0, action, mod);
	Character* c = registry.characters.get(character);

	if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_D) {
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = 0.0f;
		}
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			c->state_machine.changeState(c->airborne_move_right);
		}
	}
}
void CharacterMoveRightState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveRightState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveRightState

//Aiming
#pragma region
CharacterAimState::CharacterAimState(Entity e) : CharacterGroundedState(e) {

}


void CharacterAimState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_aim();
	UISystem::show_crosshair(character);
}

void CharacterAimState::exit() {
	CharacterGroundedState::exit();
	UISystem::hide_crosshair(character);
}

void CharacterAimState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);

}

void CharacterAimState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterAimState::on_player_key(int key, int, int action, int mod) {
	Character* chara = registry.characters.get(character);
	if (!chara->state_machine.isAI()) {
		if (action == GLFW_PRESS) {

			if (key == GLFW_KEY_UP) {
				ShootingSystem::aimUp(character, 0.05f);
			}
			if (key == GLFW_KEY_DOWN) {
				ShootingSystem::aimDown(character, 0.05f);
			}
			if (key == GLFW_KEY_W) {
				chara->state_machine.changeState(chara->airborne_state);
			}
			if (key == GLFW_KEY_D) {
				chara->state_machine.changeState(chara->move_right_state);
			}
			if (key == GLFW_KEY_A) {
				chara->state_machine.changeState(chara->move_left_state);
			}
		}
	}
}
void CharacterAimState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterAimState::on_mouse_click(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			ShootingSystem::shoot(character);
			Character* chara = registry.characters.get(character);
			chara->state_machine.changeState(chara->frozen_state);
		}
	}
}

void CharacterAimState::on_mouse_scroll(double xoffset, double yoffset) {
	if (yoffset > 0) {
		ShootingSystem::aimUp(character, (float)(0.05 * abs(yoffset)));
	}
	else {
		ShootingSystem::aimDown(character, (float)(0.05 * abs(yoffset)));
	}
}

#pragma endregion CharacterAimState

#pragma region CharacterFrozenState
CharacterWaitForBulletFrozenState::CharacterWaitForBulletFrozenState(Entity e) : CharacterGroundedState(e) {

}


void CharacterWaitForBulletFrozenState::enter() {
	CharacterGroundedState::enter();
}

void CharacterWaitForBulletFrozenState::exit() {
	CharacterGroundedState::exit();
}

void CharacterWaitForBulletFrozenState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
}

void CharacterWaitForBulletFrozenState::doChecks() {
	CharacterGroundedState::doChecks();
	if (registry.projectiles.components.empty()) {
		next_turn = true;
	}
}
#pragma endregion