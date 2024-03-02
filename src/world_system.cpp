// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

// iostream
#include <iostream>

#include "physics_system.hpp"

// Game configuration
//const int TILE_SIZE = 55;
//const int MAZE_SIZE = 12;
const int MAZE_WIDTH = MAZE_SIZE;
const int MAZE_HEIGHT = MAZE_SIZE;
std::vector<std::pair<int, int>> generated_maze_map;

bool blockedNorth = false;
bool blockedSouth = false;
bool blockedEast = false;
bool blockedWest = false;

enum Direction { UP, DOWN, LEFT, RIGHT};
Direction playerDirection;

const float CHARACTER_SPEED = 300.0f;

// evetually use pointers for this and add travesal between maps 
//const int maze_map[MAZE_SIZE][MAZE_SIZE] = {
//		{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1},
//		{1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 2, 1},
//		{1, 0, 0, 0, 1, 1, 0, 1, 2, 0, 2, 1},
//		{1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 2, 1},
//		{1, 0, 1, 2, 1, 1, 0, 0, 1, 1, 2, 1},
//		{1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 2, 1},
//		{1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1},
//		{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1},
//		{1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1},
//		{1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1},
//		{1, 0, 1, 0, 1, 1, 0, 2, 1, 0, 0, 1},
//		{1, 0, 1, 2, 1, 1, 0, 2, 1, 0, 0, 1},
//};


// Create the bug world
WorldSystem::WorldSystem() : points(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (night_music != nullptr)
	    Mix_FreeMusic(night_music);
	if (bird_sounds != nullptr)
	    Mix_FreeChunk(bird_sounds);
    if (mushroom_sound != nullptr)
        Mix_FreeChunk(mushroom_sound);
    if (player_death_sound != nullptr)
        Mix_FreeChunk(player_death_sound);
    if (inventory_full_sound != nullptr)
        Mix_FreeChunk(inventory_full_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Chicken Game Assignment", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_click_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };

	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_click_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

    night_music = Mix_LoadMUS(audio_path("forest_night.wav").c_str());
    bird_sounds = Mix_LoadWAV(audio_path("bird_noises.wav").c_str());
    mushroom_sound = Mix_LoadWAV(audio_path("mushroom.wav").c_str());
    player_death_sound = Mix_LoadWAV(audio_path("player_death.wav").c_str());
    inventory_full_sound = Mix_LoadWAV(audio_path("inventory_full.wav").c_str());

	if (night_music == nullptr || bird_sounds == nullptr || mushroom_sound == nullptr ||
        player_death_sound == nullptr || inventory_full_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
            audio_path("forest_night.wav").c_str(),
            audio_path("bird_noises.wav").c_str(),
            audio_path("mushroom.wav").c_str(),
            audio_path("player_death.wav").c_str(),
            audio_path("inventory_full.wav").c_str());

		return nullptr;
	}

	return window;
}

void renderInventory() {
    // top line
//    Entity topLine = createLine({800, 10}, {10, 10});
}

