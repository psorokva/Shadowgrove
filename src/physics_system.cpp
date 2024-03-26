// internal
#include "physics_system.hpp"
#include "world_init.hpp"


// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion) {
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// Duplicate for collision with walls, might need to change
vec2 get_bounding_box(const Immobile& immobile) {
	// abs is to avoid negative scale due to the facing direction.
	return { abs(immobile.scale.x), abs(immobile.scale.y) };
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

vec2 get_new_position(const Motion& motionA, const Immobile& motionB) {

	vec2 result;
	vec2 abs_scaleA = abs(motionA.scale);
	vec2 abs_scaleB = abs(motionB.scale);
	float randomNumber = 30.f;

	if (motionA.position.x < motionB.position.x) { // coming from left
		//float valX1 = 42.5f;//((abs_scaleB.x / 2));
		//printf("Change position to lefter by %f\n", valX1);
		//result.x = motionB.position.x - valX1;
		result.x = motionB.position.x - ((abs_scaleB.x / 2) + (abs_scaleA.x / 2));
		//result.x = motionB.position.x - (abs_scaleB.x / 2);
		//result.x = motionB.position.x - abs_scaleB.x;
		//result.x = motionB.position.x - randomNumber;
	}
	else if (motionA.position.x >= motionB.position.x) { // coming from right
		//float valX2 = 44.f;//((abs_scaleB.x / 2));
		//printf("Change position to righter by %f\n", valX2);
		//result.x = motionB.position.x + valX2;
		result.x = motionB.position.x + ((abs_scaleB.x / 2) + (abs_scaleA.x / 2)) + 25.f;
		//result.x = motionB.position.x + (abs_scaleB.x / 2);
		//result.x = motionB.position.x + abs_scaleB.x;
		//result.x = motionB.position.x + randomNumber + 6.f;
	}

	if (motionA.position.y < motionB.position.y) { // coming from top
		//float valY1 = 42.f;//((abs_scaleB.y / 2)); //(abs_scaleA.y / 25)
		//printf("Change position to higher by %f\n", valY1);
		//result.y = motionB.position.y - valY1;
		result.y = motionB.position.y - ((abs_scaleB.y / 2) + (abs_scaleA.y / 2));
		//result.y = motionB.position.y - (abs_scaleB.y / 2);
		//result.y = motionB.position.y - abs_scaleB.y;
		//result.y = motionB.position.y - randomNumber;
	}
	else if (motionA.position.y >= motionB.position.y) { // coming from bottom
		//float valY2 = 43.f;//((abs_scaleB.y / 2));
		//printf("Change position to lower by %f\n", valY2);
		//result.y = motionB.position.y + valY2;
		result.y = motionB.position.y + ((abs_scaleB.y / 2) + (abs_scaleA.y / 2));
		//result.y = motionB.position.y + (abs_scaleB.y / 2);
		//result.y = motionB.position.y + abs_scaleB.y;
		//result.y = motionB.position.y + randomNumber;
	}

	// if both x & y intersecting, chose the one with least displacement
	float diff = abs(motionA.position.x - result.x) - abs(motionA.position.y - result.y);

	//if (abs(diff) <= 5.0f) { // if intersecting in corner
	//	return result;
	//}
	if (diff < 0.f) { // if negative
		result.y = motionA.position.y;
	}
	else {
		result.x = motionA.position.x;
	}

	return result;
}

void PhysicsSystem::step(float elapsed_ms) {
	//printf("HELLO physics \n");
	ComponentContainer<Motion>& motion_container = registry.motions;
		//// Check for collisions between player and walls
	ComponentContainer<Immobile>& immobiles_container = registry.immobiles;
	Entity player = registry.players.entities[0];
	Motion& player_motion = motion_container.components[0];
	Motion prev_player_motion = player_motion;
	float step_seconds = elapsed_ms / 1000.f;




	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++) {
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		
		Entity entity = motion_registry.entities[i];
		//float step_seconds = elapsed_ms / 1000.f;


		////// TODO: check if collide with map
		//Motion future = Motion();
		//Motion future_w = Motion();
		////registry.chunks
		//ChunkLocation* current_pos = getPlayerTiles(motion,compass_immobile);
		//future.position = motion.position + step_seconds * motion.velocity + vec2(0.0, -PLAYER_WIDTH*0.25);;
		////future_w.position = motion.position + step_seconds * motion.velocity + vec2(PLAYER_WIDTH, 0.0);
		//ChunkLocation* future_pos = getPlayerTiles(future, compass_immobile);
		//ChunkLocation* future_w_pos = getPlayerTiles(future_w, compass_immobile);

		//for (Chunk& chunk : registry.chunks.components) {
		//	//printf(".");
		//	//if (DEBUG_PRINT) {
		//	//	printf("mp %f , %f \n", motion.position[0], motion.position[1]);
		//	//	printf("vec4 offset: x %d ,y %d , index: x %d ,y %d \n", current_pos->offset_x, current_pos->offset_y, current_pos->index_x, current_pos->index_y);
		//	//	printf("fp %f , %f \n", future.position[0], future.position[1]);
		//	//	printf("vec4 offset: x %d ,y %d , index: x %d ,y %d \n", future_pos->offset_x, future_pos->offset_y, future_pos->index_x, future_pos->index_y);
		//	//}
		//	// if relevant chunk is there
		//	if (chunk.x == future_pos->index_x && chunk.y == future_pos->index_y ) { // && chunk.x == future_w_pos->index_x && chunk.y == future_w_pos->index_y) {
		//		// check if future position is legal
		//		if (chunk.grid[future_pos->offset_y][future_pos->offset_x] == 0 ){ // && chunk.grid[future_w_pos->offset_y][future_w_pos->offset_x] == 0) {
		//			//motion.position = motion.position + step_seconds * motion.velocity;
		//			//printf("vec4 offset: x %u ,y %u , index: x %u ,y %u \n", future_pos->offset_x, future_pos->offset_y, future_pos->index_x, future_pos->index_y);
		//		} // else don't update also add checks for previous location after
		//		else {
		//			motion.velocity = vec2(0.0, 0.0);
		//			// TODO: keep motion going closer if we could get a little closer
		//			//printf("Bounds collision in  chunk %f , %f\n", future_pos[2], future_pos[3]);
		//		}
		//	} else {
		//		// We're not in a chunk?!
		//		//printf("Not at chunk %f , %f\n", future_pos[2], future_pos[3]);
		//	}
		//}
		////DEBUG_PRINT = false;
		//delete current_pos;
		//delete future_pos;

		// TODO: can still move a bit towards the non passable tile ?? 
		//
		motion.position = motion.position + step_seconds * motion.velocity;
		/*printf("regular position. x: %f", player_motion.position.x);
		printf("y: %f\n", player_motion.position.y);*/

		//Bounds checking for player
		//if (registry.players.has(entity)) {
		//	motion.position[0] = std::max(motion.position[0], PLAYER_WIDTH / 2);
		//	motion.position[0] = std::min(motion.position[0], window_width_px - PLAYER_WIDTH / 2);
		//	motion.position[1] = std::max(motion.position[1], PLAYER_HEIGHT / 2);
		//	motion.position[1] = std::min(motion.position[1], window_height_px - PLAYER_HEIGHT / 2);
		//}

	}

	for (uint i = 0; i < immobiles_container.components.size(); i++) {
		Immobile& fixed_i = immobiles_container.components[i];
		Entity entity_i = immobiles_container.entities[i];
		if (collides(player_motion, fixed_i)) {
			player_motion = prev_player_motion;
			//// Create a collisions event
			//registry.collisions.emplace_with_duplicates(player, entity_i);

			////player_motion.position = get_new_position(player_motion, fixed_i) - player_motion.velocity * step_seconds;
			//printf("colliding");
			//printf("position before. x: %f", player_motion.position.x);
			//printf("y: %f\n", player_motion.position.y);
			////player_motion.camera_position = get_new_position(player_motion, fixed_i) + step_seconds * player_motion.velocity;
			//player_motion.position = get_new_position(player_motion, fixed_i);
			//player_motion.velocity = vec2(0.f, 0.f);
			//printf("position after. x: %f", player_motion.position.x);
			//printf("y: %f\n", player_motion.position.y);
			////player_motion.velocity *= -1.0f;
			////player_motion.position -= elapsed_ms / 1000.0f * player_motion.velocity *1.1f;
		}
	}

	//for (uint i = 0; i < immobiles_container.components.size(); i++) {
	//	Immobile& fixed_i = immobiles_container.components[i];
	//	Entity entity_i = immobiles_container.entities[i];
	//	if (collides(player_motion, fixed_i)) {
	//		// Create a collisions event
	//		//registry.collisions.emplace_with_duplicates(player, entity_i);

	//		//player_motion.position = get_new_position(player_motion, fixed_i) - player_motion.velocity * step_seconds;
	//		printf("colliding");
	//		printf("position before. x: %f", player_motion.position.x);
	//		printf("y: %f\n", player_motion.position.y);
	//		player_motion.position = get_new_position(player_motion, fixed_i);
	//		player_motion.velocity = vec2(0.f, 0.f);
	//		printf("position after. x: %f", player_motion.position.x);
	//		printf("y: %f\n", player_motion.position.y);

	//	}
	//}

	// Check for collisions between all moving entities
   
	//for(uint i = 0; i<motion_container.components.size(); i++) {
	Motion& motion_i = motion_container.get(player);
	Entity entity_i = player;
		
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

