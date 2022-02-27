#include <iostream>
#include <hpp/map.hpp>
#include <hpp/world_init.hpp>
#include <fstream>
#include <../ext/project_path.hpp>
#include "hpp/tiny_ecs_registry.hpp"
using namespace std;

Map::Map() {

}

Map::~Map() {
	
}

void Map::init() {
	// Create a default game map
	this->mapHeight = 18;
	this->mapWidth = 30;
	this->tileScale = 64.f;

	readMapFromFile();

	// Iterate through rows
	for (int y = mapHeight - 1; y >= 0 ; y--) {
		int consecutiveTileCount = 1;

		// Iterate through columns
		for (int x = 0; x < mapWidth; x++) {
			if (tileMap[y][x] != 0) { // Is the current tile occupied?
				// YES - this tile is occupied
				if (y > 0 && tileMap[y-1][x] != 0) { // Is the tile above this one occupied?
					// YES - the tile above is occupied
					continue;
				} else {
					// NO - the tile above is not occupied
					while (x < mapWidth - 1 && tileMap[y][x + 1] != 0) { // Is the tile to the right occupied?
						// YES - the tile to the right is occupied
						consecutiveTileCount++;
						x++;
					}
					// No - the tile to the right is not occupied

					// cout << "\nThere are " << consecutiveTileCount << " tiles in a row in row " << y + 1 << endl;
					createTile(tileScale, { y, x - consecutiveTileCount }, consecutiveTileCount);
					consecutiveTileCount = 1;
				}
				
			}
			// NO - this tile is not occupied
		}
	}


	// Set the background image
	auto ent = Entity();
	registry.backgrounds.emplace(ent);
	// auto& motion = registry.motions.emplace(ent);
	// registry.rigidBodies.emplace(ent);
	// registry.boxColliders.emplace(ent);
	// motion.position = { 100.f , 100.f };
	// motion.scale = { 100.f, 100.f };
	// motion.angle = 0.f;
	// motion.velocity = { 0.f, 0.f };
	registry.renderRequests.insert(
		ent,
		{ TEXTURE_IDS::BACKGROUND,
			SHADER_PROGRAM_IDS::TEXTURE,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
}

void Map::readMapFromFile() {
	const string mapName = "map1.txt";
	ifstream infile;
	infile.open(std::string(PROJECT_SOURCE_DIR) + "assets/maps/" + mapName);

	if (!infile) {
		std::cerr << "Unable to open file\n" << std::endl;
		assert(false);
	}

	for (int y = 0; y < mapHeight; y++) {
		tileMap.push_back({});
		for (int x = 0; x < mapWidth; x++) {
			tileMap[y].push_back({});
			if (!(infile >> tileMap[y][x])) {
				cerr << "Unexpected end of file\n" << endl;
				exit(1);
			}
		}
	}

	infile.close();

	// cout << "\n";
	// for (int y = 0; y < mapHeight; y++) {
	// 	for (int x = 0; x < mapWidth; x++) {
	// 		cout << tileMap[y][x] << " ";
	// 	}
	// 	cout << endl;
	// }
}



std::vector<std::vector<unsigned int>> Map::getTileMap() {
	return tileMap;
}

void Map::updateTileMap() {
	(void)tileMap;
}
