#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
//const float BUG_BB_WIDTH = 0.6f * 165.f;
//const float BUG_BB_HEIGHT = 0.6f * 165.f;
//const float EAGLE_BB_WIDTH = 0.6f * 300.f;
//const float EAGLE_BB_HEIGHT = 0.6f * 202.f;
const float STD_PLANT_WIDTH = 90.0f;
const float STD_PLANT_HEIGHT = 90.0f;
const float PLAYER_WIDTH = 120.0f;
const float PLAYER_HEIGHT = 120.0f;

// the player
Entity createChicken(RenderSystem* renderer, vec2 pos);

Entity createPlant(RenderSystem* renderer, vec2 pos, int type);
Entity createMapChunk(vec2 position);
// the prey
//Entity createBug(RenderSystem* renderer, vec2 position);
//// the enemy
//Entity createEagle(RenderSystem* renderer, vec2 position);
//// a red line for debugging purposes
//Entity createLine(vec2 position, vec2 size);
//// a egg
//Entity createEgg(vec2 pos, vec2 size);


