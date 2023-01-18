#include "hpp/tiny_ecs.hpp"
#include "hpp/States/character_airborne_states.hpp"

#include "GLFW/glfw3.h"
#include "hpp/tiny_ecs_registry.hpp"
#include "hpp/world_system.hpp"

CharacterAirborneState::CharacterAirborneState(Entity e) : CharacterState(e) {
	
}
CharacterAirborneState::~CharacterAirborneState() = default;

void CharacterAirborneState::enter() {
	CharacterState::enter();
	Character* c = registry.characters.get(character);
	if (!c->state_machine.isJumping()) {
		Character* c = registry.characters.get(character);
		c->animate_jump();
		Rigidbody& rb = registry.rigidBodies.get(character);
		rb.collision_normal.y = 0.0f;
		Motion& motion = registry.motions.get(character);
		motion.velocity.y = -300.0f;
	}
	c->state_machine.setJumping(true);
}
void CharacterAirborneState::exit() {
	CharacterState::exit();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = 0.0f;
}
void CharacterAirborneState::step(float elapsed_ms) {
	CharacterState::step(elapsed_ms);
}
void CharacterAirborneState::doChecks() {
	CharacterState::doChecks();
	Character* c = registry.characters.get(character);
	if (c->state_machine.isJumping()) {
		if ((int)registry.rigidBodies.get(character).collision_normal.y == -1) {
			c->state_machine.setJumping(false);
			if (c->state_machine.isSelected()) {
				c->state_machine.changeState(c->aim_state);
			}
			else {
				c->state_machine.changeState(c->idle_state);
			}
		}
	}
}

void CharacterAirborneState::on_player_key(int key, int, int action, int mod) {
	Character* c = registry.characters.get(character);
	
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_SPACE) {
				WorldSystem::pause_flag = !WorldSystem::pause_flag;
			return;
		}
		if (key == GLFW_KEY_D) {
			c->state_machine.changeState(c->airborne_move_right);
		}
		if (key == GLFW_KEY_A) {
			c->state_machine.changeState(c->airborne_move_left);
		}
	}
}

CharacterAirborneMoveRightState::CharacterAirborneMoveRightState(Entity e) : CharacterAirborneState(e) {

}

void CharacterAirborneMoveRightState::enter() {
	CharacterAirborneState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = air_strafe_speed;
}
void CharacterAirborneMoveRightState::exit() {
	CharacterAirborneState::exit();
}
void CharacterAirborneMoveRightState::doChecks() {
	CharacterAirborneState::doChecks();
	Motion& motion = registry.motions.get(character);
	Character* c = registry.characters.get(character);
	if ((int)motion.velocity.x == 0) {
		c->state_machine.changeState(c->airborne_state);
	}
}

void CharacterAirborneMoveRightState::on_player_key(int key, int, int action, int mod) {
	Motion& motion = registry.motions.get(character);
	if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_SPACE) {
				WorldSystem::pause_flag = !WorldSystem::pause_flag;
			return;
			}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_D) {
			motion.velocity.x = 0.0f;
		}
	}
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) {
			Character* c = registry.characters.get(character);
			c->state_machine.changeState(c->airborne_move_left);
		}
	}

}

CharacterAirborneMoveLeftState::CharacterAirborneMoveLeftState(Entity e) : CharacterAirborneState(e) {

}

void CharacterAirborneMoveLeftState::enter() {
	CharacterAirborneState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = -air_strafe_speed;
}
void CharacterAirborneMoveLeftState::exit() {
	CharacterAirborneState::exit();
}
void CharacterAirborneMoveLeftState::doChecks() {
	CharacterAirborneState::doChecks();
	Motion& motion = registry.motions.get(character);
	Character* c = registry.characters.get(character);
	if ((int)motion.velocity.x == 0) {
		c->state_machine.changeState(c->airborne_state);
	}
}

void CharacterAirborneMoveLeftState::on_player_key(int key, int, int action, int mod) {
	Motion& motion = registry.motions.get(character);
	if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_SPACE) {
			WorldSystem::pause_flag = !WorldSystem::pause_flag;
			return;
			}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) {
			motion.velocity.x = 0.0f;
		}
	}
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_D) {
			Character* c = registry.characters.get(character);
			c->state_machine.changeState(c->airborne_move_right);
		}
	}

}
