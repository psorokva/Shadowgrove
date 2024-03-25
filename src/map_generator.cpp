#include "map_generator.hpp"
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

void MapGen::clearMap() {
	chunks.clear();
}

void MapGen::makeNoise(int level) {
	const int BIOMES = 4;
	const int RARITY = 14;
	const int NOISE_LEVELS = 10;
	const int BIG_WIDTH = cols * MAZE_SIZE;
	const int BIG_HEIGHT = rows * MAZE_SIZE;
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		bignoise[i] = new int[BIG_WIDTH];
		for (int j = 0; j < BIG_WIDTH; ++j) {
			bignoise[i][j] = rand() % NOISE_LEVELS; //random number between 0 and 4 incl
			//printf(" %x", bignoise[i][j]);
		}
		//printf("\n");
	}
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		biome_filter[i] = new int[BIG_WIDTH];
		for (int j = 0; j < BIG_WIDTH; ++j) {
			biome_filter[i][j] = level;
		}
	}
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		for (int j = 0; j < BIG_WIDTH; ++j) {
			if (rand() % RARITY == 1) {
				//int shot_grid[5][5];
				int shot_type = rand() % 8;
				int SMALLMAX = 5;
				if (shot_type > 3) {
					SMALLMAX = 7;
				}
				for (int r = 0; r < SMALLMAX; r++) {
					for (int c = 0; c < SMALLMAX; c++) {
						if ((i + r) < BIG_HEIGHT && (j + c < BIG_WIDTH)) {

							switch (shot_type) {
							case 0:
								biome_filter[i + r][j + c] += biome_shotgun_00[r][c];
								break;
							case 1:
								biome_filter[i + r][j + c] += biome_shotgun_01[r][c];
								break;
							case 2:
								biome_filter[i + r][j + c] += biome_shotgun_02[r][c];
								break;
							case 3:
								biome_filter[i + r][j + c] += biome_shotgun_03[r][c];
								break;
							case 4:
								biome_filter[i + r][j + c] += biome_shotgun_10[r][c];
								break;
							case 5:
								biome_filter[i + r][j + c] += biome_shotgun_11[r][c];
								break;
							case 6:
								biome_filter[i + r][j + c] += biome_shotgun_12[r][c];
								break;
							case 7:
								biome_filter[i + r][j + c] += biome_shotgun_13[r][c];
								break;
							}
							/// POSSIBLE limit value here within range of sprite sheets
						}
					}
				}
			}

			biome_filter[i][j] = (  biome_filter[i][j] % (BIOMES + level) )  % 10;
			
		}
	}
}


MapGen::MapGen(int maprows, int mapcols) {
	initPaths();
	// init noise grid withnoise levels of noise
	rows = maprows;
	cols = mapcols;
	const int RARITY = 14;
	const int NOISE_LEVELS = 10;
	const int BIG_WIDTH = cols * MAZE_SIZE;
	const int BIG_HEIGHT = rows * MAZE_SIZE;
	chunks = std::vector<Chunk>();
	bignoise = new int* [BIG_HEIGHT];
	// init biome filter grid with 1s
	biome_filter = new int* [BIG_HEIGHT];
	makeNoise(1);
	//for (int i = 0; i < BIG_HEIGHT; ++i) {
	//	for (int j = 0; j < BIG_WIDTH; ++j) {
	//		printf(" %x", biome_filter[i][j]);
	//	}
	//	printf("\n");
	//}
}

MapGen::~MapGen() {
	//delete chunks;
}

