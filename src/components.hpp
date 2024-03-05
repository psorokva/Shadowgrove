#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <set>

struct Chunk {
	int x = 0;
	int y = 0;
	float offset_x = 0;
	float offset_y = 0;
	float campos_x = 0;
	float campos_y = 0;
	int grid[MAZE_SIZE][MAZE_SIZE] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1},
		{0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1},
	};
};

struct SpriteSheetLoop {
	uint num_sprites = 1;
	uint sprite_w = 100;
	uint sprite_h = 100;
	uint current_sprite = 0;;
};

struct ChunkLocation {
	size_t index_x = 0;
	size_t index_y = 0;
	size_t offset_x = 0;
	size_t offset_y = 0;
};

// Tile component
struct Tile {
	
	// "path" is default type 0
	// "bush" / "forest" / "tree" will be 1 for now
	int tile_type;
	
	Tile(int type) : tile_type(type) {}
};

// Bushes/Trees/Forest are impenetrable by player
struct Impenetrable {

};

// Player component
struct Player {
	unsigned int health;
};

// Enemy component
struct Enemy {
	unsigned int attack;
	int frame_count = 0;
	std::set<Direction> last_possible_directions;
	Direction last_direction;
	Entity e_id;
};

enum Item {
	magical_apple,
	magical_strawberry,
	magical_grapes,
	magical_mushroom,
	magical_carrot,
	special_lotus,
	red_potion,
	green_potion,
	purple_potion,
	orange_potion,
	food,
	torch,
	nullval
};


// Healing Potions component
struct Healer {
	Item item_name;
};

// Escape Potions component
struct Escaper {

};

struct Deadly {

};

// Interactable component (trees & potions)
struct Interactable {

};


struct Inventory {
	unsigned int size = 0;
	std::vector<Item> holding;
	std::vector<unsigned int> crafted;
};


// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 camera_position = { 0, 0 };
};

struct Immobile {
	vec2 position = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 camera_position = { 0, 0 };
};

// Stucture to store collision information
struct Collision {
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

enum Game_screens
{
    game_screen_main_menu,
    game_screen_night,
    game_screen_day,
    game_screen_game_over,
};

// Sets the brightness of the screen
struct ScreenState {
	float darken_screen_factor = -1;
    Game_screens game_screen = game_screen_day;
};

struct HelpScreen {
    int help = 0;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent {
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying chicken
struct DeathTimer {
	float counter_ms = 3000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex {
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex {
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh {
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	PLAYER_CHAR1 = 0,
	PLAYER_CHAR2 = PLAYER_CHAR1 + 1,
	PLAYER_CHAR3 = PLAYER_CHAR2 + 1,
	PLAYER_CHAR4 = PLAYER_CHAR3 + 1,
	PLAYER_CHAR5 = PLAYER_CHAR4 + 1,
	KEEPER1 = PLAYER_CHAR5 + 1,
	KEEPER2 = KEEPER1 + 1,
	DOG = KEEPER2 + 1,
	INGR_00 = DOG + 1,
	INGR_01 = INGR_00 + 1,
	INGR_02 = INGR_01 + 1,
	INGR_03 = INGR_02 + 1,
	INGR_04 = INGR_03 + 1,
	INGR_05 = INGR_04 + 1,
	INGR_06 = INGR_05 + 1,
	INGR_07 = INGR_06 + 1,
	POTION_00 = INGR_07 + 1,
	POTION_01 = POTION_00 + 1,
	POTION_02 = POTION_01 + 1,
	POTION_03 = POTION_02 + 1,
	POTION_04 = POTION_03 + 1,
	POTION_05 = POTION_04 + 1,
	BUSH_00 = POTION_05 + 1,
	BUSH_01 = BUSH_00 + 1,
	BUSH_02 = BUSH_01 + 1,
	BUSH_03 = BUSH_02 + 1,
	BUSH_04 = BUSH_03 + 1,
	BUSH_05 = BUSH_04 + 1,
	BUSH_06 = BUSH_05 + 1,
	BUSH_07 = BUSH_06 + 1,
	BUSH_08 = BUSH_07 + 1,
	BUSH_09 = BUSH_08 + 1,
	BUSH_10 = BUSH_09 + 1,
	BUSH_11 = BUSH_10 + 1,
	BUSH_12 = BUSH_11 + 1,
	BUSH_13 = BUSH_12 + 1,
	BUSH_14 = BUSH_13 + 1,
	BUSH_15 = BUSH_14 + 1,
	BUSH_16 = BUSH_15 + 1,
	BUSH_17 = BUSH_16 + 1,
	BUSH_18 = BUSH_17 + 1,
	BUSH_19 = BUSH_18 + 1,
	BUSH_20 = BUSH_19 + 1,
	BUSH_21 = BUSH_20 + 1,
	BUSH_22 = BUSH_21 + 1,
	BUSH_23 = BUSH_22 + 1,
	BUSH_24 = BUSH_23 + 1,
	MUSH_00 = BUSH_24 + 1,
	MUSH_01 = MUSH_00 + 1,
	MUSH_02 = MUSH_01 + 1,
	MUSH_03 = MUSH_02 + 1,
	MUSH_04 = MUSH_03 + 1,
	MUSH_05 = MUSH_04 + 1,
	MUSH_06 = MUSH_05 + 1,
	MUSH_07 = MUSH_06 + 1,
	MUSH_08 = MUSH_07 + 1,
	MUSH_09 = MUSH_08 + 1,
	MUSH_10 = MUSH_09 + 1,
	MUSH_11 = MUSH_10 + 1,
	MUSH_12 = MUSH_11 + 1,
	PLANT_00 = MUSH_12 + 1,
	PLANT_01 = PLANT_00 + 1,
	PLANT_02 = PLANT_01 + 1,
	PLANT_03 = PLANT_02 + 1,
	PLANT_04 = PLANT_03 + 1,
	PLANT_05 = PLANT_04 + 1,
	PLANT_06 = PLANT_05 + 1,
	PLANT_07 = PLANT_06 + 1,
	PLANT_08 = PLANT_07 + 1,
	PLANT_09 = PLANT_08 + 1,
	PLANT_10 = PLANT_09 + 1,
	PLANT_11 = PLANT_10 + 1,
	PLANT_12 = PLANT_11 + 1,
	PLANT_13 = PLANT_12 + 1,
	PLANT_14 = PLANT_13 + 1,
	PLANT_15 = PLANT_14 + 1,
	PLANT_16 = PLANT_15 + 1,
	PLANT_17 = PLANT_16 + 1,
	DAY_SCREEN = PLANT_17 + 1,
    HELP_SCREEN = DAY_SCREEN +1,
	TEXTURE_COUNT = HELP_SCREEN + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	INVENTORY_LINE = COLOURED + 1,
	TEXTURED = INVENTORY_LINE + 1,
	NIGHT = TEXTURED + 1,
	EFFECT_COUNT = NIGHT + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	INVENTORY_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = INVENTORY_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

