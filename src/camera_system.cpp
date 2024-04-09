// internal
#include "camera_system.hpp"

void CameraSystem::step() {
	auto& motion_registry = registry.motions;
	auto& player = registry.players.entities[0];
	Motion& player_motion = motion_registry.get(player);

	for (uint i = 0; i < motion_registry.size(); i++) {
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Entity entity = motion_registry.entities[i];
		if (player == entity) {
			continue;
		}
		Motion& motion = motion_registry.get(entity);
		float xDiff = player_motion.position[0] - motion.position[0];
		float yDiff = player_motion.position[1] - motion.position[1];

		motion.camera_position = { player_motion.camera_position[0] - xDiff, player_motion.camera_position[1] - yDiff };
		//if (registry.oscilations.has(entity)) {
		//	Oscilation& osc = registry.oscilations.get(entity);
		//	osc.current += osc.amount * osc.direction;
		//	if (osc.current > osc.max) {
		//		osc.direction *= -1.0;
		//		osc.current = osc.max;
		//	} else if (osc.current < osc.min) {
		//		osc.direction *= -1.0;
		//		osc.current = osc.min;
		//	}
		//	motion.scale = { osc.base_x - osc.current, osc.base + osc.current };
		//	
		//}
		//if (registry.dances.has(entity)) {
		//	Dance& dance = registry.dances.get(entity);
		//	dance.current += dance.amount * dance.direction;
		//	if (dance.current > dance.max) {
		//		dance.direction *= -1.0;
		//		dance.current = dance.max;
		//	} else if (dance.current < dance.min) {
		//		dance.direction *= -1.0;
		//		dance.current = dance.min;
		//	}
		//	motion.angle = dance.base + dance.current;

		//}

	}

	// For walls placement, might need to change
	auto& immobiles_registry = registry.immobiles;
	for (uint i = 0; i < immobiles_registry.size(); i++) {
		Entity entity = immobiles_registry.entities[i];
		Immobile& fixed = immobiles_registry.components[i];
		float xDiff = player_motion.position[0] - fixed.position[0];
		float yDiff = player_motion.position[1] - fixed.position[1];

		fixed.camera_position = { player_motion.camera_position[0] - xDiff, player_motion.camera_position[1] - yDiff };

		if (fixed.camera_position.x > (0 - on_screen_buffer)
			&& fixed.camera_position.x < (window_width_px + on_screen_buffer)
			&& fixed.camera_position.y > (0 - on_screen_buffer)
			&& fixed.camera_position.y < (window_height_px + on_screen_buffer))
		{
			fixed.on_screen = true;
			// oscilate if on screen
			if (registry.oscilations.has(entity)) {
				Oscilation& osc = registry.oscilations.get(entity);
				osc.current += osc.amount * osc.direction;
				if (osc.current > osc.max ) {
					osc.direction *= -1.0;
					osc.current = osc.max;
				} else if( osc.current < osc.min) {
					osc.direction *= -1.0;
					osc.current = osc.min;
				}
				fixed.scale.y = osc.base + osc.current;
				fixed.camera_position.y -= osc.current * 0.5;
			}

			if (registry.dances.has(entity)) {
				Dance& dance = registry.dances.get(entity);
				dance.current += dance.amount * dance.direction;
				if (dance.current > dance.max) {
					dance.direction *= -1.0;
					dance.current = dance.max;
				} else if (dance.current < dance.min) {
					dance.direction *= -1.0;
					dance.current = dance.min;
				}
				fixed.angle = dance.base + dance.current;
				
			}

		}
		else
		{
			fixed.on_screen = false;
		}

		

	}

	auto& chunks = registry.chunks;
	for (uint i = 0; i < chunks.size(); i++) {
		Entity entity = chunks.entities[i];
		Chunk& chunk = chunks.components[i];
		float xDiff = player_motion.position[0] - chunk.offset_x;
		float yDiff = player_motion.position[1] - chunk.offset_y;
		chunk.campos_x = player_motion.camera_position[0] - xDiff;
		chunk.campos_y =  player_motion.camera_position[1] - yDiff;
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