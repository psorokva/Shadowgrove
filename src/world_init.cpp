#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createPlayer(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHICKEN);
	// registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ PLAYER_WIDTH, PLAYER_HEIGHT });
	motion.camera_position = { window_width_px / 2, window_height_px / 2 };
	

	// Add player to players registry and set starting health = 100
	auto& player = registry.players.emplace(entity);
	player.health = 100;

	// TEXTURE_COUNT indicates that no txture is needed
	registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::PLAYER_CHAR, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createKeeper(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHICKEN);
	// registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ PLAYER_WIDTH, PLAYER_HEIGHT });


	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.enemies.emplace(entity);
	registry.deadlys.emplace(entity);
	// TEXTURE_COUNT indicates that no txture is needed
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::KEEPER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createPlant(RenderSystem* renderer, vec2 position, int biome_type, int plant_type) {
	// TODO:
	// add to create function wich type (what tileset lookup an int) and what subset of tileset (an int)
	// subset will identify where to offest within tileset image

	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	//auto& motion = registry.motions.emplace(entity);
	//motion.angle = 0.f;
	//motion.velocity = { 0, 0 };
	//motion.position = position;
	//motion.scale = vec2({ STD_PLANT_WIDTH, STD_PLANT_HEIGHT });

	// Initialize the position and scale for non-moving entity
	auto& fixed = registry.immobiles.emplace(entity);
	fixed.position = position;
	fixed.scale = vec2({ STD_PLANT_WIDTH, STD_PLANT_HEIGHT });
	
	registry.impenetrables.emplace(entity);

	//add to animation // choppable // etc registry.emplace(entity);
	if (biome_type == 1) {
		// BASIC TYPE 1 BUSH
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_18, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_19, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_20, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_21, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else if (biome_type == 2) {
		// TYPE 2 BUSH
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_12, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_13, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_14, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_15, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_16, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_17, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_22, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_23, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_24, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}

	} else if (biome_type == 3) {
		// MUSHROOMS
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_06, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_09, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_10, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_08, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else if (biome_type == 4) {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_08, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_09, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_10, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_11, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_12, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_13, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_14, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_15, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_16, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_17, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_06, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_07, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_08, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_09, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	}



	return entity;
}

//Entity createMapChunk( vec2 position) {
//	auto entity = Entity();
//
//	auto& chunk = registry.chunks.emplace(entity);
//	chunk.x = position.x;
//	chunk.y = position.y;
//	chunk.offset_x = position.x * MAZE_SIZE * TILE_SIZE;
//	chunk.offset_y = position.y * MAZE_SIZE * TILE_SIZE;
//	return entity;
//}


// Not used currently
void updateTile(RenderSystem* renderer, std::vector<std::vector<Tile>>& maze, int row, int col, int type) {
	maze[row][col].tile_type = type;

	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// TODO: remove position from motion (almost everything gets position but not motion)
	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = {row * 55, col * 55};

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ STD_PLANT_WIDTH, STD_PLANT_HEIGHT });

	if (type == 1) {
		registry.impenetrables.emplace(entity);
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}
	// TODO: add other variations
	/*else {
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}*/

}

Entity createLine(vec2 position, vec2 scale) {
	Entity entity = Entity();

	// Store a reference to inventory line
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::INVENTORY_LINE,
		 GEOMETRY_BUFFER_ID::INVENTORY_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	
	return entity;
}

Entity createPotion(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHICKEN);
	// registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	//Motion& motion = registry.motions.emplace(entity);
	//motion.position = position;
	//motion.angle = 0.f;
	//motion.velocity = { 0.f, 0.f };
	//motion.scale = vec2({ MUSHROOM_SIZE, MUSHROOM_SIZE });

	// Setting fixed values
	auto& fixed = registry.immobiles.emplace(entity);
	fixed.position = position;
	fixed.scale = vec2({ MUSHROOM_SIZE, MUSHROOM_SIZE });

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.healers.emplace(entity);
	// TEXTURE_COUNT indicates that no txture is needed
	int potion_colour_offset = rand() % 11;
	switch (potion_colour_offset) {
		case (0):
			registry.healers.get(entity).item_name = magical_mushroom;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (1):
			registry.healers.get(entity).item_name = magical_strawberry;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (2):
			registry.healers.get(entity).item_name = magical_grapes;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (3):
			registry.healers.get(entity).item_name = red_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (4):
			registry.healers.get(entity).item_name = green_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (5):
			registry.healers.get(entity).item_name = purple_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (6):
			registry.healers.get(entity).item_name = orange_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (7):
			registry.healers.get(entity).item_name = magical_apple;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (8):
			registry.healers.get(entity).item_name = magical_carrot;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_07, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (9):
			registry.healers.get(entity).item_name = food;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (10):
			registry.healers.get(entity).item_name = special_lotus;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
	}
	

	return entity;
}

