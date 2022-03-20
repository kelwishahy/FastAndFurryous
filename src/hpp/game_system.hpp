#pragma once
#include <vector>
#include <array>
#include "components.hpp"

class Game {

public:

	struct Character {
		ANIMAL animal;
		WEAPON_TYPES weapon;
		TEAM alignment;
		glm::vec2 starting_pos;
		int health;
	};

	Game() {};

	~Game() {};

	void init();

	//broken up for clarity
	void addCat(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);
	void addDog(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);

	void setTimer(float timer);
	float getTimer();

	std::vector<Character> getCharacters();

	
private:

	std::vector<Character> characters;
	float timer;

};
