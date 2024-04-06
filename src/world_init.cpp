#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <random>

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
	registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::PLAYER_CHAR1, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createSpeechBubble(RenderSystem* renderer, vec2 pos, bool direction) {
	auto entity = Entity();
	UIComp& ui_comp = registry.uicomps.emplace(entity);
	ui_comp.position = pos;
	ui_comp.scale = vec2(540.0, 200.0 );
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUBBLE0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createPortrait(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	UIComp& ui_comp = registry.uicomps.emplace(entity);
	ui_comp.position = pos;
	ui_comp.scale = vec2(250.0, 300.0);
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PORTRAIT0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	AnimatedFaceOject& animation = registry.face_animation.emplace(entity);
	return entity;
}

Entity createDogPortrait(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	UIComp& ui_comp = registry.uicomps.emplace(entity);
	ui_comp.position = pos;
	ui_comp.scale = vec2(250.0, 300.0);
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::DOG_PORTRAIT0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	AnimatedDogFace& animation = registry.dog_face_animation.emplace(entity);
	return entity;
}

Entity createConversationPotion(RenderSystem* renderer, vec2 pos, int type) {
	// TYPE: 0 = GREEN , PURPLE = 1,  ORANGE = default
	auto entity = Entity();
    registry.permanents.emplace(entity);
	UIComp& ui_comp = registry.uicomps.emplace(entity);
	ui_comp.position = pos;
	ui_comp.scale = vec2(50, 50.0);
	if (type == 0) {
		// GREEN
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	} else if (type == 1) {
		// PURPLE
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	} else {
		// ORANGE
		registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}
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
	motion.scale = vec2({ KEEPER_WIDTH, KEEPER_HEIGHT });

	// Keeper is an enemy starting with 50 points of attack (adjust as needed)
	auto& enemy = registry.enemies.emplace(entity);

	// adjust these as needed for game balance
	unsigned int lower_bound_damage = 10;
	unsigned int upper_bound_damage = 30;
	enemy.attack = rand() % (upper_bound_damage - lower_bound_damage + 1) + lower_bound_damage;

	enemy.e_id = entity;
	registry.deadlys.emplace(entity); // Do we still need this?
	// TEXTURE_COUNT indicates that no texture is needed
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::KEEPER1, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createPlant(RenderSystem* renderer, vec2 position, int biome_type, int plant_type) {
	// TODO:
	// add to create function wich type (what tileset lookup an int) and what subset of tileset (an int)
	// subset will identify where to offest within tileset image

	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position and scale for non-moving entity
	auto& fixed = registry.immobiles.emplace(entity);
	fixed.position = position;
	fixed.scale = vec2({ STD_PLANT_WIDTH, STD_PLANT_HEIGHT });
	
	auto& wall = registry.impenetrables.emplace(entity);
	wall.position = position;
	wall.scale = fixed.scale;

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
	} else if (biome_type == 5) {
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
	} else if (biome_type == 6) {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_26, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_27, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_28, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_29, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_30, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_31, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_32, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_33, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_34, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_35, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else if (biome_type == 7) {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_12, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_13, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_14, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_15, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_16, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_17, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_18, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_19, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_20, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::MUSH_21, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else if (biome_type == 8) {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_27, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_28, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_29, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_30, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_31, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_22, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_21, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_20, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_19, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_18, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else if (biome_type == 9) {
		if (plant_type == 1) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_23, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 2) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_24, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 3) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_25, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 4) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::PLANT_26, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 5) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_35, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 6) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_36, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 7) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_37, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 8) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_38, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else if (plant_type == 9) {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_39, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		} else {
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BUSH_40, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
		}
	} else {
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
	motion.camera_position = position;

	registry.debugComponents.emplace(entity);
	
	return entity;
}