void WorldSystem::init(RenderSystem* renderer_arg, MapGen* generator_arg) {
	this->renderer = renderer_arg;
	this->generator = generator_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(night_music, -1);
	fprintf(stderr, "Loaded music\n");

    // Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	//for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	//    Motion& motion = motions_registry.components[i];
	//	if (motion.position.x + abs(motion.scale.x) < 0.f) {
	//		if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
	//			registry.remove_all_components_of(motions_registry.entities[i]);
	//	}
	//}

	// Processing the chicken state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    camera.step();
	ai.step();

    // load the inventory
//    renderInventory();
    Entity l1 = createLine({800, 10}, {10, 100});
    Entity l2 = createLine({100, 100}, {100, 10});

    Motion& m_l1 = registry.motions.get(l1);
    m_l1.camera_position = {100, 100};


    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Create a new chicken
	player = createPlayer(renderer, { window_width_px / 2, window_height_px - 200 });
	
	registry.colors.insert(player, { 1, 0.8f, 0.8f });

	createKeeper(renderer, { window_width_px / 2, window_height_px / 2 });



	//Chunk& chunk = registry.chunks.components[0];
		
	
	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create empty "path" maze
	std::vector<std::vector<Tile>> maze(MAZE_HEIGHT*3, std::vector<Tile>(MAZE_WIDTH*3, Tile(0)));
	//// ADD PLANTS HERE Note: ordering matter here unless we implement a z axis for layers 
	auto& chunks = generator->chunks;
	//for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
	//for (Chunk c : chunks) {
	for (uint i = 0; i < chunks.size(); i++) { //chunks.size()
		for (int row = 0; row < MAZE_SIZE; row++) {
			for (int col = 0; col < MAZE_SIZE; col++) {
				Chunk c = chunks[i];
				if ( c.grid[row][col] > 0) {
					int major_row = c.y * MAZE_SIZE + row;
					int major_col = c.x * MAZE_SIZE + col;
					int plant_x = c.offset_x + col * TILE_SIZE;
					int plant_y = c.offset_y + row * TILE_SIZE;
					// TODO store plant type in // c.grid[row][col]
					createPlant(renderer, { plant_x, plant_y }, generator->biome_filter[major_row][major_col], generator->bignoise[major_row][major_col]); // 
					//printf("tile: %d %d ", c.y * MAZE_SIZE + row, c.x * MAZE_SIZE + col);
					//updateTile(renderer, maze, c.offset_y + row, c.offset_x + col, 1);
					//updateTile(renderer, maze, c.y * MAZE_SIZE + row, c.x * MAZE_SIZE + col, c.grid[row][col]);
				}
				
			}
			//printf("\n");
		}
		//printf("--------\n");
	}


	

	// TODO Add function generateMaze
	
	/*for (int col = 0; col < MAZE_HEIGHT; col++) {
		for (int row = 0; row < MAZE_WIDTH; row++) {
			updateTile(renderer, maze, row, col, 1);
		}
	}*/

	//// Manually create maze (for now):
	//// Horizontal:
	//// Row 0
	//for (int x = 0; x < MAZE_WIDTH; x++) {
	//	updateTile(renderer, maze, x, 0, 1);
	//}
	//// Row 1
	//for (int x = 0; x < 5; x++) {
	//	updateTile(renderer, maze, x, 1, 1);
	//}
	//for (int x = 10; x < MAZE_WIDTH; x++) {
	//	updateTile(renderer, maze, x, 1, 1);
	//}

	//// Rows 2-4
	//for (int y = 2; y < 5; y++) {
	//	for (int x = 0; x < 2; x++) {
	//		updateTile(renderer, maze, x, y, 1);
	//	}
	//}
	//updateTile(renderer, maze, MAZE_WIDTH-1, 2, 1);

	//// Rows 5-7
	//for (int y = 5; y < 7; y++) {
	//	for (int x = 4; x < 8; x++) {
	//		updateTile(renderer, maze, x, y, 1);
	//	}
	//}
	//for (int x = 4; x < 7; x++) {
	//	updateTile(renderer, maze, x, 7, 1);
	//}
	//for (int y = 5; y < MAZE_HEIGHT; y++) {
	//	updateTile(renderer, maze, MAZE_WIDTH - 1, y, 1);
	//}

	//// Rows 8-12
	//for (int y = 8; y < MAZE_HEIGHT; y++) {
	//	for (int x = 0; x < 5; x++) {
	//		updateTile(renderer, maze, x, y, 1);
	//	}
	//}
	//for (int y = 9; y < MAZE_HEIGHT; y++) {
	//	for (int x = 9; x < MAZE_WIDTH; x++) {
	//		updateTile(renderer, maze, x, y, 1);
	//	}
	//}

	//// Create a new chicken
	//player = createPlayer(renderer, { window_width_px/2, window_height_px - 200 });
	//registry.colors.insert(player, {1, 0.8f, 0.8f});




	// A2: Enable static eggs on the ground, for reference
	// Create eggs on the floor, use this for reference
	/*
	for (uint i = 0; i < 20; i++) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float radius = 30 * (uniform_dist(rng) + 0.3f); // range 0.3 .. 1.3
		Entity egg = createEgg({ uniform_dist(rng) * w, h - uniform_dist(rng) * 20 },
			         { radius, radius });
		float brightness = uniform_dist(rng) * 0.5 + 0.5;
		registry.colors.insert(egg, { brightness, brightness, brightness});
	}
	*/
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the chicken
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			// Checking Player - Wall collisions
			if (registry.impenetrables.has(entity_other)) {
				
				Motion& motion = registry.motions.get(entity);
				// Can adjust this
				float offset = 10.0f;
                // Mix_PlayChannel(-1, tree_collision_sound, 0);
				
				if (playerDirection == UP) {
					//printf("Collided with North wall");
					blockedNorth = true;
					// Offset player position to stop from going through
					motion.position = motion.position - vec2(0, -offset);
				}
				if (playerDirection == DOWN) {
					//printf("Collided with South wall");
					blockedSouth = true;
					motion.position = motion.position - vec2(0, offset);
				}
				if (playerDirection == LEFT) {
					//printf("Collided with West wall");
					blockedWest = true;
					motion.position = motion.position - vec2(-offset, 0);
				}
				if (playerDirection == RIGHT) {
					//printf("Collided with East wall");
					blockedEast = true;
					motion.position = motion.position - vec2(offset, 0);
				}
			}

			// Checking Player - Deadly collisions
			if (registry.deadlys.has(entity_other)) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity)) {
					// Scream, reset timer, and make the chicken sink
					registry.deathTimers.emplace(entity);

					// so that player stops moving after collision with keeper
					registry.motions.get(player).velocity = { 0, 0 };
					Mix_PlayChannel(-1, player_death_sound, 0);
				}
			}
			// Checking Player - Eatable collisions
			else if (registry.healers.has(entity_other)) {
				if (!registry.deathTimers.has(entity)) {
					// chew, count points, and set the LightUp timer
					if (points < 5) {
						registry.remove_all_components_of(entity_other);
						Mix_PlayChannel(-1, mushroom_sound, 0);
						++points;
					} else {
                        Mix_PlayChannel(-1, inventory_full_sound, 0);
                    }
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
	// Reset collision flags
	blockedNorth = false;
	blockedSouth = false;
	blockedWest = false;
	blockedEast = false;
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		std::cout << "EXIT GAME" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Exiting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		printf("ESC key pressed, close window");
		glfwSetWindowShouldClose(window, true);
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	if (!registry.deathTimers.has(player)) {

		Motion& motion = registry.motions.get(player);

		// Character movement
		// Move Up
		if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
			playerDirection = UP;
			if (!blockedNorth) {
				motion.velocity = vec2(0.f, -CHARACTER_SPEED);
			}
			// TODO fix this if using angle
			/*if (motion.angle != 0.f)
			{
				motion.velocity = vec2(-CHARACTER_SPEED, -CHARACTER_SPEED);
				motion.velocity.x = motion.velocity.x * cos(motion.angle);
				motion.velocity.y = motion.velocity.y * sin(motion.angle);
			}*/
		}

		// Move Down
		if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
			playerDirection = DOWN;
			if (!blockedSouth) {
				motion.velocity = vec2(0.f, CHARACTER_SPEED);
			}
			
			/*if (motion.angle != 0.f)
			{
				motion.velocity = vec2(CHARACTER_SPEED, CHARACTER_SPEED);
				motion.velocity.x = motion.velocity.x * cos(motion.angle);
				motion.velocity.y = motion.velocity.y * sin(motion.angle);
			}*/
		}

		// Move Right
		if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
			playerDirection = RIGHT;
			if (!blockedEast) {
				motion.velocity = vec2(CHARACTER_SPEED, 0.f);
				motion.scale = vec2(-PLAYER_WIDTH, PLAYER_HEIGHT );
			}
			
			/*if (motion.angle != 0.f)
			{
				motion.velocity = vec2(CHARACTER_SPEED, CHARACTER_SPEED);
				motion.velocity.y = motion.velocity.y * cos(motion.angle);
				motion.velocity.x = motion.velocity.x * sin(motion.angle);
			}*/
		}

		// Move Left
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
			playerDirection = LEFT;
			if (!blockedWest) {
				motion.velocity = vec2(-CHARACTER_SPEED, 0.f);
				motion.scale = vec2(PLAYER_WIDTH, PLAYER_HEIGHT);
			}
			
			/*if (motion.angle != 0.f)
			{
				motion.velocity = vec2(-CHARACTER_SPEED, -CHARACTER_SPEED);
				motion.velocity.y = motion.velocity.y * cos(motion.angle);
				motion.velocity.x = motion.velocity.x * sin(motion.angle);
			}*/
		}

		// Stop moving when key is released
		if (action == GLFW_RELEASE) {
			motion.velocity = vec2(0.f, 0.f);
		}
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}

	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A1: HANDLE CHICKEN ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the chicken's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
	//std::cout << "x: " << mouse_position[0] << ", y: " << mouse_position[1] << std::endl;
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		if (points <= 5) {
			createPotion(renderer, camera.adjust_to_player(vec2{ xpos, ypos }));
		}
		
	}
	
	
}
