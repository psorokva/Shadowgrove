// internal
#include "ai_system.hpp"
#include <iostream>
#include <algorithm>
#include <queue>


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
			//std::cout << "RIGHT" << std::endl;
			break;
		case Direction::LEFT:
			//std::cout << "LEFT" << std::endl;
			break;
		case Direction::UP:
			//std::cout << "UP" << std::endl;
			break;
		case Direction::DOWN:
			//std::cout << "DOWN" << std::endl;
			break;
		}
	}
}


AISystem::AISystem() {
	auto rng = std::default_random_engine{};
}


int compute_heuristic(MazeLocation start, MazeLocation end)
{
	int xDiff = abs(start.x - end.x);
	int yDiff = abs(start.y - end.y);

	return xDiff + yDiff;
}

Direction AISystem::AStar(vec2 player_location, vec2 keeper_location)
{
	// max step amount ?
	int steps = 0;
	std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> pq;
	//std::set<AStarNode> openSet;
	std::map<MazeLocation, int> gScore;

	MazeLocation keeper_loc = get_maze_location(keeper_location);
	MazeLocation player_loc = get_maze_location(player_location);

	AStarNode start;
	start.maze_loc = keeper_loc;
	start.gScore = 0;
	start.fScore = compute_heuristic(keeper_loc, player_loc) + start.gScore;
	gScore[start.maze_loc] =  0;
	pq.push(start);

	bool first_step = true;

	while (pq.size() > 0)
	{
		steps++;
		if (steps > max_astar_steps)
		{
			return Direction::NONE;
		}
		AStarNode current = pq.top();

		if (current.maze_loc == player_loc)
		{
			return current.parent;
		}
		pq.pop();

		std::vector<Direction> directions = get_possible_directions(current.maze_loc);

		for (const auto& direction : directions)
		{
			AStarNode neighborNode;
			MazeLocation step = direction_to_location[direction];
			MazeLocation new_loc = current.maze_loc;
			new_loc.x = new_loc.x + step.x;
			new_loc.y = new_loc.y + step.y;
			neighborNode.maze_loc = new_loc;
			neighborNode.gScore = gScore[current.maze_loc] + 1;
			if (first_step)
			{
				neighborNode.parent = direction;
			}
			else
			{
				neighborNode.parent = current.parent;
			}
			bool new_node = false;
			if (gScore.find(neighborNode.maze_loc) == gScore.end())
			{
				gScore[neighborNode.maze_loc] = neighborNode.gScore;
				new_node = true;
			}
			if (neighborNode.gScore < gScore[neighborNode.maze_loc] || new_node)
			{
				gScore[neighborNode.maze_loc] = neighborNode.gScore;
				neighborNode.fScore = neighborNode.gScore + compute_heuristic(neighborNode.maze_loc, player_loc);
				pq.push(neighborNode);

			}
		}
		first_step = false;
	}

	return Direction::NONE;
}


void AISystem::step() 
{
	enemy_step();
	floating_enemy_step();
}


void AISystem::floating_enemy_step()
{
	auto& enemy_registry = registry.floating_enemies;
	auto& player = registry.players.entities[0];
	Motion player_position = registry.motions.get(player);
	for (uint i = 0; i < enemy_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		FloatingEnemy& enemy = enemy_registry.components[i];
		Entity entity = enemy_registry.entities[i];
		Motion& enemy_motion = registry.motions.get(entity);
		float xDiff = player_position.position[0] - enemy_motion.position[0];
		float yDiff = player_position.position[1] - enemy_motion.position[1];
		float distance = std::sqrt(pow(xDiff, 2) + pow(yDiff, 2));

		float normalized_xDiff = (xDiff) / distance;
		float normalized_yDiff = (yDiff) / distance;

		enemy_motion.velocity = { normalized_xDiff * floating_enemy_velocity_multiplier, normalized_yDiff * floating_enemy_velocity_multiplier };
	}
}


void AISystem::enemy_step()
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

		vec2 adjusted_keeper_pos = keeper_motion.position;
		adjusted_keeper_pos.y = adjusted_keeper_pos.y + (KEEPER_HEIGHT / 2) - (TILE_SIZE / 2);

		vec2 adjusted_player_pos = player_motion.position;
		adjusted_player_pos.y = adjusted_player_pos.y + (PLAYER_HEIGHT / 2);

		// ------- Chase Player state -------

		if (distance < chase_distance)
		{
			if (!registry.colors.has(keeper_entity))
			{
				registry.colors.insert(keeper_entity, vec3{ 1.0, 0.0, 0.0 });
			}
			Direction toGo = AStar(adjusted_player_pos, adjusted_keeper_pos);
			// in case AStar somehow fails we have a check
			if (toGo != Direction::NONE)
			{
				keeper_motion.velocity = direction_to_velocity[toGo] + direction_to_velocity[toGo];
				keeper.last_direction = toGo;
				continue;
			}
		}
		else
		{
			registry.colors.remove(keeper_entity);
		}

		// ------- Wander Randomly state -------

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
			keeper_motion.velocity = direction_to_velocity[keeper.last_direction];
			continue;
		}

		// shuffle for randomness
		std::shuffle(std::begin(directions), std::end(directions), rng);


		Direction new_keeper_direction;
		// we could potentially have a scenario in which no direction is valid and then we crash? so adding check here...
		if ((int)directions.size() == 0)
		{
			// hopefully we never end up here
			new_keeper_direction = Direction::LEFT;
		}
		else
		{
			new_keeper_direction = directions[0];
		}
		keeper.last_direction = new_keeper_direction;
		keeper_motion.velocity = direction_to_velocity[new_keeper_direction];
	}
	return;
}
