#pragma once
#include <vector>
#include "components.hpp"

class Game {

public:

	struct Character {
		ANIMAL animal = ANIMAL::CAT;
		WEAPON_TYPES weapon = WEAPON_TYPES::TOTAL;
		TEAM alignment = TEAM::PLAYER_1_TEAM;
		glm::vec2 starting_pos = vec2(0.0f,0.0f);
		int health = -1;
	};

	Game() = default;

	~Game() = default;

	void init();

	//broken up for clarity
	void addCat(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);
	void addDog(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);

	void setTimer(float timer);
	float getTimer();

	std::vector<Character> getCharacters();

	
private:

	std::vector<Character> characters;
	float timer = 1.0f;

};
