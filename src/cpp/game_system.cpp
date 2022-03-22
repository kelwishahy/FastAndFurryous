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
	character.weapon = weapon;

	characters.push_back(character);
}

void Game::addDog(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health) {
	Character character;
	character.animal = ANIMAL::DOG;
	character.health = health;
	character.alignment = alignment;
	character.starting_pos = pos;
	character.weapon = weapon;

	characters.push_back(character);
}

void Game::setTimer(float timeInMs) {
	timer = timeInMs;
}

void Game::setBackGround(MAPS map) {
	background = map;
}

MAPS Game::getBackGround() {
	return background;
}


float Game::getTimer() {
	return timer;
}

std::vector<Game::Character> Game::getCharacters() {
	return characters;
}