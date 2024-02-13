// internal
#include "physics_system.hpp"
#include "world_init.hpp"


// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion) {
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

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

//returns chunk and place within chunk
ChunkLocation* getPlayerTiles(const Motion& m) {
	ChunkLocation *cl = new ChunkLocation();
	cl->offset_x = ((size_t)((m.position.x + PLAYER_WIDTH*0.5) / TILE_SIZE) % MAZE_SIZE);
	cl->offset_y = ((size_t)((m.position.y + PLAYER_WIDTH)/ TILE_SIZE) % MAZE_SIZE);
	cl->index_x = ((size_t)((m.position.x + PLAYER_WIDTH * 0.5)/ TILE_SIZE) / MAZE_SIZE);
	cl->index_y = ((size_t)((m.position.y + PLAYER_WIDTH)/ TILE_SIZE) / MAZE_SIZE);
	return cl;
}


void PhysicsSystem::step(float elapsed_ms) {
	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++) {
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Motion future = Motion();
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		// TODO: check if collide with map
		// registry.chunks
		ChunkLocation *current_pos = getPlayerTiles(motion);
		future.position = motion.position + step_seconds * motion.velocity;
		ChunkLocation *future_pos = getPlayerTiles(future);

		for (Chunk& chunk : registry.chunks.components) {
			//if (DEBUG_PRINT) {
			//	//printf("mp %f , %f \n", motion.position[0], motion.position[1]);
			//	//printf("vec4 offset: x %d ,y %d , index: x %d ,y %d \n", current_pos->offset_x, current_pos->offset_y, current_pos->index_x, current_pos->index_y);
			//	//printf("fp %f , %f \n", future.position[0], future.position[1]);
			//	//printf("vec4 offset: x %d ,y %d , index: x %d ,y %d \n", future_pos->offset_x, future_pos->offset_y, future_pos->index_x, future_pos->index_y);
			//}

			// if relevant chunk is there
			if (chunk.x == future_pos->index_x && chunk.y == future_pos->index_y) {
				// check if future position is legal
				if (chunk.grid[future_pos->offset_y][future_pos->offset_x] == 0 ) {
					motion.position = motion.position + step_seconds * motion.velocity;
					//printf("vec4 offset: x %u ,y %u , index: x %u ,y %u \n", future_pos->offset_x, future_pos->offset_y, future_pos->index_x, future_pos->index_y);
				} // else don't update also add checks for previous location after
				else {
					// TODO: keep motion going closer if we could get a little closer
					//printf("Bounds collision in  chunk %f , %f\n", future_pos[2], future_pos[3]);
				}
			} else {
				// We're not in a chunk?!
				//printf("Not at chunk %f , %f\n", future_pos[2], future_pos[3]);
			}
		}
		//DEBUG_PRINT = false;
		delete current_pos;
		delete future_pos;
		// TODO: can still move a bit towards the non passable tile ?? 
		motion.position = motion.position + step_seconds * motion.velocity;
		//motion.position = motion.position + step_seconds * motion.velocity;


	}
	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++) {
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++) {
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j)) {
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

