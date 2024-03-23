// internal
#include "ai_system.hpp"
#include <iostream>
#include <algorithm>


// MazeLocation is value within overall maze. Not limited to one chunk.
inline int get_chunk_value(MazeLocation location) {
	int chunk_x = location.x / MAZE_SIZE;
	int chunk_y = location.y / MAZE_SIZE;
	//std::cout << "CHUNK X: " << chunk_x << " CHUNK Y: " << chunk_y << std::endl;

	for (const Chunk& chunk : registry.chunks.components) {
		if (chunk.x == chunk_x && chunk.y == chunk_y) {
			// we should convert the location to location within chunk
			location.y = location.y % MAZE_SIZE;
			location.x = location.x % MAZE_SIZE;
			return chunk.grid[location.y][location.x];
		}
	}
	return -1;
}

// returns the tile location of the bottom part of the keeper
inline MazeLocation get_maze_location(vec2 location) {
	int row = (int)(location.y / TILE_SIZE);
	int col = (int)(location.x / TILE_SIZE);

	return MazeLocation{ col, row };
}

inline std::vector<Direction> get_possible_directions(MazeLocation loc) {
	std::vector<Direction> directions;

	// can only go right if we can go right and right and up (loc is bottom part of keeper)
	int right = get_chunk_value({ loc.x + 1, loc.y });
	int right_and_up = get_chunk_value({ loc.x + 1, loc.y - 1 });
	if (right == 0 && right_and_up == 0)
	{
		directions.push_back(Direction::RIGHT);
	}

	// can only go left if we can go left and left and up (loc is bottom part of keeper)
	int left = get_chunk_value({ loc.x - 1, loc.y });
	int left_and_up = get_chunk_value({ loc.x - 1, loc.y - 1 });
	if (left == 0 && left_and_up == 0)
	{
		directions.push_back(Direction::LEFT);
	}

	// can only go down if we can go down once
	int down = get_chunk_value({ loc.x, loc.y + 1 });
	if (down == 0)
	{
		directions.push_back(Direction::DOWN);
	}

	// can only go up if we can go up twice (loc is bottom part of keeper)
	int up = get_chunk_value({ loc.x, loc.y - 2 });
	if (up == 0)
	{
		directions.push_back(Direction::UP);
	}

	//print_directions(directions);

	return directions;
}

void print_directions(std::vector<Direction> directions)
{
	for (const auto& direction : directions)
	{
		switch (direction)
		{
		case Direction::RIGHT:
			std::cout << "RIGHT" << std::endl;
			break;
		case Direction::LEFT:
			std::cout << "LEFT" << std::endl;
			break;
		case Direction::UP:
			std::cout << "UP" << std::endl;
			break;
		case Direction::DOWN:
			std::cout << "DOWN" << std::endl;
			break;
		}
	}
}


AISystem::AISystem() {
	auto rng = std::default_random_engine{};
}

void AISystem::step() 
{
	ComponentContainer<Enemy>& enemies_container = registry.enemies;
	ComponentContainer<Motion>& motion_container = registry.motions;
	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);

	for (int i = 0; i < enemies_container.entities.size(); i++) {
		// get relevant entities and components
		const Entity& keeper_entity = enemies_container.entities[i];
		Enemy& keeper = enemies_container.components[i];
		Motion& keeper_motion = motion_container.get(keeper_entity);

		// chasing
		float xDiff = player_motion.position[0] - keeper_motion.position[0];
		float yDiff = player_motion.position[1] - keeper_motion.position[1];
		float distance = std::sqrt(pow(xDiff, 2) + pow(yDiff, 2));

		if (distance < chase_distance)
		{
			if (!registry.colors.has(keeper_entity))
			{
				registry.colors.insert(keeper_entity, vec3{ 1.0, 0.0, 0.0 });
			}
		}
		else
		{
			registry.colors.remove(keeper_entity);
		}

		vec2 adjusted_keeper_pos = keeper_motion.position;
		adjusted_keeper_pos.y = adjusted_keeper_pos.y + (KEEPER_HEIGHT / 2) - (TILE_SIZE / 2);

		// get possible directions within overall maze
		MazeLocation loc = get_maze_location(adjusted_keeper_pos);
		std::vector<Direction> directions = get_possible_directions(loc);

		// only pick a new direction if the last direction isn't available
		bool dead_end = true;
		for (const auto& direction : directions)
		{
			if (direction == keeper.last_direction)
			{
				dead_end = false;
			}
		}
		if (!dead_end)
		{
			continue;
		}

		// shuffle for randomness
		std::shuffle(std::begin(directions), std::end(directions), rng);

		Direction new_keeper_direction = directions[0];
		keeper.last_direction = new_keeper_direction;
		keeper_motion.velocity = direction_to_velocity[new_keeper_direction];
	}



	return;
}
