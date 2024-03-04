#pragma once
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include "components.hpp"

#include <array>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <iostream>

class MapGen {
private:
	//std::vector<Chunk> chunks;
	//static const int MapGen::grid_pillars[MAZE_SIZE][MAZE_SIZE];
	//static const int MapGen::grid_alt_base1[MAZE_SIZE][MAZE_SIZE];
	//static const int MapGen::grid_alt_base2[MAZE_SIZE][MAZE_SIZE];

public:
	int** bignoise;
	int** biome_filter;
	int rows;
	int cols;
	std::vector<Chunk> chunks;
	MapGen(int maprows, int mapcols);
	~MapGen();
	void clearMap();
	//bool createChunkGrid(Chunk& chunk);
	bool createWorldMap();
	Chunk& createMapChunk(vec2 position);
};


