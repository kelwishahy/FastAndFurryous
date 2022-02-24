#pragma once
#include <vector>

class Map {
public:
	Map();

	~Map();

	std::vector<std::vector<unsigned int>> getTileMap();

	void updateTileMap();

	unsigned int getMapWidth() { return this->mapWidth; }
	unsigned int getMapHeight() { return this->mapHeight; }

private:
	/*
	 * The game map is a 2d array of unsigned ints,
	 * with each int referencing a specific tile type
	 * (e.g. if gameMap[i][j] = 1, we render a stone tile)
	 */
	std::vector<std::vector<unsigned int>> tileMap;

	// Dimensions of this game map
	unsigned int mapHeight;
	unsigned int mapWidth;
};
