#include "hpp/tiny_ecs.hpp"
#include "hpp/States/character_airborne_states.hpp"

#include "GLFW/glfw3.h"
#include "hpp/tiny_ecs_registry.hpp"

CharacterAirborneState::CharacterAirborneState(Entity e) : CharacterState(e) {
	
}
CharacterAirborneState::~CharacterAirborneState() = default;

void CharacterAirborneState::enter() {
	CharacterState::enter();
	Character* c = registry.characters.get(character);
	c->animate_jump();
	Rigidbody& rb = registry.rigidBodies.get(character);
	rb.collision_normal.y = 0.0f;
	Motion& motion = registry.motions.get(character);
	motion.velocity.y = -300.0f;
	isJumping = true;
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
	if (isJumping) {
		if ((int)registry.rigidBodies.get(character).collision_normal.y == -1) {
			isJumping = false;
			Character* c = registry.characters.get(character);
			if (c->state_machine.isSelected()) {
				c->state_machine.changeState(c->aim_state);
			}
			else {
				c->state_machine.changeState(c->idle_state);
			}
		}
	}
	if ((int)registry.motions.get(character).velocity.x == 0) {
		Motion& motion = registry.motions.get(character);
		if (A_key_state == GLFW_PRESS && D_key_state == GLFW_PRESS) {
			motion.velocity.x = 0;
		} else if (A_key_state == GLFW_PRESS) {
			motion.velocity.x = -air_strafe_speed;
		} else if (D_key_state == GLFW_PRESS) {
			motion.velocity.x = air_strafe_speed;
		}
	}
}

void CharacterAirborneState::on_player_key(int key, int, int action, int mod) {
	Motion& motion = registry.motions.get(character);
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_D || key == GLFW_KEY_A) {
			motion.velocity.x = 0.0f;
		}
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_D) {
			motion.velocity.x = air_strafe_speed;
		}
		if (key == GLFW_KEY_A) {
			motion.velocity.x = -air_strafe_speed;
		}
	}

}
