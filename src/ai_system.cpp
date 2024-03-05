// internal
#include "ai_system.hpp"
#include <iostream>


AISystem::AISystem() {
	auto rng = std::default_random_engine{};
}

void AISystem::step() 
{
	ComponentContainer<Immobile>& immobiles_container = registry.immobiles;
	ComponentContainer<Enemy>& enemies_container = registry.enemies;
	ComponentContainer<Motion>& motion_container = registry.motions;
	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);


	for (uint j = 0; j < enemies_container.size(); j++) {
		Enemy& enemy = enemies_container.components[j];
		if (enemy.frame_count < frames_till_update) {
			enemy.frame_count += 1;
			continue;
		}
		Entity& enemy_entity = enemies_container.entities[j];
		Motion& enemy_motion = motion_container.get(enemy_entity);
		std::vector<bool> illegal_directions = { false, false, false, false };

		std::vector<Direction> direction_to_player;

		// calculate player direction from enemy
		if (player_motion.position.x < enemy_motion.position.x) {
			direction_to_player.push_back(LEFT);
		}
		else {
			direction_to_player.push_back(RIGHT);
		}
		if (player_motion.position.y < enemy_motion.position.y) {
			direction_to_player.push_back(UP);
		}
		else {
			direction_to_player.push_back(DOWN);
		}

		//std::cout << "DIRECTION TO PLAYER IS " << direction_to_player[0] << "DIRECTION" << std::endl;
		//std::cout << "DIRECTION TO PLAYER IS " << direction_to_player[1] << "DIRECTION" << std::endl;

		std::set<Direction> new_possible_directions;

		//for (int k = 0; k < 4; k++) {
		//	if (enemy.last_possible_directions.find(static_cast<Direction>(k)) != enemy.last_possible_directions.end()) {
		//		std::cout << "DIRECTION: " << k << " IN LAST ITERATION" << std::endl;
		//	}
		//}

		//std::cout << "--------------" << std::endl;
		Entity player = registry.players.entities[0];
		Motion& player_motion = registry.motions.get(player);
		float xDiff = player_motion.position[0] - enemy_motion.position[0];
		float yDiff = player_motion.position[1] - enemy_motion.position[1];
		float distance = std::sqrt(pow(xDiff, 2) + pow(yDiff, 2));

		// shuffle to get some randomness in enemy decision
		std::shuffle(std::begin(direction_to_player), std::end(direction_to_player), rng);

		for (uint i = 0; i < immobiles_container.components.size(); i++) {
			Immobile& immobile = immobiles_container.components[i];

			vec2 direction;
			// left
			direction = { -step_size, 0 };
			if (!check_step(enemy_motion, immobile, direction)) {
				illegal_directions[LEFT] = true;
			}
			// right
			direction = { step_size, 0 };
			if (!check_step(enemy_motion, immobile, direction)) {
				illegal_directions[RIGHT] = true;
			}
			// up
			direction = { 0, -step_size };
			if (!check_step(enemy_motion, immobile, direction)) {
				illegal_directions[UP] = true;
			}
			// down
			direction = { 0, step_size };
			if (!check_step(enemy_motion, immobile, direction)) {
				illegal_directions[DOWN] = true;
			}
		}

		bool new_intersection = false;
		bool deadend = false;
		std::vector<Direction> possible_directions;
		for (int k = 0; k < illegal_directions.size(); k++) {
			bool direction = illegal_directions[k];
			if (!direction) {
				possible_directions.push_back(static_cast<Direction>(k));
				new_possible_directions.insert(static_cast<Direction>(k));
				//std::cout << "DIRECTION: " << k << " IN CUR ITERATION" << std::endl;
				if (enemy.last_possible_directions.find(static_cast<Direction>(k)) == enemy.last_possible_directions.end()) {
					new_intersection = true;
				}
				//std::cout << k << "DIRECTION IS LEGAL" << std::endl;
			}
			else {
				if (enemy.last_direction == static_cast<Direction>(k)) {
					deadend = true;
				}
			}
		}

		//std::cout << "--------------" << std::endl;

		Direction new_enemy_direction = enemy.last_direction;


		// if we reach a new interesection or we can no longer go the same direction we pick a new direction to go
		if (new_intersection || deadend) {
			if (distance < chase_distance) {
				//std::cout << "NEW INTERSECTION OR DEADEND" << std::endl;
				if (new_possible_directions.find(direction_to_player[0]) != new_possible_directions.end()) {
					//std::cout << "going " << direction_to_player[0]  << "direction" << std::endl;
					new_enemy_direction = direction_to_player[0];
				}
				else if (new_possible_directions.find(direction_to_player[1]) != new_possible_directions.end()) {
					//std::cout << "going " << direction_to_player[1] << "direction" << std::endl;
					new_enemy_direction = direction_to_player[1];
				}
				else {
					// get some randomness in keeper decision making
					std::shuffle(std::begin(possible_directions), std::end(possible_directions), rng);
					//std::cout << "GOING " << possible_directions[0] << "DIRECTION" << std::endl;
					if (possible_directions.size() == 0) {
						new_enemy_direction = direction_to_player[0];
					} else {
						new_enemy_direction = possible_directions[0];
					}
				}
			}
			else {
				// get some randomness in keeper decision making
				std::shuffle(std::begin(possible_directions), std::end(possible_directions), rng);
				//std::cout << "GOING " << possible_directions[0] << "DIRECTION" << std::endl;
				if (possible_directions.size() == 0) {
					new_enemy_direction = direction_to_player[0];
				} else {
					new_enemy_direction = possible_directions[0];
				}

			}
		}

		enemy.last_possible_directions = new_possible_directions;
		enemy.last_direction = new_enemy_direction;
		enemy_motion.velocity = direction_to_velocity[new_enemy_direction];
		enemy.frame_count = 0;


		// choose new direction for enemy
		
		//if (std::find(possible_directions.begin(), possible_directions.end(), direction_to_player[0]) != possible_directions.end()) {
		//	//std::cout << "GOING " << direction_to_player[0]  << "DIRECTION" << std::endl;
		//	new_enemy_direction = direction_to_velocity[direction_to_player[0]];
		//}
		//else if (std::find(possible_directions.begin(), possible_directions.end(), direction_to_player[1]) != possible_directions.end()) {
		//	//std::cout << "GOING " << direction_to_player[1] << "DIRECTION" << std::endl;
		//	new_enemy_direction = direction_to_velocity[direction_to_player[1]];
		//}
		//else {
		//	// get some randomness in keeper decision making
		//	std::shuffle(std::begin(possible_directions), std::end(possible_directions), rng);
		//	//std::cout << "GOING " << possible_directions[0] << "DIRECTION" << std::endl;
		//	new_enemy_direction = direction_to_velocity[possible_directions[0]];
		//}

		//std::cout << "------------" << std::endl;


	}

	//auto& enemy_registry = registry.enemies;
	//auto& player = registry.players.entities[0];
	//Motion player_position = registry.motions.get(player);
	//for (uint i = 0; i < enemy_registry.size(); i++) {
	//	// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
	//	Enemy& enemy = enemy_registry.components[i];
	//	Entity entity = enemy_registry.entities[i];
	//	Motion& enemy_motion = registry.motions.get(entity);
	//	float xDiff = player_position.position[0] - enemy_motion.position[0];
	//	float yDiff = player_position.position[1] - enemy_motion.position[1];
	//	float distance = std::sqrt(pow(xDiff, 2) + pow(yDiff, 2));

	//	float normalized_xDiff = (xDiff) / distance;
	//	float normalized_yDiff = (yDiff) / distance;

	//	enemy_motion.velocity = { normalized_xDiff * enemy_velocity_multipler, normalized_yDiff * enemy_velocity_multipler };
	//}
}



inline bool AISystem::check_step(const Motion& motion, const Immobile& immobile, const vec2& direction)
{
	Motion potential_motion = motion;
	potential_motion.position += direction;
	if (!collides(potential_motion, immobile)) {
		return true;
	}
	return false;
}
