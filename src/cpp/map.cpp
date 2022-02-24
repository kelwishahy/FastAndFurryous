#include <hpp/map.hpp>

Map::Map() {
	// Create a default game map
	this->mapHeight = 20;
	this->mapWidth = 30;
	this->tileScale = 64.f;

	// Map of all stone blocks
	for (int x = 0; x < mapHeight; x++) {
		tileMap.push_back({});
		for (int y = 0; y < mapWidth; y++) {
			tileMap[x].push_back(1);
		}
	}

	// TODO:
	// Create a tile entity for each tile where tileMap[x][y] != 0.
	// This is inefficient but should be ok for now to make collsion handling easier.
}

Map::~Map() {
	
}

std::vector<std::vector<unsigned int>> Map::getTileMap() {
	return tileMap;
}

void Map::updateTileMap() {
	(void)tileMap;
}
