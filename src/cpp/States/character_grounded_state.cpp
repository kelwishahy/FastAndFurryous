#include "hpp/States/character_grounded_state.hpp"

#include "hpp/tiny_ecs_registry.hpp"


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

//Idle Sub class
CharacterIdleState::CharacterIdleState(Entity e, GLFWwindow* w) : CharacterGroundedState(e, w) {

}

CharacterIdleState::~CharacterIdleState() = default;


void CharacterIdleState::enter() {
	CharacterGroundedState::enter();
	if (check_if_cat(character)) {
		Cat cat = registry.cats.get(character);
		cat.animate_cat_idle();
	} else {
		Dog dog = registry.dogs.get(character);
		dog.animate_dog_idle();
	}
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

void CharacterIdleState::set_user_callbacks() {
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { static_cast<CharacterIdleState*>(glfwGetWindowUserPointer(wnd))->on_player_key(_0, _1, _2, _3); };
	glfwSetKeyCallback(this->window, key_redirect);
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { static_cast<CharacterIdleState*>(glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(this->window, cursor_pos_redirect);
	auto mouse_input = [](GLFWwindow* wnd, int _0, int _1, int _2) { static_cast<CharacterIdleState*>(glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(this->window, mouse_input);
}
void CharacterIdleState::on_player_key(int key, int, int action, int mod) {


	if (action == GLFW_PRESS) {
		if (registry.cats.has(character)) {
			Cat& cat = registry.cats.get(character);

			if (key == GLFW_KEY_W) {
				//TODO
			}
			if (key == GLFW_KEY_D) {

			}
			if (key == GLFW_KEY_A) {

			}
		}
	}
}
void CharacterIdleState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterIdleState::on_mouse_click(int button, int action, int mods) {

}


//Move sub class