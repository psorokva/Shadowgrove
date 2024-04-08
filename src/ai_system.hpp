#pragma once

#include <vector>
#include <map>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include <random>
#include <algorithm>

inline int get_chunk_value(MazeLocation location);
inline MazeLocation get_maze_location(MazeLocation loc);
inline std::vector<Direction> get_possible_directions(vec2 location);
void print_directions(std::vector<Direction> directions);

class AISystem {
public:
	AISystem();
	void step();
private:
	Direction AStar(vec2 player_location, vec2 keeper_location);
	void enemy_step();
	void floating_enemy_step();

	const int enemy_velocity_multipler = 80;
	const int floating_enemy_velocity_multiplier = 50;

	const int step_size = PLAYER_HEIGHT / 2;
	const int frames_till_update = 30;
	const int chase_distance = 300;

	const int max_astar_steps = 100;

	std::unordered_map<Direction, vec2> direction_to_velocity = {
		{LEFT, {-enemy_velocity_multipler, 0} },
		{RIGHT, {enemy_velocity_multipler, 0} },
		{UP, {0, -enemy_velocity_multipler} },
		{DOWN, {0, enemy_velocity_multipler} },
		{NONE, {0, 0}}
	};

	std::unordered_map<Direction, MazeLocation> direction_to_location = {
		{LEFT, {-1, 0} },
		{RIGHT, {1, 0} },
		{UP, {0, - 1} },
		{DOWN, {0, 1 } },
		{NONE, {0, 0}}
	};

	std::unordered_map<Direction, Direction> opposite_directions = {
		{LEFT, RIGHT},
		{RIGHT, LEFT},
		{UP, DOWN},
		{DOWN, UP},
		{NONE, NONE}
	};

	struct AStarNode {
		MazeLocation maze_loc;
		float fScore;
		float gScore;
		Direction parent;

		bool operator > (const AStarNode& node) const
		{
			return fScore > node.fScore;
		}
		bool operator >= (const AStarNode& node) const
		{
			return fScore >= node.fScore;
		}
		bool operator < (const AStarNode& node) const
		{
			return fScore < node.fScore;
		}
		bool operator <= (const AStarNode& node) const
		{
			return fScore <= node.fScore;
		}
		bool operator == (const AStarNode& node) const
		{
			return maze_loc == node.maze_loc;
		}
		bool operator != (const AStarNode& node) const
		{
			return !(fScore == node.fScore);
		}
	};

	std::default_random_engine rng;
};