#include <hpp/map.hpp>

Map::Map() {
	// Create a default game map
	this->mapHeight = 20;
	this->mapWidth = 30;

	// Map of all stone blocks
	for (int x = 0; x < mapHeight; x++) {
		tileMap.push_back({});
		for (int y = 0; y < mapWidth; y++) {
			tileMap[x].push_back(1);
		}
	}
}

Map::~Map() {
	
}

std::vector<std::vector<unsigned int>> Map::getTileMap() {
	return tileMap;
}

void Map::updateTileMap() {
	(void)tileMap;
}
