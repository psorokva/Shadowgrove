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
	//std::vector<int[PV_SIZE][PV_SIZE]> path_var_all;

	std::vector< std::vector< std::vector<int> > > path_var_all;
	std::vector< std::vector< std::vector<int> > > path_var_horiz;
	std::vector< std::vector< std::vector<int> > > path_var_vert;
	std::vector< std::vector< std::vector<int> > > path_var_notn;
	std::vector< std::vector< std::vector<int> > > path_var_notw;
	std::vector< std::vector< std::vector<int> > > path_var_nots;
	std::vector< std::vector< std::vector<int> > > path_var_note;
	std::vector< std::vector< std::vector<int> > > path_var_corne;
	std::vector< std::vector< std::vector<int> > > path_var_cornw;
	std::vector< std::vector< std::vector<int> > > path_var_corse;
	std::vector< std::vector< std::vector<int> > > path_var_corsw;
	std::vector< std::vector< std::vector<int> > > stitch_var_we;
	std::vector< std::vector< std::vector<int> > > stitch_var_ns;

	std::vector< std::vector< std::vector<int> > > path_var_only_n;
	std::vector< std::vector< std::vector<int> > > path_var_only_w;
	std::vector< std::vector< std::vector<int> > > path_var_only_s;
	std::vector< std::vector< std::vector<int> > > path_var_only_e;

public:
	int** bignoise;
	int** biome_filter;
	int rows;
	int cols;
	std::vector<Chunk> chunks;
	MapGen(int maprows, int mapcols);
	~MapGen();
	void clearMap();
	void initPaths();
	//bool createChunkGrid(Chunk& chunk);
	void getStitchingVector(std::vector<ChunkStitch> & stitching_jobs, int rowz, int colz, int paths);
	void makeNoise(int level);
	bool createWorldMap();
	Chunk& createMapChunk(vec2 position);
};