// feed into the connectio row, cols, d, h into create?
bool MapGen::createWorldMap() {
	//printf("CREATE WORLD MAP\n");
	int sparn_chunk_col = (window_width_px / 2) / (MAZE_SIZE * TILE_SIZE);
	int spawn_chunk_row = (window_height_px - 200) / (MAZE_SIZE * TILE_SIZE);
	int start_row = ((window_height_px - 200) / TILE_SIZE) % MAZE_SIZE - 2;
	int fin_row = ((window_height_px - 200) / TILE_SIZE) % MAZE_SIZE + 3;
	int start_col = (window_width_px / 2 / TILE_SIZE) % MAZE_SIZE - 2;
	int fin_col = (window_width_px / 2 / TILE_SIZE) % MAZE_SIZE + 3;
	//printf("Adding Chunks\n");
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			Chunk& chunk = createMapChunk(vec2{ r, c });
			int alternate_map = rand() % 5;
			if (alternate_map < 1) {
				int alternate_type = rand() % 10;
				if (alternate_map < 5) {
					if (alternate_map % 2 == 0) {
						for (int r = 0; r < MAZE_SIZE; r++) {
							for (int c = 0; c < MAZE_SIZE; c++) {
								if (grid_alt_base1[r][c] < 2) {
									chunk.grid[r][c] = grid_alt_base1[r][c];
								}
							}
						}
					} else { 
						for (int r = 0; r < MAZE_SIZE; r++) {
							for (int c = 0; c < MAZE_SIZE; c++) {
								if (grid_alt_base2[r][c] < 2) {
									chunk.grid[r][c] = grid_alt_base2[r][c];
								}
							}
						}
					}
				} else if (alternate_map % 2 == 0) {
					for (int r = 0; r < MAZE_SIZE; r++) {
						for (int c = 0; c < MAZE_SIZE; c++) {
							if (grid_pillars[r][c] < 2) {
								chunk.grid[r][c] = grid_pillars[r][c];
							}
						}
					}
				} else {
					for (int r = 0; r < MAZE_SIZE; r++) {
						for (int c = 0; c < MAZE_SIZE; c++) {
							if (grid_center_clear1[r][c] < 2) {
								chunk.grid[r][c] = grid_center_clear1[r][c];
							}
						}
					}
				}
			}

			chunks.push_back(chunk);
			//printf("Added Chunk\n");



		}
	}
	//printf("stitching_jobs begin\n");
	std::vector<ChunkStitch> stitching_jobs = std::vector<ChunkStitch>();
	getStitchingVector(stitching_jobs, rows, cols, 2);
	//printf("stitching_jobs complete\n");
	//printf("stitching_jobs size %d \n", stitching_jobs.size());
	for (ChunkStitch& job : stitching_jobs) {
		//printf("JOB\n");
		//printf("job ch1(%i,%i)  ch2(%i,%i)  at %i\n",job.ch1x, job.ch1y, job.ch2x, job.ch2y , job.location);
		Chunk* chunk1 = new Chunk(); //= chunks[job.chunk1.x * cols + job.chunk1.y];
		Chunk* chunk2 = new Chunk(); // = chunks[job.chunk2.x * cols + job.chunk2.y];
		chunk1->x = -1;
		chunk2->x = -1;
		for (Chunk& chunk: chunks) {
			if (chunk.x == job.ch1x && chunk.y == job.ch1y) {
				chunk1 = &chunk;
			}
			if (chunk.x == job.ch2x && chunk.y == job.ch2y) {
				chunk2 = &chunk;
			}
		}
		//printf("job ch1(%i,%i)  ch2(%i,%i)  at %i\n", job.ch1x, job.ch1y, job.ch2x, job.ch2y, job.location);
		if (chunk1->x != -1 && chunk2->x != -1) { //  // && chunk1->x == chunk2->x 
			if (job.ch1x == job.ch2x) {
				int s = rand() % stitch_var_ns.size();
				std::vector<std::vector<int>> style = stitch_var_ns[s];
				// North to south connection 
				//printf("Style NS\n");
				for (int r = 0; r < style.size(); r++) {
					for (int c = 0; c < style[0].size(); c++) {
						// from top left corner of fill region job.location * 6 + 2 to bottom right
						//printf("Stitched NS %d at (%i,%i) and (%i,%i)\n", style[r][c], MAZE_SIZE - 3 + r, job.location * 6 + 2 + c, r, job.location * 6 + 2 + c);
						chunk1->grid[MAZE_SIZE - 3 + r][job.location * 6 + 2 + c] = style[r][c];
						chunk2->grid[r][job.location * 6 + 2 + c] = style[r][c];
					}
					//printf("\n");
				}
				//printf("\n");
			} else {// if (chunk1->x != -1 && chunk2->x != -1 && chunk1->y == chunk2->y) {
				//printf("Style WE\n");
				int s = rand() % stitch_var_we.size();
				std::vector<std::vector<int>> style = stitch_var_we[s];
				// North to south connection 
				for (int r = 0; r < style.size(); r++) {
					for (int c = 0; c < style[0].size(); c++) {
						// from top left corner of fill region job.location * 6 + 2 to bottom right
						//printf("Stitched WE %d at (%i,%i) and (%i,%i)\n", style[r][c], job.location * 6 + 2 + r, MAZE_SIZE - 3 + c, job.location * 6 + 2 + r, c);
						//printf("%d ", style[r][c]);
						chunk1->grid[job.location * 6 + 2 + r][MAZE_SIZE - 3 + c] = style[r][c];
						chunk2->grid[job.location * 6 + 2 + r][c] = style[r][c];
					}
					//printf("\n");
				}
				//printf("\n");
			} //else {
			//	printf("Invalid JOB\n");
			//}
		}

		//printf("END createWorldMap\n");

	}

	for (Chunk& chunk : chunks) {
	//	printf("Chunk:\n");
	//	//printf("position: x= %x, y= %x \n",chunk.x,chunk.y);
	//	//printf("offset: x= %f, y= %f \n", chunk.offset_x, chunk.offset_y);

	//	if (chunk.y == 0) {
	//		for (int i = 0; i < MAZE_SIZE; i++) {
	//			chunk.grid[0][i] = rand() % 3 + 1;
	//		}
	//	}
	//	if (chunk.x == 0) {
	//		for (int i = 0; i < MAZE_SIZE; i++) {
	//			chunk.grid[i][0] = rand() % 3 + 1;
	//		}
	//	}
	//	if (chunk.y == rows-1) {
	//		for (int i = 0; i < MAZE_SIZE; i++) {
	//			chunk.grid[MAZE_SIZE-1][i] = rand() % 3 + 1;
	//		}
	//	}
	//	if (chunk.x == cols-1) {
	//		for (int i = 0; i < MAZE_SIZE; i++) {
	//			chunk.grid[i][MAZE_SIZE-1] = rand() % 3 + 1;
	//		}
	//	}
	//	// ensure starting location is relatively free
	//	// player spawns at window_width_px / 2, window_height_px - 200
	//	//TODO: FIX SPAWN PLAYER POINT
	//	//if (chunk.y == spawn_chunk_row && chunk.x == sparn_chunk_col) {
	//	//	for (int i = start_row; i < fin_row; i++) {
	//	//		for (int j = start_col; j < fin_col; j++) {
	//	//			chunk.grid[i][j] = 0;
	//	//		}
	//	//	}
	//	//}
	// PRINT 
	//	//printf("  ");
	//	//for (int c = 0; c < 11; c++) {
	//	//	printf(" %d", c);
	//	//}

		//PRINT OUT MAP STUFF
		//printf("\n");
		//for (int r = 0; r < MAZE_SIZE; r++) {
		//	printf("%d: ",r);
		//	for (int c = 0; c < MAZE_SIZE; c++) {
		//		// print chunk
		//		if (chunk.grid[r][c] == 1) {
		//			printf("%c%c", (char)219u, (char)219u);
		//		} else {
		//			printf("  ");
		//		}
		//			
		//		//printf(" %x ", chunk.grid[r][c]);
		//	}
		//	printf("\n");
		//}
		//printf("--------\n");
	}
	return true;
}

