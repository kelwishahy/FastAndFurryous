#pragma once
#include <vector>
#include <array>
#include "components.hpp"

class MapSystem {
public:
	class Map {
	public:
		Map() {};
		Map(MAPS name);
		unsigned int getMapWidth() { return this->mapWidth; }
		unsigned int getMapHeight() { return this->mapHeight; }
		float getTileScale() { return this->tileScale; }
		std::vector<std::vector<unsigned int>> getTileMap();
		void build();
	private:
		MAPS name;
		std::vector<std::vector<unsigned int>> tileMap;
		unsigned int mapHeight;
		unsigned int mapWidth;
		float tileScale;
		void readMapFromFile();
	};
	MapSystem(){};

	~MapSystem(){};

	void init();

	Map& getMap(MAPS mapName) { return maps[(int)mapName]; }
private:
	std::array<Map, mapCount> maps;
};
