#include <iostream>
#include <hpp/map.hpp>
#include <hpp/world_init.hpp>

#include "hpp/tiny_ecs_registry.hpp"

Map::Map() {

}

Map::~Map() {
	
}

void Map::init() {
	// Create a default game map
	this->mapHeight = 20;
	this->mapWidth = 30;
	this->tileScale = 64.f;

	/*
	 * First atempt at efficient collision handling: add collision components
	 * only if there are no tiles above this tile
	 */

	 // Map of all stone blocks
	 // Generate the map from the bottom up
	for (int y = 0; y < mapHeight; y++) {
		tileMap.push_back({});
		for (int x = 0; x < mapWidth; x++) {
			if (y >= 15) {
				tileMap[y].push_back(1);
			} else if (y == 11 & (14 <= x && x < 20)) {
				tileMap[y].push_back(1);
			}
			else {
				tileMap[y].push_back(0);
			}
		}
	}
	int tileCount = 0;
	int tilesInRow = 0;
	// std::cout << "{";
	for (int y = 0; y < mapHeight; y++) {
		// std::cout << "\n";
		for (int x = 0; x < mapWidth; x++) {
			// std::cout << tileMap[y][x];
			// std::cout << ", ";
			if (tileMap[y][x] != 0 && tileMap[y - 1][x] == 0) {
				if(x < mapWidth - 1 && tileMap[y][x+1] != 0) {
					tilesInRow++;
				} else {
					printf("\nCreating %d tiles in a row\n", tilesInRow);
					printf("Position being passed is (%d, %d)\n", y, x - tilesInRow);
					createTile(tileScale, { y, x - tilesInRow}, tilesInRow);
				}
			}
		}
		tilesInRow = 0;
	}
	// std::cout << "}\n" << std::endl;

	// printf("Number of tiles created  = %d\n", tileCount);
}


std::vector<std::vector<unsigned int>> Map::getTileMap() {
	return tileMap;
}

void Map::updateTileMap() {
	(void)tileMap;
}