//// friendlly stitching alg for at min 2x2 chunks 
//if (r == 0) {
//	int d = rand() % 3;
//	int h = rand() % 4; // random connector
//	switch (d) {
//	case 0:	 // don't stitch chunks upper east west
//
//
//		break;
//	case 1: // don't stitch chunks lower east west 
//
//
//		break;
//	case 2: // dont stitch chunks prev north south
//
//
//
//		break;
//	}
//
//
//
//	//} else if (r < (rows - 1)) {	//if not on last row
//} else {
//
//}

void MapGen::getStitchingVector(std::vector<ChunkStitch>& stiching_tasks, int rowz, int colz, int paths) {
	//std::vector<ChunkStitch> stiching_tasks = std::vector<ChunkStitch>();
	// DEBUG
	//ChunkStitch debugstitch = ChunkStitch();
	//debugstitch.location = 3;
	//debugstitch.ch1x = 0;
	//debugstitch.ch1y = 1;
	//debugstitch.ch2y = 1;
	//debugstitch.ch2y = 1;
	//stiching_tasks.push_back(debugstitch);
	//for (ChunkStitch& cs : stiching_tasks) {
	//	printf("ChunkStitch (%d,%d) (%d,%d) :%d\n", cs.ch1x, cs.ch1y, cs.ch2x, cs.ch2y, cs.location);
	//}
	// END DEBUG
	enum { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3, };
	enum { PATH_N = 0x0001, PATH_E = 0x0002, PATH_S = 0x0004, PATH_W = 0x0008, VISITED = 0x0010, };
	const int NR = rowz; 
	const int NC = colz;
	const int NxN = NR* NC;
	for (int path_cycle = 0; path_cycle < paths; path_cycle++) {
		std::vector< std::vector<int> > maze = std::vector<std::vector<int>>();
		//printf("start maze\n");
		for (int r = 0; r < NR; r++) {
			maze.push_back(std::vector<int>());
			for (int c = 0; c < NC; c++) {
				maze[r].push_back(0);
			}
		}
		//printf("All Zeros:\t");
		//for (int r = 0; r < NR; r++) {
		//	for (int c = 0; c < NC; c++) {
		//		printf(" %i ",maze[r][c]);
		//	}
		//} printf(" \n");
		//printf("r/c set to 0 maze\n");
		std::stack< vec2 > path_stack;
		//start randomly
		int start_cell_r = rand() % NR;
		int start_cell_c = rand() % NC;
		// as x,y
		//path_stack.push(vec2(start_cell_c, start_cell_r));
		path_stack.push(vec2(0, 0));
		//maze[start_cell_r][start_cell_c] = VISITED;
		maze[0][0] = VISITED;
		int cells_visited = 1;
		//lambdas for targetting nearby maze cells in relation to top cell  
		//auto dX = [&](int x) { return (topx + x);	};
		//auto dY = [&](int y) { return (topy + y); };
		//printf("maze portion generating ");
		while (cells_visited < NxN) {
			//printf("visiting..\n");
			std::vector<int> neighbours;
			// only look above at North Neighbour Cell if not in top row == 0 also Y is row X is col
			if (!path_stack.empty()) {
				int topx = path_stack.top().x;
				int topy = path_stack.top().y;
				//printf("Top (%i,%i) finds:\t", topx,topy);
				if (topy > 0 && ( (maze[topy-1][topx] & VISITED ) == 0) ) {
					neighbours.push_back(NORTH);
				}
				if (topx < (NC - 1) && ((maze[topy][topx +1] & VISITED) == 0)) {
					neighbours.push_back(EAST); 		// East clear
				}

				if (topy < (NR - 1) && ((maze[topy+1][topx] & VISITED) == 0)) {
					neighbours.push_back(SOUTH); 		// South clear
				}
				if (topx > 0 && ((maze[topy][topx - 1] & VISITED) == 0)) {
					neighbours.push_back(WEST);		// West clear
				}
				// grab rando available direction
				if (!neighbours.empty()) {
					int next_cell_dir = neighbours[rand() % neighbours.size()];
					//printf(" %i \n", next_cell_dir);
					int stitch_location = rand() % 4;
					ChunkStitch stitch = ChunkStitch();
					stitch.location = stitch_location;
					switch (next_cell_dir) {
					case NORTH:
						maze[topy][topx ] |= PATH_N;	// curr cell gets north path
						maze[topy-1 ][topx] |= PATH_S;	// north cell has south path
						path_stack.push( vec2( topx, topy - 1 )); // add north neighbour to stack
						stitch.ch2x = topx;
						stitch.ch1x = topx;
						stitch.ch2y = topy;
						stitch.ch1y = topy-1;
						stiching_tasks.push_back(stitch);
						break;
					case EAST:
						maze[topy][topx] |= PATH_E;
						maze[topy][topx+1] |= PATH_W;	// eastern cell gets a western path
						path_stack.push(vec2(topx + 1, topy)); // add e neighbour to stack
						//stitch.chunk1 = vec2((int)dX(0), (int)dY(0));
						//stitch.chunk2 = vec2((int)dX(1), (int)dY(0));
						stitch.ch2x = topx+1;
						stitch.ch1x = topx;
						stitch.ch2y = topy;
						stitch.ch1y = topy;
						stiching_tasks.push_back(stitch);
						break;
					case SOUTH:
						maze[topy][topx] |= PATH_S;	// curr cell gets south path
						maze[topy+1][topx] |= PATH_N;	// south cell has north path
						path_stack.push(vec2(topx, topy+1)); // add s neighbour to stack
						//stitch.chunk1 = vec2((int)dX(0), (int)dY(0));
						//stitch.chunk2 = vec2((int)dX(0), (int)dY(1));
						stitch.ch2x = topx;
						stitch.ch1x = topx;
						stitch.ch2y = topy+1;
						stitch.ch1y = topy;
						stiching_tasks.push_back(stitch);
						break;
					case WEST:
						maze[topy][topx] |= PATH_W;
						maze[topy][topx - 1] |= PATH_E;	// western cell gets a eastern path
						path_stack.push(vec2(topx-1, topy)); // add w neighbour to stack
						//stitch.chunk2 = vec2((int)dX(0), (int)dY(0));
						//stitch.chunk1 = vec2((int)dX(-1), (int)dY(0));
						stitch.ch2x = topx;
						stitch.ch1x = topx - 1;
						stitch.ch2y = topy;
						stitch.ch1y = topy;
						stiching_tasks.push_back(stitch);
						//printf("%d\n", topx);
						//printf("%d\n", topy);
						//printf("%d\n", topx+1);
						//printf("%d\n", topy+1);
						//printf("%d\n", topx-1);
						//printf("%d\n", topy-1);
						break;
					}
					maze[topy][topx] |= VISITED;
					cells_visited++;
				} else {
					if (!path_stack.empty()) {
						//stiching_tasks.push_back(path_stack.top());
						//printf("POP (%d,%d)\n", topx, topy);
						path_stack.pop();
					} else {
					}
				}
			}
		}
		//printf("Maze numbers:\t");
		//for (int r = 0; r < NR; r++) {
		//	for (int c = 0; c < NC; c++) {
		//		printf(" %i ", (int)maze[r][c]);
		//	}
		//	printf(" \n");
		//} printf(" \n");
		//while (!path_stack.empty()) {
			//stiching_tasks.push_back(path_stack.top());
			//path_stack.pop();
		//}

	}
	//for (ChunkStitch& cs: stiching_tasks) {
	//	printf("ChunkStitch (%d,%d) (%d,%d) :%d\n",cs.ch1x, cs.ch1y, cs.ch2x, cs.ch2y, cs.location );
	//}
	//return &stiching_tasks;
}


