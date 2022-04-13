#pragma once
#include <vector>
#include "components.hpp"

class Game {

public:

	struct Character {
		ANIMAL animal = ANIMAL::CAT;
		WEAPON_TYPES weapon = WEAPON_TYPES::TOTAL;
		TEAM alignment = TEAM::PLAYER_1_TEAM;
		glm::vec2 starting_pos = glm::vec2(0.0f,0.0f);
		int health = -1;
	};

	Game();

	~Game();

	void init();

	//broken up for clarity
	void addCat(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);
	void addDog(WEAPON_TYPES weapon, TEAM alignment, glm::vec2 pos, int health);

	void setTimer(float timeInMs);
	float getTimer();

	void setBackGround(MAPS map);
	MAPS getBackGround();

	std::vector<Character> getCharacters();

	bool tutorial = false;

	
private:

	std::vector<Character> characters;
	float timer;
	MAPS background = MAPS::INDUSTRIAL;

};
