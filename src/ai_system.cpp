// internal
#include "ai_system.hpp"

void AISystem::step() {
	auto& enemy_registry = registry.enemies;
	auto& player = registry.players.entities[0];
	Motion player_position = registry.motions.get(player);
	for (uint i = 0; i < enemy_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Enemy& enemy = enemy_registry.components[i];
		Entity entity = enemy_registry.entities[i];
		Motion& enemy_motion = registry.motions.get(entity);
		float xDiff = player_position.position[0] - enemy_motion.position[0];
		float yDiff = player_position.position[1] - enemy_motion.position[1];
		float distance = std::sqrt(pow(xDiff, 2) + pow(yDiff, 2));

		float normalized_xDiff = (xDiff) / distance;
		float normalized_yDiff = (yDiff) / distance;

		enemy_motion.velocity = { normalized_xDiff * enemy_velocity_multipler, normalized_yDiff * enemy_velocity_multipler };
	}
}