// 
// okay so the x,y getting switched in some places wasn't planned i think in x,y .. but arrays are row col
Chunk& MapGen::createMapChunk(vec2 position) {
	enum { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3, };
	enum { PATH_N = 0x01, PATH_E = 0x02, PATH_S = 0x04, PATH_W = 0x08, VISITED = 0x10, };

	Chunk *chunk = new Chunk();
	//chunk.x = (int)position.x;
	//chunk.y = (int)position.y;
	//chunk.offset_x = position.x * MAZE_SIZE * TILE_SIZE;
	//chunk.offset_y = position.y * MAZE_SIZE * TILE_SIZE;

	chunk->x = (int)position.x;
	chunk->y = (int)position.y;
	chunk->offset_x = position.x * MAZE_SIZE * TILE_SIZE;
	chunk->offset_y = position.y * MAZE_SIZE * TILE_SIZE;

	//int alternate_map = rand() % 5;
	//if (alternate_map < 1) {
	//	int alternate_type = rand() % 10;
	//	if (alternate_map < 8) {
	//		if (alternate_map % 2 == 0) {
	//			for (int r = 0; r < MAZE_SIZE; r++) {
	//				for (int c = 0; c < MAZE_SIZE; c++) {
	//					chunk->grid[r][c] = grid_alt_base1[r][c];
	//				}
	//			}
	//		} else {
	//			for (int r = 0; r < MAZE_SIZE; r++) {
	//				for (int c = 0; c < MAZE_SIZE; c++) {
	//					chunk->grid[r][c] = grid_alt_base2[r][c];
	//				}
	//			}
	//		}
	//	} else {
	//		for (int r = 0; r < MAZE_SIZE; r++) {
	//			for (int c = 0; c < MAZE_SIZE; c++) {
	//				chunk->grid[r][c] = grid_pillars[r][c];
	//			}
	//		}
	//		return *chunk;
	//	}
	//}

	const int N = 4; // MAZE_SIZE / 4;			// assert always a multiple of 4?
	const int NxN = 16;
	int maze[4][4];
	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			maze[r][c] = 0;
		}
	}
	//int num_cells = N * N; 
	std::stack< std::pair<int, int> > path_stack;
	//start in arbitrary top left
	path_stack.push(std::make_pair(0, 0));
	maze[0][0] = VISITED;
	int cells_visited = 1;
	//lambdas for targetting nearby maze cells in relation to top cell  
	auto dX = [&](int x) { return (path_stack.top().first + x);	};
	auto dY = [&](int y) { return (path_stack.top().second + y); };
	//printf("maze portion generating ");
	while (cells_visited < NxN) {
		///printf(".");
		std::vector<int> neighbours;
		// only look above at North Neighbour Cell if not in top row == 0 also Y is row X is col
		if (!path_stack.empty()) {
			if (path_stack.top().second > 0 && ((maze[dY(-1)][dX(0)] & VISITED) == 0)) {
				neighbours.push_back(NORTH);
			}
			if (path_stack.top().first < (N - 1) && ((maze[dY(0)][dX(1)] & VISITED) == 0)) {
				neighbours.push_back(EAST); 		// East clear
			}

			if (path_stack.top().second < (N - 1) && ((maze[dY(1)][dX(0)] & VISITED) == 0)) {
				neighbours.push_back(SOUTH); 		// South clear
			}
			if (path_stack.top().first > 0 && ((maze[dY(0)][dX(-1)] & VISITED) == 0)) {
				neighbours.push_back(WEST);		// West clear
			}
			// grab rando available direction
			if (!neighbours.empty()) {
				int next_cell_dir = neighbours[rand() % neighbours.size()];
				switch (next_cell_dir) {
				case NORTH:
					maze[dY(0)][dX(0)] |= PATH_N;	// curr cell gets north path
					maze[dY(-1)][dX(0)] |= PATH_S;	// north cell has south path
					path_stack.push(std::make_pair(dX(0), dY(-1))); // add north neighbour to stack
					break;
				case EAST:
					maze[dY(0)][dX(0)] |= PATH_E;
					maze[dY(0)][dX(1)] |= PATH_W;	// eastern cell gets a western path
					path_stack.push(std::make_pair(dX(1), dY(0))); // add e neighbour to stack
					break;
				case SOUTH:
					maze[dY(0)][dX(0)] |= PATH_S;	// curr cell gets south path
					maze[dY(1)][dX(0)] |= PATH_N;	// south cell has north path
					path_stack.push(std::make_pair(dX(0), dY(1))); // add s neighbour to stack
					break;
				case WEST:
					maze[dY(0)][dX(0)] |= PATH_W;
					maze[dY(0)][dX(-1)] |= PATH_E;	// western cell gets a eastern path
					path_stack.push(std::make_pair(dX(-1), dY(0))); // add w neighbour to stack
					break;
				}
				maze[dY(0)][dX(0)] |= VISITED;
				cells_visited++;
				//printf(".");
			} else {
				if (!path_stack.empty()) {
					//printf("^(%d,%d)", path_stack.top().first, path_stack.top().second);
					//printf("^");
					path_stack.pop();
				} else {
				}
			}
		}
	}
	//printf("\n");
	while (!path_stack.empty()) {
		path_stack.pop();
	}

	//printf("chunk maze complete\n");
	//// maze like thing generated but now to convert to out actual tilemade grid (all ones so far)
	//// TODO: add Worley noise filter modulation to make more organic?
	//// okay this is the fun part where this mini maze thing is now a cell filter
	//// we just pop holes where we need to 
	int node_size = 6;
	int node_edge = node_size - 1; // 5
	const int BLOCK_RARITY = 7;
	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			//printf("NODE CONNECTION %d %d\n", r, c);
			// fill in the centers of the paths with zeros 
			// TODO: add random path chunks 
			std::vector<std::vector< std::vector<int>>> chosen_path_vector;
			if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) != 0) {
				chosen_path_vector = path_var_all;
			} else if ((maze[r][c] & PATH_N) == 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) == 0 && (maze[r][c] & PATH_W) != 0) {
				// horiz
				chosen_path_vector = path_var_horiz;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) == 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) == 0) {
				// verical
				chosen_path_vector = path_var_vert;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) == 0 && (maze[r][c] & PATH_W) == 0) {
				// corner ne 
				chosen_path_vector = path_var_corne;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) == 0 && (maze[r][c] & PATH_W) != 0) {
				// c nw
				chosen_path_vector = path_var_cornw;
			} else if ((maze[r][c] & PATH_N) == 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) == 0) {
				// c se
				chosen_path_vector = path_var_corse;
			} else if ((maze[r][c] & PATH_N) == 0 && (maze[r][c] & PATH_E) == 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) != 0) {
				// c sw
				chosen_path_vector = path_var_corsw;
			} else if ((maze[r][c] & PATH_N) == 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) != 0) {
				// not n 
				chosen_path_vector = path_var_notn;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) == 0 && (maze[r][c] & PATH_W) != 0) {
				// not s
				chosen_path_vector = path_var_nots;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) == 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) != 0) {
				// not e
				chosen_path_vector = path_var_note;
			} else if ((maze[r][c] & PATH_N) != 0 && (maze[r][c] & PATH_E) != 0 && (maze[r][c] & PATH_S) != 0 && (maze[r][c] & PATH_W) == 0) {
				// not w
				chosen_path_vector = path_var_notw;
			} else {
				// cover edge cases after like single ends etc
				chosen_path_vector = path_var_all;
			}





			// choose relevant fill for paths
			// std::vector
			int random_path_var = rand() % chosen_path_vector.size();
			auto& path = chosen_path_vector.at(random_path_var);
			//for (int ry = 1; ry < node_edge; ry ++) {
			//	for (int cx = 1; cx < node_edge; cx++) {
			//		chunk->grid[1+ r * node_size + ry][1+ c * node_size + cx] = 0;
			//	}
			//}
			for (int ry = 0; ry < PV_SIZE; ry ++) {
				for (int cx = 0; cx < PV_SIZE; cx++) {
					chunk->grid[2+ r * (node_size) + ry][2+ c * (node_size) + cx] = path[ry][cx];
				}
			}

			// i is basically going though [-XXXX-]  +/- an X
			//printf("North\n");
			if ((maze[r][c] & PATH_N) != 0) {
				// make rare blocking plants  
				int rand_start = 1; if (rand()% BLOCK_RARITY == 0) { rand_start++;	}
				int rand_end = node_edge; if (rand() % BLOCK_RARITY == 0) {rand_end--;	}
				for (int i = rand_start; i < rand_end; i++) {
					chunk->grid[1 + r * node_size][1 + c * node_size + i] = 0;
					//printf("(%d,%d)= %d   ",(1 + r * node_size),(1 + c * node_size + i), chunk->grid[1 + r * node_size][1 + c * node_size + i]);
				}
				//printf("\n");
			}
			//printf("East\n");
			if ((maze[r][c] & PATH_E) != 0) {
				int rand_start = 1; if (rand() % BLOCK_RARITY == 0) { rand_start++; }
				int rand_end = node_edge; if (rand() % BLOCK_RARITY == 0) { rand_end--; }
				for (int i = rand_start; i < rand_end; i++) {
					chunk->grid[1 + r * node_size + i][1 + c * node_size + node_edge] = 0;
					//printf("(%d,%d)= %d   ", (1 + r * node_size + i), (1 + c * node_size + node_edge), chunk->grid[1 + r * node_size + i][1 + c * node_size + node_edge]);
				}
				//printf("\n");
			}
			//printf("South\n");
			if ((maze[r][c] & PATH_S) != 0) {
				int rand_start = 1; if (rand() % BLOCK_RARITY == 0) { rand_start++; }
				int rand_end = node_edge; if (rand() % BLOCK_RARITY == 0) { rand_end--; }
				for (int i = rand_start; i < rand_end; i++) {
					chunk->grid[1 + r * node_size + node_edge][1 + c * node_size + i] = 0;
					//printf("(%d,%d)= %d   ", (1 + r * node_size + node_edge), (1 + c * node_size + i), chunk->grid[1 + r * node_size + node_edge][1 + c * node_size + i]);
				}
				//printf("\n");
			}
			//printf("West\n");
			if ((maze[r][c] & PATH_W) != 0) {
				int rand_start = 1; if (rand() % BLOCK_RARITY == 0) { rand_start++; }
				int rand_end = node_edge; if (rand() % BLOCK_RARITY == 0) { rand_end--; }
				for (int i = rand_start; i < rand_end; i++) {
					chunk->grid[1 + r * node_size + i][1 + c * node_size] = 0;
					//printf("(%d,%d)= %d   ", (1 + r * node_size + i), (1 + c * node_size), chunk->grid[1 + r * node_size + i][1 + c * node_size]);
				}
				//printf("\n");
			}
			//printf("\n");
		}
	}
	//printf("chunk punching grid complete\n");
	//printf("Chunk:\n");
	//printf("position: x= %x, y= %x \n", chunk->x, chunk->y);
	////printf("offset: x= %f, y= %f \n", chunk.offset_x, chunk.offset_y);
	//for (int r = 0; r < MAZE_SIZE; r++) {
	//	for (int c = 0; c < MAZE_SIZE; c++) {
	//		printf(" %x ", chunk->grid[r][c]);
	//	}
	//	printf("\n");
	//}
	//printf("--------\n");
	
	return *chunk;

}


