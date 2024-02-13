#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createPlayer(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHICKEN);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ PLAYER_WIDTH, PLAYER_HEIGHT });
	

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.players.emplace(entity);
	// TEXTURE_COUNT indicates that no txture is needed
	registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::PLAYER_CHAR, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createPlant(RenderSystem* renderer, vec2 position, int type) {
	// TODO:
	// add to create function wich type (what tileset lookup an int) and what subset of tileset (an int)
	// subset will identify where to offest within tileset image

	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// TODO: remove position from motion (almost everything gets position but not motion)
	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ STD_PLANT_WIDTH, STD_PLANT_HEIGHT });

	//add to animation // choppable // etc registry.emplace(entity);
	if (type == 1) {
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	} else {
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}


	

	return entity;
}

Entity createMapChunk( vec2 position) {
	auto entity = Entity();

	auto& chunk = registry.chunks.emplace(entity);
	chunk.x = position.x;
	chunk.y = position.y;
	chunk.offset_x = position.x * MAZE_SIZE * TILE_SIZE;
	chunk.offset_y = position.y * MAZE_SIZE * TILE_SIZE;
	return entity;
}



//Entity createEagle(RenderSystem* renderer, vec2 position)
//{
//	auto entity = Entity();
//
//	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);
//
//	// Initialize the motion
//	auto& motion = registry.motions.emplace(entity);
//	motion.angle = 0.f;
//	motion.velocity = { 0, 100.f };
//	motion.position = position;
//
//	// Setting initial values, scale is negative to make it face the opposite way
//	motion.scale = vec2({ -EAGLE_BB_WIDTH, EAGLE_BB_HEIGHT });
//
//	// Create and (empty) Eagle component to be able to refer to all eagles
//	registry.deadlys.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::EAGLE,
//		 EFFECT_ASSET_ID::TEXTURED,
//		 GEOMETRY_BUFFER_ID::SPRITE });
//
//	return entity;
//}

Entity createLine(vec2 position, vec2 scale) {
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

//Entity createEgg(vec2 pos, vec2 size) {
//	auto entity = Entity();
//
//	// Setting initial motion values
//	Motion& motion = registry.motions.emplace(entity);
//	motion.position = pos;
//	motion.angle = 0.f;
//	motion.velocity = { 0.f, 0.f };
//	motion.scale = size;
//
//	// Create and (empty) Chicken component to be able to refer to all eagles
//	registry.deadlys.emplace(entity);
//	registry.renderRequests.insert(
//		entity,
//		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
//			EFFECT_ASSET_ID::EGG,
//			GEOMETRY_BUFFER_ID::EGG });
//
//	return entity;
//}


