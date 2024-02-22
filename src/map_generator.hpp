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
	std::vector<Chunk> chunks;
	MapGen();
	~MapGen();
	//bool createChunkGrid(Chunk& chunk);
	bool createWorldMap(int rows, int cols);
	Chunk& createMapChunk(vec2 position);
};