void rngFillGrid(int* grid, int w, int h) {
	for (int r = 0; r < h; r++) {
		for (int c = 0; c < w; c++) {
			grid[r*w + c] = rand() % 10;
		}
	}
}


//void addCnstIntArray(std::vector<std::vector<std::vector<int>>* vect, int[][] arr ) {
//}
void MapGen::initPaths() {
	//path_var_all = std::vector<int[PV_SIZE][PV_SIZE]>(); doesn't work vectors must be copy constructable
	path_var_all = std::vector< std::vector< std::vector<int> > >();
	path_var_horiz = std::vector< std::vector< std::vector<int>>>();
	path_var_vert = std::vector< std::vector< std::vector<int>>>();
	path_var_notn = std::vector< std::vector< std::vector<int>>>();
	path_var_notw = std::vector< std::vector< std::vector<int>>>();
	path_var_nots = std::vector< std::vector< std::vector<int>>>();
	path_var_note = std::vector< std::vector< std::vector<int>>>();
	path_var_corne = std::vector< std::vector< std::vector<int>>>();
	path_var_cornw = std::vector< std::vector< std::vector<int>>>();
	path_var_corse = std::vector< std::vector< std::vector<int>>>();
	path_var_corsw = std::vector< std::vector< std::vector<int>>>();
	stitch_var_we = std::vector< std::vector< std::vector<int>>>();
	stitch_var_ns = std::vector< std::vector< std::vector<int>>>();


	std::vector < std::vector<int> > temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_clear) {
		temp.push_back(std::vector<int>(std::begin(row), std::end(row)));
	}
	path_var_all.push_back(temp);
	path_var_horiz.push_back(temp);
	path_var_vert.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_round) {
		temp.push_back(std::vector<int>(std::begin(row), std::end(row)));
	}
	path_var_all.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_d1) {
		temp.push_back(std::vector<int>(std::begin(row), std::end(row)));
	}
	path_var_all.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_d2) {
		temp.push_back(std::vector<int>(std::begin(row), std::end(row)));
	}
	path_var_all.push_back(temp);

	// Then more vectors
	//{ 0,0,0,1 },
	//{ 0,0,0,1 },
	//{ 0,0,0,1 },
	//{ 0,0,0,1 },
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_west) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); 	}
	path_var_note.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_east) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_notw.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_north) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_nots.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_south) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_notn.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_noe) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_note.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_now) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_notw.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_nos) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_nots.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_non) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_notn.push_back(temp);

	// VERTICAL
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_v1) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_vert.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_v2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_vert.push_back(temp);

	// HORIZONTAL
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_h1) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_horiz.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_h2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_horiz.push_back(temp);

	//// CORNERS
	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_cne) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_corne.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_cnw) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_cornw.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_cse) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_corse.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : path_var_csw) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	path_var_corsw.push_back(temp);

	//temp = std::vector < std::vector<int> >();
	//for (const auto& row : path_var_cne2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	//path_var_corne.push_back(temp);

	//temp = std::vector < std::vector<int> >();
	//for (const auto& row : path_var_cnw2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	//path_var_cornw.push_back(temp);

	//temp = std::vector < std::vector<int> >();
	//for (const auto& row : path_var_cse2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	//path_var_corse.push_back(temp);

	//temp = std::vector < std::vector<int> >();
	//for (const auto& row : path_var_csw2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	//path_var_corsw.push_back(temp);
	//printf("Path Vector Size %x ", path_var_all.size());

	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ew1) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_we.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ew2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_we.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ew3) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_we.push_back(temp);

	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ns1) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_ns.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ns2) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_ns.push_back(temp);
	temp = std::vector < std::vector<int> >();
	for (const auto& row : stitch_var_ns3) { temp.push_back(std::vector<int>(std::begin(row), std::end(row))); }
	stitch_var_ns.push_back(temp);

}

