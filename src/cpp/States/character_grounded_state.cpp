#include "hpp/States/character_grounded_state.hpp"

#include "hpp/tiny_ecs_registry.hpp"

#pragma region
//Super class 
CharacterGroundedState::CharacterGroundedState(Entity e, GLFWwindow* win) : CharacterState(e, win) {

}

CharacterGroundedState::~CharacterGroundedState() = default;


void CharacterGroundedState::enter() {
	CharacterState::enter();
}

void CharacterGroundedState::exit() {
	CharacterState::enter();
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
CharacterIdleState::CharacterIdleState(Entity e, GLFWwindow* w) : CharacterGroundedState(e, w) {

}

CharacterIdleState::~CharacterIdleState() = default;


void CharacterIdleState::enter() {
	CharacterGroundedState::enter();
	Character chara = registry.characters.get(character);
	chara.animate_idle();
}

void CharacterIdleState::exit() {
	CharacterGroundedState::enter();
}

void CharacterIdleState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
}

void CharacterIdleState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterIdleState::on_player_key(int key, int, int action, int mod) {


	if (action == GLFW_PRESS) {

		if (key == GLFW_KEY_W) {
			//TODO
		}
		if (key == GLFW_KEY_D) {
			printf("Hello1");
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = 90.0f;
			Character& chara = registry.characters.get(character);
			chara.state_machine.changeState(chara.move_state);
		}
		if (key == GLFW_KEY_A) {
			printf("Hello1");
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = -90.0f;
			Character& chara = registry.characters.get(character);
			chara.state_machine.changeState(chara.move_state);
		}

	}
}
void CharacterIdleState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterIdleState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion Sub-CharacterIdleState


//Move sub class
CharacterMoveState::CharacterMoveState(Entity e, GLFWwindow* w) : CharacterGroundedState(e, w) {

}

CharacterMoveState::~CharacterMoveState() = default;


void CharacterMoveState::enter() {
	CharacterGroundedState::enter();
	Character chara = registry.characters.get(character);
	chara.animate_walk();
}

void CharacterMoveState::exit() {
	CharacterGroundedState::enter();
}

void CharacterMoveState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);

}

void CharacterMoveState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterMoveState::on_player_key(int key, int, int action, int mod) {


	if (action == GLFW_PRESS) {

		if (key == GLFW_KEY_W) {
			printf("hello world 2");
		}
		if (key == GLFW_KEY_D) {
			printf("hello world 2");
		}
		if (key == GLFW_KEY_A) {
			printf("hello world 2");
		}
	}
}
void CharacterMoveState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveState::on_mouse_click(int button, int action, int mods) {

}