Entity createPotion(RenderSystem* renderer, vec2 position, std::vector<int> randomizer) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ MUSHROOM_SIZE, MUSHROOM_SIZE });

	registry.healers.emplace(entity);

	// TEXTURE_COUNT indicates that no txture is needed
	int randomizer_size = randomizer.size();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, randomizer_size - 1);

	int random_pick = distr(gen);
	switch (randomizer[random_pick]) {

		case (0):
			registry.healers.get(entity).item_name = magical_mushroom;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (1):
			registry.healers.get(entity).item_name = magical_shell;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (2):
			registry.healers.get(entity).item_name = magical_strawberry;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (3):
			registry.healers.get(entity).item_name = magical_grapes;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (4):
			registry.healers.get(entity).item_name = magical_apple;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (5):
			registry.healers.get(entity).item_name = red_lotus;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (6):
			registry.healers.get(entity).item_name = white_lotus;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_06, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (7):
			registry.healers.get(entity).item_name = magical_carrot;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_07, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case (8):
			registry.healers.get(entity).item_name = torch;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::TORCH_OFF0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			registry.torch_off_animation.emplace(entity);
			break;
	}
	Entity other = createSparkle(renderer, position);
	Paired& pair = registry.pairs.emplace(entity);
	pair.other = other;
	return entity;
}

Entity createSparkle(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos; // +vec2(0.0, -10.0);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ MUSHROOM_SIZE*1.3, MUSHROOM_SIZE * 1.3 });
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::SPARK0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	AnimatedSparkle& spark = registry.sparkle_animation.emplace(entity);
	return entity;
}

Entity createPlayerSparkle(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	UIComp& motion = registry.uicomps.emplace(entity);
	motion.position = pos;
	motion.scale = vec2({ PLAYER_HEIGHT *1.5, PLAYER_HEIGHT * 1.5 });
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::BSPARK0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	AnimatedPlayerSparkle& spark = registry.player_sparkle_animation.emplace(entity);
	return entity;
}

