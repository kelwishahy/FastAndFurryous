#include "hpp/States/character_state.hpp"

CharacterState::CharacterState(Entity e, GLFWwindow* window) : character(e), window(window) {

}

CharacterState::~CharacterState() = default;


void CharacterState::enter() {
	doChecks();
	start_time = std::chrono::system_clock::now();
}

void CharacterState::exit() {

}

void CharacterState::step(float elapsed_ms) {
	doChecks();
}

void CharacterState::doChecks() {

}