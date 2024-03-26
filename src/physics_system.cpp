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
bool collides(const Motion& motionA, const Impenetrable& motionB) {
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
	
	auto& motion_registry = registry.motions;
	//ComponentContainer<Motion>& motion_container = registry.motions;
	// Check for collisions between player and walls
	ComponentContainer<Immobile>& immobiles_container = registry.immobiles;
	Entity player = registry.players.entities[0];
	Motion& player_motion = motion_registry.components[0];
	//Motion& player_motion = motion_container.get(player);
	Motion prev_player_motion = player_motion;
	float step_seconds = elapsed_ms / 1000.f;

	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	
	for (uint i = 0; i < motion_registry.size(); i++) {
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		motion.position = motion.position + step_seconds * motion.velocity;
		/*printf("motion.velocity x %f/n", motion.velocity.x);
		printf("motion.velocity y %f/n", motion.velocity.y);*/
	}

	// Check for collision with walls
	auto& walls_container = registry.impenetrables;
	for (uint i = 0; i < walls_container.components.size(); i++) {
		Impenetrable& fixed_i = walls_container.components[i];
		Entity entity_i = walls_container.entities[i];
		// If not a screen
		if (!registry.screenStates.has(entity_i)) {
			if (collides(player_motion, fixed_i)) {
				player_motion = prev_player_motion;
				//printf("Colliding with wall\n");
				//Mix_PlayChannel(-1, tree_collision_sound, 0);
				//// Create a collisions event
				//registry.collisions.emplace_with_duplicates(player, entity_i);
			}
		}
		
	}

	// Check for collisions between player and other moving entities
	//Motion& motion_i = motion_container.get(player);
	//Entity entity_i = player;
	// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
	for(uint j = 1; j < motion_registry.components.size(); j++) {
		Entity entity_j = motion_registry.entities[j];
		Motion& motion_j = motion_registry.components[j];
		if (collides(player_motion, motion_j) && player != entity_j) {
			// Create a collisions event
			// We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
			registry.collisions.emplace_with_duplicates(player, entity_j);
			registry.collisions.emplace_with_duplicates(entity_j, player);
		}
	}
	


}

