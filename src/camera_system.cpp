// internal
#include "camera_system.hpp"

void CameraSystem::step() {
	auto& motion_registry = registry.motions;
	auto& player = registry.players.entities[0];
	Motion& player_motion = motion_registry.get(player);

	for (uint i = 0; i < motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Entity entity = motion_registry.entities[i];
		if (player == entity) {
			continue;
		}
		Motion& motion = motion_registry.components[i];
		float xDiff = player_motion.position[0] - motion.position[0];
		float yDiff = player_motion.position[1] - motion.position[1];

		motion.camera_position = { player_motion.camera_position[0] - xDiff, player_motion.camera_position[1] - yDiff };
	}
}


vec2 CameraSystem::adjust_to_player(vec2 pos) {
	auto& motion_registry = registry.motions;
	auto& player = registry.players.entities[0];
	Motion& player_motion = motion_registry.get(player);

	float xDiff = player_motion.camera_position[0] - pos[0];
	float yDiff = player_motion.camera_position[1] - pos[1];

	return vec2{ player_motion.position[0] - xDiff, player_motion.position[1] - yDiff };

}