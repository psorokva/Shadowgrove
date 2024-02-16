#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float STD_PLANT_WIDTH = 90.0f;
const float STD_PLANT_HEIGHT = 90.0f;
const float PLAYER_WIDTH = 80.0f;
const float PLAYER_HEIGHT = PLAYER_WIDTH * 2.0;

// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);

Entity createPlant(RenderSystem* renderer, vec2 pos, int type);
Entity createMapChunk(vec2 position);

void updateTile(RenderSystem* renderer, std::vector<std::vector<Tile>>& maze, int row, int col, int tile_type);



