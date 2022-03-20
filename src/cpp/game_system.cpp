#include "hpp/game_system.hpp"

Game::Game() {
	
}

Game::~Game() {
	
}

void Game::init() {
	
}

void Game::addCat(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health) {
	Character character;
	character.animal = ANIMAL::CAT;
	character.health = health;
	character.alignment = alignment;
	character.starting_pos = pos;

	characters.push_back(character);
}

void Game::addDog(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health) {
	Character character;
	character.animal = ANIMAL::DOG;
	character.health = health;
	character.alignment = alignment;
	character.starting_pos = pos;
}

void Game::setTimer(float timer) {
	this.timer = timer;
}

void Game::getTimer() {
	return timer;
}

std::vector<Character> Game::getCharacters() {
	return characters;
}