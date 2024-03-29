#include <iostream>
#include <hpp/map.hpp>
#include <hpp/world_init.hpp>
#include <fstream>
#include <sstream>
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
	this->tileScale = ceil(scaleToScreenResolution({ (defaultResolution.x / 30.f), (defaultResolution.x / 30.f) }).x);
	readMapFromFile();
	printf("Tiles are %fx%f for map %d\n", tileScale, tileScale, (int)this->name);

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

	printf("Number of tiles in this map: %d\n", numTiles);

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

		case MAPS::CYBERPUNK: {
			for (int i = 0; i < CyberpunkBackgroundLayers; i++) {
				auto ent = Entity();
				auto& bg = registry.backgrounds.emplace(ent);
				bg.layer = LAYERS[i];
				TEXTURE_IDS texture;

				switch (i) {
				case 0:
					texture = TEXTURE_IDS::CYBERPUNK1;
					break;
				case 1:
					texture = TEXTURE_IDS::CYBERPUNK2;
					break;
				default:
					texture = TEXTURE_IDS::CYBERPUNK3;
				}

				registry.renderRequests.insert(ent, { texture, SHADER_PROGRAM_IDS::TEXTURE, GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
			}
			break;
		}

		case MAPS::MIAMI: {
			for (int i = 0; i < MiamiBackgroundLayers; i++) {
				auto ent = Entity();
				auto& bg = registry.backgrounds.emplace(ent);
				bg.layer = LAYERS[i];
				TEXTURE_IDS texture;

				switch (i) {
				case 0:
					texture = TEXTURE_IDS::MIAMI1;
					break;
				case 1:
					texture = TEXTURE_IDS::MIAMI2;
					break;
				case 2:
					texture = TEXTURE_IDS::MIAMI3;
					break;
				default:
					texture = TEXTURE_IDS::MIAMI4;
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
	case MAPS::TUTORIAL: {

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
		case MAPS::MIAMI: {
			fileName = "map2.txt";
			break;
		}
		case MAPS::CYBERPUNK: {
			fileName = "map3.txt";
			break;
		}
	case MAPS::TUTORIAL: {
			fileName = "tutorial.txt";
			break;
		}
		default: {
			fileName = "map1.txt";
		}
	}

	fstream f;
	int i = 0, j = 0, n = 0;
	string line;
	f.open(std::string(PROJECT_SOURCE_DIR) + "assets/maps/" + fileName, ios::in);

	for (;;) {
		std::getline(f, line);
		if (!f) {  // test eof after read
			break; 
		}
		tileMap.push_back({});
		std::istringstream fline(line);
		j = 0;

		for (;;) {
			int val;
			fline >> val;
			if (!fline) break;
			tileMap[i].push_back(val);

			if (tileMap[i][j] != 0) {
				numTiles++;
	
				// Compute tile positions since they won't change
				/* MATRIX TRANSFORMATIONS */
				mat4 transformationMatrix = transform(
					{ (0.5 + j) * tileScale, (0.5 + i) * tileScale },
					{ tileScale, tileScale },
					0.f, 0);
				
				transformations.push_back(transformationMatrix);
			}


			j++;
		}

		i++;
		if (n == 0) n = j;
		else if (n != j) {
			cerr << "Error line " << i << " - " << j << " values instead of " << n << endl;
		}
	}

	if (i != n) {
		cerr << "Error " << i << " lines instead of " << n << endl;
	}

	this->mapWidth = j;
	this->mapHeight = i;

	printf("Map is %dx%d\n", i, j);

	f.close();

	// Pass in tile transformation matrices
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * numTiles, transformations.data(), GL_STATIC_DRAW);
	glHasError();

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 0));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
}
