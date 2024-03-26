// internal
#include "physics_system.hpp"
#include "world_init.hpp"


// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion) {
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

//// Duplicate for collision with walls, might need to change
//vec2 get_bounding_box(const Immobile& immobile) {
//	// abs is to avoid negative scale due to the facing direction.
//	return { abs(immobile.scale.x), abs(immobile.scale.y) };
//}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2) {
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared) {
		return true;
	}
	return false;
}

// Collision with walls (AABB)
bool collides(const Motion& motionA, const Immobile& motionB) {
	return motionA.position.x < motionB.position.x + abs(motionB.scale.x) / 2//might need to divide scales by 2
		&& motionA.position.x + abs(motionA.scale.x) / 2 > motionB.position.x
		&& motionA.position.y < motionB.position.y + abs(motionB.scale.y) / 2
		&& motionA.position.y + abs(motionA.scale.y) / 2 > motionB.position.y;
	//vec2 dp = motion1.position - immobile.position;
	//float dist_squared = dot(dp, dp);
	//const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	//const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	//const vec2 my_bonding_box = get_bounding_box(immobile) / 2.f;
	//const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	//const float r_squared = max(other_r_squared, my_r_squared);
	//if (dist_squared < r_squared) {
	//	return true;
	//}
	//return false;
}

//returns chunk and place within chunk
ChunkLocation* getPlayerTiles(const Motion& m, const Immobile* compass) {
	ChunkLocation *cl = new ChunkLocation();
	cl->offset_x = ((size_t)((m.position.x - compass->position.x + PLAYER_WIDTH*0.5) / TILE_SIZE) % MAZE_SIZE);
	cl->offset_y = ((size_t)((m.position.y - compass->position.y + PLAYER_WIDTH)/ TILE_SIZE) % MAZE_SIZE);
	cl->index_x = ((size_t)((m.position.x - compass->position.x + PLAYER_WIDTH * 0.5)/ TILE_SIZE) / MAZE_SIZE);
	cl->index_y = ((size_t)((m.position.y - compass->position.y + PLAYER_WIDTH)/ TILE_SIZE) / MAZE_SIZE);
	return cl;
}

void PhysicsSystem::setCompass(Entity* entity, Immobile* immobile) {
	compass_entity = entity;
	compass_immobile = immobile;
}

void PhysicsSystem::step(float elapsed_ms) {
	
	ComponentContainer<Motion>& motion_container = registry.motions;
	// Check for collisions between player and walls
	ComponentContainer<Immobile>& immobiles_container = registry.immobiles;
	Entity player = registry.players.entities[0];
	Motion& player_motion = motion_container.components[0];
	Motion prev_player_motion = player_motion;
	float step_seconds = elapsed_ms / 1000.f;

	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++) {
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		motion.position = motion.position + step_seconds * motion.velocity;
	}

	for (uint i = 0; i < immobiles_container.components.size(); i++) {
		Immobile& fixed_i = immobiles_container.components[i];
		Entity entity_i = immobiles_container.entities[i];
		if (collides(player_motion, fixed_i)) {
			player_motion = prev_player_motion;
			//Mix_PlayChannel(-1, tree_collision_sound, 0);
			//// Create a collisions event
			//registry.collisions.emplace_with_duplicates(player, entity_i);
		}
	}

	// Check for collisions between all moving entities
	for(uint i = 0; i<motion_container.components.size(); i++) {
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
	// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
	for(uint j = 0; j < motion_container.components.size(); j++) {
		Entity entity_j = motion_container.entities[j];
		Motion& motion_j = motion_container.components[j];
		if (collides(motion_i, motion_j) && entity_i != entity_j) {
			// Create a collisions event
			// We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
			registry.collisions.emplace_with_duplicates(entity_i, entity_j);
			registry.collisions.emplace_with_duplicates(entity_j, entity_i);
		}
	}
	//}


}

