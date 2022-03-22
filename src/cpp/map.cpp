#include <iostream>
#include <hpp/map.hpp>
#include <hpp/world_init.hpp>
#include <fstream>
#include <../ext/project_path.hpp>
#include "hpp/common.hpp"
#include "hpp/tiny_ecs_registry.hpp"

using namespace std;

void MapSystem::init() {
	for (int i = 0; i < (int)MAPS::TOTAL; i++) {
		maps[i] = Map(MAPS(i));
	}
}

std::vector<std::vector<unsigned>> MapSystem::Map::getTileMap() {
	return tileMap;
}

MapSystem::Map::Map(MAPS name) {
	this->name = name;
}

void MapSystem::Map::build() {
	this->mapHeight = 18;
	this->mapWidth = 60;
	// this->tileScale = ceil((defaultResolution.x / 30.f) / defaultResolution.x * screenResolution.x);
	this->tileScale = ceil(scaleToScreenResolution({ (defaultResolution.x / 30.f), (defaultResolution.x / 30.f) }).x);
	printf("Tiles are %fx%f for map %d\n", tileScale, tileScale, (int)this->name);

	readMapFromFile();

	// Iterate through rows
	for (int y = mapHeight - 1; y >= 0; y--) {
		int consecutiveTileCount = 1;

		// Iterate through columns
		for (int x = 0; x < mapWidth; x++) {
			if (tileMap[y][x] != 0) { // Is the current tile occupied?
				// YES - this tile is occupied
				if (y > 0 && tileMap[y - 1][x] != 0) { // Is the tile above this one occupied?
					// YES - the tile above is occupied
					continue;
				}
				else {
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

	switch (name) {
		case MAPS::INDUSTRIAL: {
			for (int i = 0; i < IndustrialBackgroundLayers; i++) {
				auto ent = Entity();
				auto& bg = registry.backgrounds.emplace(ent);
				bg.layer = LAYERS[i];
				TEXTURE_IDS texture;

				switch (i) {
					case 0:
						texture = TEXTURE_IDS::INDUSTRIAL_BG;
						break;
					case 1:
						texture = TEXTURE_IDS::INDUSTRIAL_FAR_BUILDINGS;
						break;
					case 2:
						texture = TEXTURE_IDS::INDUSTRIAL_BUILDINGS;
						break;
					default:
						texture = TEXTURE_IDS::INDUSTRIAL_FOREGROUND;
				}

				registry.renderRequests.insert(ent, { texture, SHADER_PROGRAM_IDS::TEXTURE, GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
			}
			break;
		}

		case MAPS::NIGHT: {
			for (int i = 0; i < NightBackgroundLayers; i++) {
				auto ent = Entity();
				auto& bg = registry.backgrounds.emplace(ent);
				bg.layer = LAYERS[i];
				TEXTURE_IDS texture;

				if (i == 0) {
					texture = TEXTURE_IDS::NIGHT1;
				} else {
					texture = TEXTURE_IDS::NIGHT2;
				}

				registry.renderRequests.insert(ent, { texture, SHADER_PROGRAM_IDS::TEXTURE, GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
			}
			break;
		}

		case MAPS::FOREST: {
			auto ent = Entity();
			registry.backgrounds.emplace(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_IDS::FOREST,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
			break;
		}
		case MAPS::SPACE: {
			auto ent = Entity();
			registry.backgrounds.emplace(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_IDS::SPACE,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
			break;
		}
		default: {
		}
	}
}


void MapSystem::Map::readMapFromFile() {
	string fileName;
	switch (name) {
		case MAPS::INDUSTRIAL: {
			fileName = "map1.txt";
			break;
		}
		case MAPS::FOREST: {
			fileName = "map2.txt";
			break;
		}
		case MAPS::SPACE: {
			fileName = "map3.txt";
			break;
		}
		default: {
			fileName = "map1.txt";
		}
	}

	ifstream infile;
	infile.open(std::string(PROJECT_SOURCE_DIR) + "assets/maps/" + fileName);

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
}