Entity createSunMoonSpinner(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	auto entityBG = Entity();
	UIComp& motion = registry.uicomps.emplace(entity);
	motion.position = pos;
	motion.scale = vec2({ PLAYER_HEIGHT * 2., PLAYER_HEIGHT * 2.0 });
	Rotator& rot = registry.rotators.emplace(entity);
	UIComp& uibg = registry.uicomps.emplace(entityBG);
	uibg.position = pos;
	uibg.scale = vec2({ PLAYER_HEIGHT * 2., PLAYER_HEIGHT * 2.0 });
	registry.renderRequests.insert(entityBG, { TEXTURE_ASSET_ID::DAYNIGHT0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::DAYNIGHT1, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

// just an invisible marker to calculate where player is in relation to
Entity createCompass(vec2 position) {
	auto entity = Entity();
	auto& fixed = registry.immobiles.emplace(entity);
	fixed.position.x = position.x;
	fixed.position.y = position.y;
	fixed.camera_position.x = position.x;
	fixed.camera_position.y = position.y;
	return entity;
}

Entity createCraftingUI(vec2 position) {
	auto entity = Entity();

    registry.daytimes.emplace(entity);
    registry.debugComponents.emplace(entity);
	Immobile& motion = registry.immobiles.emplace(entity);
    motion.position = position;
//    immobiles.angle = 0.f;
//	motion.velocity = { 0.f, 0.f };
    motion.scale = vec2({ 600, 600 });
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::CRAFTING, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createCheck(vec2 position) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.camera_position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ MUSHROOM_SIZE + 100, MUSHROOM_SIZE + 50});

	Immobile& fixed = registry.immobiles.emplace(entity);
	fixed.position = position;
	fixed.scale = vec2({ 3 * MUSHROOM_SIZE, 3 * MUSHROOM_SIZE });

	registry.debugComponents.emplace(entity);
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::CHECK, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createInventoryPotion(Item& item, vec2 position) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    // Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHICKEN);
    // registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.camera_position = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = vec2({ MUSHROOM_SIZE, MUSHROOM_SIZE });

    Immobile& fixed = registry.immobiles.emplace(entity);
    fixed.position = position;
    fixed.scale = vec2({ 3 * MUSHROOM_SIZE, 3 * MUSHROOM_SIZE });

    registry.debugComponents.emplace(entity);

    registry.healers.emplace(entity);
    // TEXTURE_COUNT indicates that no txture is needed

    switch (item) {
		case magical_mushroom:
			registry.healers.get(entity).item_name = magical_mushroom;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case magical_shell:
			registry.healers.get(entity).item_name = magical_shell;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case magical_strawberry:
			registry.healers.get(entity).item_name = magical_strawberry;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case magical_grapes:
			registry.healers.get(entity).item_name = magical_grapes;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case magical_apple:
			registry.healers.get(entity).item_name = magical_apple;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case red_lotus:
			registry.healers.get(entity).item_name = red_lotus;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case white_lotus:
			registry.healers.get(entity).item_name = white_lotus;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_06, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case magical_carrot:
			registry.healers.get(entity).item_name = magical_carrot;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INGR_07, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case red_potion:
			registry.healers.get(entity).item_name = red_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case blue_potion:
			registry.healers.get(entity).item_name = blue_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case green_potion:
			registry.healers.get(entity).item_name = green_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case purple_potion:
			registry.healers.get(entity).item_name = purple_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_03, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case orange_potion:
			registry.healers.get(entity).item_name = orange_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_04, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case yellow_potion:
			registry.healers.get(entity).item_name = yellow_potion;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::POTION_05, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case torch:
			registry.healers.get(entity).item_name = torch;
			registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::TORCH_OFF0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
			registry.torch_off_animation.emplace(entity);
			break; 
    }


    return entity;
}

Entity createDay() {
	auto entity = Entity();

	auto& day = registry.screenStates.emplace(entity);
	Daytime& sign = registry.daytimes.emplace(entity);

	auto& fixed = registry.immobiles.emplace(entity);
	//    fixed.position = { window_width_px / 2, 0.75 * window_height_px };
	fixed.position = { window_width_px / 2, 0.5 * window_height_px };
	fixed.scale = { window_width_px, window_height_px };
	//    fixed.camera_position = { window_width_px / 2, window_height_px / 2 };

		// TEXTURE_COUNT indicates that no txture is needed
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::DAY_SCREEN, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createIntroScreen() {
	auto entity = Entity();

	auto& day = registry.screenStates.emplace(entity);
	Daytime& sign = registry.daytimes.emplace(entity);

	auto& fixed = registry.immobiles.emplace(entity);
	//    fixed.position = { window_width_px / 2, 0.75 * window_height_px };
	fixed.position = { window_width_px / 2, 0.5 * window_height_px };
	fixed.scale = { window_width_px, window_height_px };
	//    fixed.camera_position = { window_width_px / 2, window_height_px / 2 };

		// TEXTURE_COUNT indicates that no txture is needed
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::INTRO_SCREEN, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnd() {
    auto entity = Entity();

    auto& day = registry.screenStates.emplace(entity);
    Daytime& sign = registry.daytimes.emplace(entity);

    auto& fixed = registry.immobiles.emplace(entity);
//    fixed.position = { window_width_px / 2, 0.75 * window_height_px };
    fixed.position = { window_width_px / 2, 0.5 * window_height_px };
    fixed.scale = { window_width_px, window_height_px };
//    fixed.camera_position = { window_width_px / 2, window_height_px / 2 };

    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::END_SCREEN, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createSign() {
    auto entity = Entity();

    auto& day = registry.screenStates.emplace(entity);

    auto& fixed = registry.immobiles.emplace(entity);
    fixed.position = { 0.15 * window_width_px, 0.85 * window_height_px };
    fixed.scale = { 300, 300 };

    Daytime& sign = registry.daytimes.emplace(entity);

    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::SIGN_01, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createSign2() {
    auto entity = Entity();

    auto& day = registry.screenStates.emplace(entity);

    auto& fixed = registry.immobiles.emplace(entity);
    fixed.position = { 0.35 * window_width_px, 0.20 * window_height_px };
    fixed.scale = { 150, 150 };

    Daytime& sign = registry.daytimes.emplace(entity);

    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::SIGN_02, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createNight() {
	auto entity = Entity();

	registry.nightTimer.emplace(entity);

	return entity;
}

Entity createHelp() {
    auto entity = Entity();

    HelpScreen& help = registry.helpScreen.emplace(entity);

    Motion& help_motion = registry.motions.emplace(entity);
//    help_motion.position = {2000, 2000};
    help_motion.scale = {0.5 * window_width_px, 0.5 * window_height_px};
    help_motion.camera_position = {window_width_px / 2 + 1.0, 0.5 * window_height_px};

    auto &fixed = registry.immobiles.emplace(entity);
    fixed.position = {window_width_px / 2 + 1.0, 0.5 * window_height_px};
    fixed.scale = {0.5 * window_width_px +2.0, 0.5 * window_height_px};
    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::HELP_SCREEN, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createCraftingList() {
    auto entity = Entity();

    HelpScreen& help = registry.helpScreen.emplace(entity);

    Motion& motion = registry.motions.emplace(entity);
    motion.scale = {0.5 * window_width_px, 0.5 * window_height_px};
    motion.camera_position = {window_width_px / 2 + 1.0, 0.5 * window_height_px};

    auto &fixed = registry.immobiles.emplace(entity);
    fixed.position = {window_width_px / 2 + 1.0, 0.5 * window_height_px};
    fixed.scale = {0.5 * window_width_px +2.0, 0.5 * window_height_px};
    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::CRAFTINGLIST, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createTutorial() {
	auto entity = Entity();

	Motion& tutorial_motion = registry.motions.emplace(entity);
	tutorial_motion.scale = { 0.75 * window_width_px, 0.75 * window_height_px };
	tutorial_motion.position = { window_width_px / 2, window_height_px / 2 };

	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::TUT1, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	TutorialSequence& tutorial = registry.tutorialSequence.emplace(entity);

	return entity;
}

Entity createShadowOverlay(RenderSystem* renderer) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.camera_position = vec2(window_width_px*0.5, window_height_px * 0.5);
	//motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ window_width_px, window_height_px });
	registry.debugComponents.emplace(entity);
	//registry.debugComponents.emplace(entity);
	registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::SHADOW_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createWinDance(RenderSystem* renderer, vec2 pos) {
    auto entity = Entity();
    Motion& immobile = registry.motions.emplace(entity);
    immobile.position = pos;
    immobile.scale = vec2({ 1.5 * PLAYER_WIDTH*4.0, 1.5 * PLAYER_HEIGHT *2.0 });
    immobile.camera_position = { 0.3 * window_width_px, 0.3 * window_height_px };

    registry.daytimes.emplace(entity);

    registry.renderRequests.insert(entity, { TEXTURE_ASSET_ID::WIN1, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}
Entity createDog(vec2 position) {
    auto entity = Entity();

    Immobile& immobile = registry.immobiles.emplace(entity);
    immobile.position = position;
    immobile.scale = vec2({ 1.6 * 135, 1.6 * 60 });
    immobile.camera_position = { window_width_px / 4, window_height_px / 4 };

    Daytime& dog = registry.daytimes.emplace(entity);

    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::DOG, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createLogo() {
    auto entity = Entity();

    auto& day = registry.screenStates.emplace(entity);

    auto& fixed = registry.immobiles.emplace(entity);
    fixed.position = { 0.5 * window_width_px, 0.4 * window_height_px };
    fixed.scale = { 1000, 300 };

    Daytime& logo = registry.daytimes.emplace(entity);

    // TEXTURE_COUNT indicates that no txture is needed
    registry.renderRequests.insert( entity,	{ TEXTURE_ASSET_ID::LOGO_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}