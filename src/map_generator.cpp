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



MapGen::MapGen(int maprows, int mapcols) {
	// init noise grid withnoise levels of noise
	rows = maprows;
	cols = mapcols;
	const int RARITY = 14;
	const int NOISE_LEVELS = 10;
	const int BIG_WIDTH = cols * MAZE_SIZE;
	const int BIG_HEIGHT = rows * MAZE_SIZE;
	chunks = std::vector<Chunk>();
	bignoise = new int* [BIG_HEIGHT];
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		bignoise[i] = new int[BIG_WIDTH];
		for (int j = 0; j < BIG_WIDTH; ++j) {
			bignoise[i][j] = rand() % NOISE_LEVELS; //random number between 0 and 4 incl
			//printf(" %x", bignoise[i][j]);
		}
		//printf("\n");
	}

	// init biome filter grid with 1s
	biome_filter = new int* [BIG_HEIGHT];
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		biome_filter[i] = new int[BIG_WIDTH];
		for (int j = 0; j < BIG_WIDTH; ++j) {
			biome_filter[i][j] = 1;
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
		}
	}
	for (int i = 0; i < BIG_HEIGHT; ++i) {
		for (int j = 0; j < BIG_WIDTH; ++j) {
			printf(" %x", biome_filter[i][j]);
		}
		printf("\n");
	}
}

MapGen::~MapGen() {
	//delete chunks;
}

// feed into the connectio row, cols, d, h into create?
bool MapGen::createWorldMap() {
	int sparn_chunk_col = (window_width_px / 2) / (MAZE_SIZE * TILE_SIZE);
	int spawn_chunk_row = (window_height_px - 200) / (MAZE_SIZE * TILE_SIZE);
	int start_row = ((window_height_px - 200) / TILE_SIZE) % MAZE_SIZE - 2;
	int fin_row = ((window_height_px - 200) / TILE_SIZE) % MAZE_SIZE + 3;
	int start_col = (window_width_px / 2 / TILE_SIZE) % MAZE_SIZE - 2;
	int fin_col = (window_width_px / 2 / TILE_SIZE) % MAZE_SIZE + 3;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			Chunk& chunk = createMapChunk(vec2{ r, c });
			chunks.push_back(chunk);
			
		}
	}

	for (Chunk& chunk : chunks) {
		printf("Chunk:\n");
		printf("position: x= %x, y= %x \n",chunk.x,chunk.y);
		//printf("offset: x= %f, y= %f \n", chunk.offset_x, chunk.offset_y);

		if (chunk.y == 0) {
			for (int i = 0; i < MAZE_SIZE; i++) {
				chunk.grid[0][i] = rand() % 3 + 1;
			}
		}
		if (chunk.x == 0) {
			for (int i = 0; i < MAZE_SIZE; i++) {
				chunk.grid[i][0] = rand() % 3 + 1;
			}
		}
		if (chunk.y == rows-1) {
			for (int i = 0; i < MAZE_SIZE; i++) {
				chunk.grid[MAZE_SIZE-1][i] = rand() % 3 + 1;
			}
		}
		if (chunk.x == cols-1) {
			for (int i = 0; i < MAZE_SIZE; i++) {
				chunk.grid[i][MAZE_SIZE-1] = rand() % 3 + 1;
			}
		}
		// ensure starting location is relatively free
		// player spawns at window_width_px / 2, window_height_px - 200

		if (chunk.y == spawn_chunk_row && chunk.x == sparn_chunk_col) {
			for (int i = start_row; i < fin_row; i++) {
				for (int j = start_col; j < fin_col; j++) {
					chunk.grid[i][j] = 0;
				}
			}
		}




		for (int r = 0; r < MAZE_SIZE; r++) {
			for (int c = 0; c < MAZE_SIZE; c++) {

				// print chunk
				if (chunk.grid[r][c] == 1) {
					printf("%c%c", (char)219u, (char)219u);
				} else {
					printf("  ");
				}
					

				//printf(" %x ", chunk.grid[r][c]);
			}
			printf("\n");
		}
		printf("--------\n");
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

	int alternate_map = rand() % 5;
	if (alternate_map < 1) {
		int alternate_type = rand() % 10;
		if (alternate_map < 8) {
			if (alternate_map % 2 == 0) {
				for (int r = 0; r < MAZE_SIZE; r++) {
					for (int c = 0; c < MAZE_SIZE; c++) {
						chunk->grid[r][c] = grid_alt_base1[r][c];
					}
				}
			} else {
				for (int r = 0; r < MAZE_SIZE; r++) {
					for (int c = 0; c < MAZE_SIZE; c++) {
						chunk->grid[r][c] = grid_alt_base2[r][c];
					}
				}
			}
		} else {
			for (int r = 0; r < MAZE_SIZE; r++) {
				for (int c = 0; c < MAZE_SIZE; c++) {
					chunk->grid[r][c] = grid_pillars[r][c];
				}
			}
			return *chunk;
		}
	}

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
	printf("\n");
	while (!path_stack.empty()) {
		path_stack.pop();
	}

	//printf("chunk maze complete\n");
	//// maze like thing generated but now to convert to out actual tilemade grid (all ones so far)
	//// TODO: add Worley noise filter modulation to make more organic?
	//// okay this is the fun part where this mini maze thing is now a cell filter
	//// we just pop holes where we need to 
	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			//chunk.grid[r * 4 + 1][c * 4 + 1] = 0;
			//chunk.grid[r * 4 + 1][c * 4 + 2] = 0;
			//chunk.grid[r * 4 + 2][c * 4 + 1] = 0;
			//chunk.grid[r * 4 + 2][c * 4 + 2] = 0;
			if ((maze[r][c] & PATH_N) != 0) {
				chunk->grid[r * 4][c * 4 + 1] = 0;
				chunk->grid[r * 4][c * 4 + 2] = 0;
			}
			if ((maze[r][c] & PATH_E) != 0) {
				chunk->grid[r * 4 + 1][c * 4 + 3] = 0;
				chunk->grid[r * 4 + 2][c * 4 + 3] = 0;
			}
			if ((maze[r][c] & PATH_S) != 0) {
				chunk->grid[r * 4 + 3][c * 4 + 1] = 0;
				chunk->grid[r * 4 + 3][c * 4 + 2] = 0;
			}
			if ((maze[r][c] & PATH_W) != 0) {
				chunk->grid[r * 4 + 1][c * 4] = 0;
				chunk->grid[r * 4 + 2][c * 4] = 0;
			}
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




