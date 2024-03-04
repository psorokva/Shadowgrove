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
		std::set<Direction> new_directions;

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
		std::vector<Direction> possible_directions;
		for (int k = 0; k < illegal_directions.size(); k++) {
			bool direction = illegal_directions[k];
			if (!direction) {
				possible_directions.push_back(static_cast<Direction>(k));
				new_directions.insert(static_cast<Direction>(k));
				//std::cout << k << "DIRECTION IS LEGAL" << std::endl;
			}
		}

		// choose new direction for enemy
		vec2 new_enemy_direction;
		if (std::find(possible_directions.begin(), possible_directions.end(), direction_to_player[0]) != possible_directions.end()) {
			//std::cout << "GOING " << direction_to_player[0]  << "DIRECTION" << std::endl;
			new_enemy_direction = direction_to_velocity[direction_to_player[0]];
		}
		else if (std::find(possible_directions.begin(), possible_directions.end(), direction_to_player[1]) != possible_directions.end()) {
			//std::cout << "GOING " << direction_to_player[1] << "DIRECTION" << std::endl;
			new_enemy_direction = direction_to_velocity[direction_to_player[1]];
		}
		else {
			// get some randomness in keeper decision making
			std::shuffle(std::begin(possible_directions), std::end(possible_directions), rng);
			//std::cout << "GOING " << possible_directions[0] << "DIRECTION" << std::endl;
			new_enemy_direction = direction_to_velocity[possible_directions[0]];
		}

		//std::cout << "------------" << std::endl;

		enemy_motion.velocity = new_enemy_direction;
		enemy.frame_count = 0;
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
