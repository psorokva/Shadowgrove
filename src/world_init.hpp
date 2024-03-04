#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float STD_PLANT_WIDTH = 90.0f;
const float STD_PLANT_HEIGHT = 90.0f;
const float PLAYER_WIDTH = 40.0f;
const float PLAYER_HEIGHT = PLAYER_WIDTH * 2.0;
const float MUSHROOM_SIZE = 50.0f;

// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
Entity createKeeper(RenderSystem* renderer, vec2 pos);
Entity createCompass(vec2 position);
Entity createPlant(RenderSystem* renderer, vec2 pos, int biome_type, int plant_type);
Entity createLine(vec2 position, vec2 size);
//Entity createMapChunk(vec2 position);
Entity createPotion(RenderSystem* renderer, vec2 position);
Entity createInventoryPotion(Item& item, vec2 position);

Entity createLine(vec2 position, vec2 size);

void updateTile(RenderSystem* renderer, std::vector<std::vector<Tile>>& maze, int row, int col, int tile_type);



