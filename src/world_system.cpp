// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

// iostream
#include <iostream>
#include <vector>
#include "physics_system.hpp"

// Game configuration
const int MAZE_WIDTH = MAZE_SIZE;
const int MAZE_HEIGHT = MAZE_SIZE;

bool blockedNorth = false;
bool blockedSouth = false;
bool blockedEast = false;
bool blockedWest = false;

Direction playerDirection;
const uint NUM_POTIONS = 100;
const float CHARACTER_SPEED = 300.0f;
const float ANIMATION_SPEED = 1000.0 / 16.0;
float keeper_rate = 10.0;
float KEEPER_SPAWN_FREQ = keeper_rate * 1000.0;
float keeper_spawn_time = 0.0f;
int num_of_item_on_screen = 0;
uint animation_cycle = 0;
float animation_elpased_ms = 0.0f;
int level = 0;
const float TORCH_LENGTH = 25 * 1000.0;
float torch_timer_ms = TORCH_LENGTH;

bool game_paused = false;

// Create the bug world
WorldSystem::WorldSystem() : points(0), chunk_border(false) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (night_music != nullptr)
	    Mix_FreeMusic(night_music);
    if (day_music != nullptr)
        Mix_FreeMusic(day_music);
	if (bird_sounds != nullptr)
	    Mix_FreeChunk(bird_sounds);
    if (mushroom_sound != nullptr)
        Mix_FreeChunk(mushroom_sound);
    if (player_death_sound != nullptr)
        Mix_FreeChunk(player_death_sound);
    if (inventory_full_sound != nullptr)
        Mix_FreeChunk(inventory_full_sound);
	if (player_hit_sound != nullptr)
		Mix_FreeChunk(player_hit_sound);
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
    day_music = Mix_LoadMUS(audio_path("day.wav").c_str());
    bird_sounds = Mix_LoadWAV(audio_path("bird_noises.wav").c_str());
    mushroom_sound = Mix_LoadWAV(audio_path("mushroom.wav").c_str());
    player_death_sound = Mix_LoadWAV(audio_path("player_death.wav").c_str());
    inventory_full_sound = Mix_LoadWAV(audio_path("inventory_full.wav").c_str());
	player_hit_sound = Mix_LoadWAV(audio_path("player_hit.wav").c_str());

	if (night_music == nullptr || day_music == nullptr || bird_sounds == nullptr || mushroom_sound == nullptr ||
        player_death_sound == nullptr || inventory_full_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s \n make sure the data directory is present",
            audio_path("forest_night.wav").c_str(),
            audio_path("day.wav").c_str(),
            audio_path("bird_noises.wav").c_str(),
            audio_path("mushroom.wav").c_str(),
            audio_path("player_death.wav").c_str(),
            audio_path("inventory_full.wav").c_str(),
			audio_path("player_hit.wav").c_str());

		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg, MapGen* generator_arg, PhysicsSystem* physics_arg) {
	this->renderer = renderer_arg;
	this->generator = generator_arg;
	this->physics = physics_arg;
//	// Playing background music indefinitely
//	Mix_PlayMusic(night_music, -1);
//	fprintf(stderr, "Loaded music\n");

    // Set all states to default
    restart_game();
}

void WorldSystem::createInventory(vec2 pos) {
	Entity backgroundLine = createLine({ pos.x, pos.y }, { 308, 70 });
    Entity topLine = createLine({pos.x, pos.y - 35}, {308, 8});
    Entity bottomLine = createLine({pos.x, pos.y + 35}, {308, 8});
    Entity sideLine1 = createLine({pos.x - 150, pos.y}, {8, 70});
    Entity sideLine2 = createLine({pos.x - 90, pos.y}, {8, 70});
    Entity sideLine3 = createLine({pos.x - 30, pos.y}, {8, 70});
    Entity sideLine4 = createLine({pos.x + 30, pos.y}, {8, 70});
    Entity sideLine5 = createLine({pos.x + 90, pos.y}, {8, 70});
    Entity sideLine6 = createLine({pos.x + 150, pos.y}, {8, 70});

    vec3 green = {0.07, 0.5, 0.05};
	vec3 bg_colour = { 0.1, 0.3, 0.1 };
	registry.colors.insert(backgroundLine, bg_colour);
    registry.colors.insert(topLine, green);
    registry.colors.insert(bottomLine, green);
    registry.colors.insert(sideLine1, green);
    registry.colors.insert(sideLine2, green);
    registry.colors.insert(sideLine3, green);
    registry.colors.insert(sideLine4, green);
    registry.colors.insert(sideLine5, green);
    registry.colors.insert(sideLine6, green);

}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {



	// tabbing out of the game or minimizing the window will pause the game
	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
	if (!focused) pause_game();

	// Don't do anything if game is paused
	if (game_paused) return true;

	//	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];
    Player& player_component = registry.players.components[0];

    // dont do anything if player is dead
    if (player_component.alive == 0) return true;

    if (registry.nightTimer.size() > 0) {
        float min_night_counter_ms = 60000.f;
        NightTimer& night_counter = registry.nightTimer.components[0];
        // progress timer
        night_counter.counter_ms -= elapsed_ms_since_last_update;
        if (night_counter.counter_ms < min_night_counter_ms) {
            min_night_counter_ms = night_counter.counter_ms;
        }

		// switch to day after night timer expired
		if (night_counter.counter_ms < 0) {
			registry.nightTimer.clear();

			screen.game_screen = game_screen_day;
			screen.torch_mode = 0;

			restart_game();
			return true;
		}
		screen.night_counter_ms = night_counter.counter_ms;
	}

	animation_elpased_ms += elapsed_ms_since_last_update;
	keeper_spawn_time += elapsed_ms_since_last_update;
	if (animation_elpased_ms > ANIMATION_SPEED) {
		animation_elpased_ms = 0.0;
		animation_cycle++;
		screen.torch_anim = animation_cycle % 3;
		animation_cycle %= 4;
	}
	//const float KEEPER_SPAWN_FREQ = 10.0 * 1000.0;
	//f//loat keeper_spawn_time = 0.0f;
	if (keeper_spawn_time > KEEPER_SPAWN_FREQ && screen.game_screen == game_screen_night) {
		keeper_spawn_time = 0.0;
		vec2 spawn_pos = registry.get_empty_spawn_keeper_location();
		createKeeper(renderer, spawn_pos);
	}
	



	// Updating window title with points
	std::stringstream title_ss;
	//title_ss << "Points: " << points;
	int fps = 1000 / elapsed_ms_since_last_update;
	title_ss << "FPS: " << fps;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Animating enemy entities
	auto& enemies = registry.enemies.components;
	for (int i = 0; i < enemies.size(); i++ ) {
		RenderRequest& r = registry.renderRequests.get( enemies[i].e_id );
		if (animation_cycle % 2 == 1) {
			r.used_texture = TEXTURE_ASSET_ID::KEEPER2;
		} else {
			r.used_texture = TEXTURE_ASSET_ID::KEEPER1;
		}
	}

	// debugging maze tiles
	draw_chunk_borders();

    camera.step();

	///  put help back
	Motion& help_motion = registry.motions.get(help);
	Immobile& help_immobile = registry.immobiles.get(help);
	vec2 help_pos = vec2(window_width_px*0.5, window_height_px*0.5);
	help_motion.position = help_pos;
	help_motion.camera_position = help_pos;
	help_immobile.position = help_pos;
	help_immobile.camera_position = help_pos;

	if ( screen.game_screen == game_screen_win) {
		Motion& mot = registry.motions.get(win_dance);
		mot.position = { 400.0, 400.0 + 1.0*(animation_cycle % 2) };
		RenderRequest& render_request = registry.renderRequests.get(win_dance);
		TEXTURE_ASSET_ID animation_frame = (TEXTURE_ASSET_ID) (  (int)TEXTURE_ASSET_ID::WIN0 + (animation_cycle % 2) );
		render_request.used_texture = animation_frame;
	}

    ai.step();
//    Entity p = registry.players.entities[0];
    Motion& p_motion = registry.motions.get(player);

//   createHelp();
    if (screen.game_screen == game_screen_night) {
        // Displaying health bar
        auto healthBG = createLine(vec2(250, 60), vec2(408, 48));
        registry.colors.insert(healthBG, { 0, 0, 0 });

        auto healthBar = createLine(vec2(50 + 2 * player_component.health, 60), vec2(4*player_component.health, 40));
        registry.colors.insert(healthBar, { 1, 0, 0 });

        // inventory bar position on screen
        vec2 pos = {720, 60};
        createInventory(pos);

    createHelp();
//    Entity help = registry.helpScreen.entities[0];
//    vec2 help_pos = registry.motions.get(help).camera_position;
//    help_pos = {window_width_px / 2 + 1.0, 0.5 * window_height_px};

        for (int i = 0; i < 5; i++) {
            Inventory& player_inv = registry.inventory.get(player);
            auto& item = player_inv.holding[i];

            Entity localItem = createInventoryPotion(item, {pos.x - 120 + (60.0 * i), pos.y});
            if (player_inv.select_state == i) {
                registry.motions.get(localItem).scale = { 100, 100 };
            }
            else {
                registry.motions.get(localItem).scale = { 50, 50 };
            }
        }
    }


    if (screen.game_screen == game_screen_day) {
        vec2 pos = {820, 60};
        createInventory(pos);

        for (int i = 0; i < 5; i++) {
            Inventory& player_inv = registry.inventory.get(player);
            auto& item = player_inv.holding[i];

            Entity localItem = createInventoryPotion(item, {pos.x - 120 + (60.0 * i), pos.y});
            if (player_inv.select_state == i) {
                registry.immobiles.get(localItem).scale = { 100, 100 };
            }
            else {
                registry.immobiles.get(localItem).scale = { 50, 50 };
            }
        }

        if (registry.inventory.get(player).is_open == true) {
            createCraftingUI({0.5 * window_width_px, 0.5 * window_height_px});
            for (int i = 0; i < registry.inventory.get(player).inUI.size(); i++) {
                Item item = registry.inventory.get(player).inUI[i];
                Entity e = createInventoryPotion(item, { pos.x - 472 + (197 * i), pos.y + 115});
                registry.immobiles.get(e).scale = { 90, 90 };
            }
            checkCrafted();
            std::vector<unsigned int> allCrafted = registry.inventory.get(registry.players.entities[0]).crafted;
            auto rec1 = std::find(allCrafted.begin(), allCrafted.end(), 1);
            auto rec2 = std::find(allCrafted.begin(), allCrafted.end(), 2);
            auto rec3 = std::find(allCrafted.begin(), allCrafted.end(), 3);
            if (rec1 != allCrafted.end() && rec2 != allCrafted.end() && rec3 != allCrafted.end()) {
                // call for end game - sucess
            }
        }
    }

	//createShadowOverlay(renderer);

	if (num_of_item_on_screen < NUM_POTIONS) {
		std::vector<int> randomizer = { 0,1,2,3,4,5,6,7,8,9,10,11 };
		for (int i = 0; i < 5; i++) {
			Item& item = registry.inventory.get(player).holding[i];
			switch (item) {
				case (magical_mushroom):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 0), randomizer.end());
					break;
				case (magical_strawberry):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 1), randomizer.end());
					break;
				case (magical_grapes):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 2), randomizer.end());
					break;
				case (red_potion):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 3), randomizer.end());
					break;
				case (green_potion):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 4), randomizer.end());
					break;
				case (purple_potion):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 5), randomizer.end());
					break;
				case (orange_potion):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 6), randomizer.end());
					break;
				case (magical_apple):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 7), randomizer.end());
					break;
				case (magical_carrot):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 8), randomizer.end());
					break;
				case (food):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 9), randomizer.end());
					break;
				case (special_lotus):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 10), randomizer.end());
					break;
				case (torch):
					break;
			}
		}

		vec2 spawn_pos = registry.get_empty_spawn_location();
		createPotion(renderer, spawn_pos, randomizer);
		//for (size_t i = 0; i < randomizer.size(); ++i) {
		//	printf("%d ", randomizer[i]);
		//}
		//printf("\n");
		num_of_item_on_screen++;
	}


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
            screen.game_screen = game_screen_game_over;
            player_component.alive = 0;
            restart_game();
			return true;
		}
	}

	for (Entity entity : registry.torches.entities) {
		// progress timer
		Torches& counter = registry.torches.get(entity);
		counter.t_timer -= elapsed_ms_since_last_update;
		if (counter.t_timer > 0) {
			screen.torch_mode = 1;
		}
		else {
			screen.torch_mode = 0;
			registry.torches.remove(entity);
		}
	}

	// reduce window brightness if any of the present is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

    // on collision with enemy
    float hit_counter_ms = 3000.f;
    for (Entity entity : registry.hitTimers.entities)
    {
        // progress timer
        HitTimer &counter = registry.hitTimers.get(entity);
        counter.counter_ms -= elapsed_ms_since_last_update;

        if (counter.counter_ms < hit_counter_ms)
        {
            hit_counter_ms = counter.counter_ms;
        }

        if (counter.counter_ms < 0)
        {
            player_component.hit = 0;
            registry.invincibles.remove(entity);
            registry.hitTimers.remove(entity);
            return true;
        }
    }

	return true;
}

void WorldSystem::checkCrafted() {
	std::vector<Item> UIItem = registry.inventory.get(registry.players.entities[0]).inUI;
	auto rec1 = std::find(UIItem.begin(), UIItem.end(), magical_mushroom);
	auto rec2 = std::find(UIItem.begin(), UIItem.end(), green_potion);
	auto rec3 = std::find(UIItem.begin(), UIItem.end(), orange_potion);
	auto rec4 = std::find(UIItem.begin(), UIItem.end(), magical_strawberry);
	auto rec5 = std::find(UIItem.begin(), UIItem.end(), red_potion);
	auto rec6 = std::find(UIItem.begin(), UIItem.end(), magical_carrot);
	auto rec7 = std::find(UIItem.begin(), UIItem.end(), magical_grapes);
	auto rec8 = std::find(UIItem.begin(), UIItem.end(), purple_potion);
	auto rec9 = std::find(UIItem.begin(), UIItem.end(), magical_apple);
	if (rec1 != UIItem.end() && rec2 != UIItem.end() && rec3 != UIItem.end()) {
		registry.inventory.get(registry.players.entities[0]).crafted.push_back(1);
	}
	if (rec4 != UIItem.end() && rec5 != UIItem.end() && rec6 != UIItem.end()) {
		registry.inventory.get(registry.players.entities[0]).crafted.push_back(2);
	}
	if (rec7 != UIItem.end() && rec8 != UIItem.end() && rec9 != UIItem.end()) {
		registry.inventory.get(registry.players.entities[0]).crafted.push_back(3);
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();

	printf("Restarting\n");

    game_paused = false;
	// Reset the game speed
	current_speed = 1.f;
//    inventory = 0;
	num_of_item_on_screen = 0;
	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
//    for (Entity entity : registry.motions.entities)
//    {
//        if (!registry.helpScreen.has(entity)) {
//            registry.remove_all_components_of(entity);
//        }
//    }
	//Entity tempPLayer = player;
	//Inventory& tempInventory = registry.inventory.get(player);

	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());
	while (registry.immobiles.entities.size() > 0)
		registry.remove_all_components_of(registry.immobiles.entities.back());
	while (registry.inventory.entities.size() > 0)
		registry.remove_all_components_of(registry.inventory.entities.back());
	while (registry.chunks.entities.size() > 0)
		registry.remove_all_components_of(registry.chunks.entities.back());
	while (registry.players.entities.size() > 0)
		registry.remove_all_components_of(registry.players.entities.back());
    for (Entity entity : registry.textRenderRequests.entities)
    {
        if (!registry.pauses.has(entity)) {
            registry.remove_all_components_of(entity);
        }
    }
//    while (registry.screenStates.entities.size() > 0)
//        registry.remove_all_components_of(registry.screenStates.entities.back());
	//registry.clear_all_components();
	generator->clearMap();
	//printf("AFTER CLEARED REGISTRY");
	registry.list_all_components();
	// Create a new player
	float start_x = MAZE_SIZE * TILE_SIZE * 1.5;
	float start_y = MAZE_SIZE * TILE_SIZE * 1.5;
	//registry.get_empty_spawn_location();


	player = createPlayer(renderer, registry.get_empty_spawn_location());
	//player = createPlayer(renderer, { window_width_px / 2, window_height_px - 200 });

	// vec2(start_x, start_y);
	//if (level > 1) {
	//	registry.inventory.insert(player,tempInventory);
	//} else {
	registry.inventory.emplace(player);

	registry.inventory.get(player).holding.push_back(nullval);
	registry.inventory.get(player).holding.push_back(nullval);
	registry.inventory.get(player).holding.push_back(nullval);
	registry.inventory.get(player).holding.push_back(nullval);
	registry.inventory.get(player).holding.push_back(nullval);
	save_state.loadInventory();
	//}
//	registry.inventory.get(player).holding.push_back(nullval);
    registry.inventory.get(player).holding.push_back(magical_grapes);

    save_state.loadInventory();


    help = createHelp();
	//printf("MAP AND PLAYER CREATED\n");

	// create a compass to calc player movement
	Entity compass = createCompass(vec2(155.0,155.0));
	Immobile& imo = registry.immobiles.get(compass);
	compass_immobile = &imo;
	//printf("COMPASS SETTING: x %f  y: %f", compass_immobile->position.x, compass_immobile->position.x);

	physics->setCompass(&compass,&imo);

	registry.colors.insert(player, { 1, 0.8f, 0.8f });

    Player& player_component = registry.players.components[0];
    ScreenState &screen = registry.screenStates.components[0];


    if (screen.game_screen == game_screen_game_over) {
//        printf("in game over screen\n");

        auto over = Entity();
        registry.textRenderRequests.insert(over,
                                           {"Game over", vec2(0.33 * window_width_px, 0.5 * window_height_px)});
        TextRenderRequest& over_text = registry.textRenderRequests.get(over);
        over_text.scale = 1.2;
        auto spaceKey = Entity();
        registry.textRenderRequests.insert(spaceKey,
                                           {
                                                   "Press space key to restart", vec2(0.27 * window_width_px, 0.4 * window_height_px) // might need to adjust position
                                           });
        TextRenderRequest& spaceKeyText = registry.textRenderRequests.get(spaceKey);
        spaceKeyText.scale = 0.6;

        Entity end_keeper = createKeeper(renderer, { window_width_px / 2, window_height_px / 2 });
        Motion& keeper_motion = registry.motions.get(end_keeper);
        keeper_motion.position = { window_width_px / 2, window_height_px / 2 };
        keeper_motion.scale *= 4;
        keeper_motion.velocity = {0,0};

        player_component.alive = 0;
    }
    else if (screen.game_screen == game_screen_night) {
        printf("in game night screen\n");
        // Playing background music indefinitely
		createNight();
        Mix_PlayMusic(night_music, -1);
        fprintf(stderr, "Loaded music\n");
		generator->makeNoise(level);
        generator->createWorldMap();
		Motion& p_motion = registry.motions.get(player);
		p_motion.position = registry.get_empty_spawn_keeper_location();
		if (level < 9) {
			level++;
			keeper_rate-= 0.8;
			KEEPER_SPAWN_FREQ = keeper_rate * 1000.0;
		}
		for (int k = 0; k < level +2; k++) {
			vec2 spawn_point = registry.get_empty_spawn_keeper_location();
			createKeeper(renderer, spawn_point);
		}


    }
    else if (screen.game_screen == game_screen_day || screen.game_screen == game_screen_main_menu) {
        // Playing background music indefinitely
        Mix_PlayMusic(day_music, -1);
        fprintf(stderr, "Loaded music\n");
        printf("here\n");

        Motion& player_motion = registry.motions.get(player);
        player_motion.position = { 0.6 * window_width_px, 0.75 * window_height_px };
        // put player alive again
        player_component.alive = 1;
        player_motion.scale = { 2 * 60, 2 * 80 };
        printf("created player\n");

        Entity day = createDay();
        printf("day entity init %zu\n", day);

        if (screen.game_screen == game_screen_day) {
            printf("in game day screen\n");
            createDog({ 0.85 * window_width_px, 0.75 * window_height_px });

            createSign();
            createSign2();
        } else {
            printf("in game start screen\n");
            createDog({ 0.3 * window_width_px, 0.8 * window_height_px });

            createLogo();

            auto spaceKeyText = Entity();
            registry.textRenderRequests.insert(spaceKeyText,
                                               {
                                                       "Press space key to start", vec2(0.3 * window_width_px, 0.4 * window_height_px) // might need to adjust position
                                               });
            TextRenderRequest& text = registry.textRenderRequests.get(spaceKeyText);
            text.scale = 0.6;
        }

    }
    else if (screen.game_screen == game_screen_win) {


        //  change music to victory music
        Mix_PlayMusic(day_music, -1);
        fprintf(stderr, "Loaded music\n");

        Motion& player_motion = registry.motions.get(player);
        player_motion.position = { window_width_px / 2, window_height_px - 200 };

        Entity day = createDay();
        printf("day entity init %zu\n", day);
		//screen.torch_mode = 0;
		//screen.night_counter_ms = 0;

		win_dance = createWinDance(renderer, { 0.6 * window_width_px, 0.75 * window_height_px });
		renderer->setDance(win_dance);
		// get win animation


        auto win = Entity();
        registry.textRenderRequests.insert(win,
                                           {"You win!", vec2(0.35 * window_width_px, 0.64 * window_height_px)});
        TextRenderRequest& over_text = registry.textRenderRequests.get(win);
        over_text.scale = 1.4;

        auto savedDog = Entity();
        registry.textRenderRequests.insert(savedDog,
                                           {
                                                   "Congratulations! You saved your beloved dog!", vec2(0.12 * window_width_px, 0.56 * window_height_px) // might need to adjust position
                                           });
        TextRenderRequest& savedDogText = registry.textRenderRequests.get(savedDog);
        savedDogText.scale = 0.6;

        auto spaceKey = Entity();
        registry.textRenderRequests.insert(spaceKey,
                                           {
                                                   "Press space key to play again", vec2(0.23 * window_width_px, 0.5 * window_height_px) // might need to adjust position
                                           });
        TextRenderRequest& spaceKeyText = registry.textRenderRequests.get(spaceKey);
        spaceKeyText.scale = 0.6;


       // createDog({ window_width_px / 2, window_height_px - 200 });


    }

	// Create empty "path" maze
	std::vector<std::vector<Tile>> maze(MAZE_HEIGHT*3, std::vector<Tile>(MAZE_WIDTH*3, Tile(0)));
	//// ADD PLANTS HERE Note: ordering matter here unless we implement a z axis for layers
	auto& chunks = generator->chunks;
	//for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
	//for (Chunk c : chunks) {
	for (uint i = 0; i < chunks.size(); i++) { //chunks.size()
		auto entity = Entity();
		registry.chunks.insert(entity, chunks[i], false);

		for (int row = 0; row < MAZE_SIZE; row++) {
			for (int col = 0; col < MAZE_SIZE; col++) {
				Chunk c = chunks[i];
				if ( c.grid[row][col] > 0) {
					int major_row = c.y * MAZE_SIZE + row;
					int major_col = c.x * MAZE_SIZE + col;
					// add the TILE_SIZE / 2 so that plants are aligned with their respective cell
					int plant_x = c.offset_x + col * TILE_SIZE + TILE_SIZE / 2;
					int plant_y = c.offset_y + row * TILE_SIZE + TILE_SIZE / 2;
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
	// wait();
	// Debugging for memory/component leaks
	//torchEntity = createShadowOverlay(renderer);

	registry.list_all_components();


}

void WorldSystem::pause_game() {

	game_paused = true;
	//printf("Game paused \n");

	auto entity = Entity();
    registry.pauses.emplace(entity);
	registry.textRenderRequests.insert(entity,
		{
			"PAUSED", vec2(0.4 * window_width_px, 0.5 * window_height_px) // might need to adjust position
		});
	for (Motion& motion : registry.motions.components) {
		motion.velocity = { 0, 0 };
	}
}

void WorldSystem::resume_game() {
	
	game_paused = false;
	// Remove "Paused" text (does it remove too much?)
//	while (registry.textRenderRequests.entities.size() > 0) {
//		registry.remove_all_components_of(registry.textRenderRequests.entities.back());
//	}

    registry.remove_all_components_of(registry.pauses.entities[0]);
	
	//printf("Game unpaused \n");
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
        Player& player_component = registry.players.components[0];

		// For now, we are only interested in collisions that involve the chicken
		if (!game_paused && registry.players.has(entity)) {
			Player& p = registry.players.get(entity);
		
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

			// Checking Player - Enemy collisions
			if (registry.enemies.has(entity_other)) {
				Enemy& e = registry.enemies.get(entity_other);
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity) && !registry.invincibles.has(entity) && player_component.alive == 1) {
					Motion& enemy_motion = registry.motions.get(entity_other);

					if ((p.health - e.attack) > 0) {
						p.health = p.health - e.attack;
                        p.hit = 1;
                        // make player temporarily invincible
                        if (!registry.invincibles.has(entity))
                        {
                            registry.invincibles.emplace(entity);
                        }

                        if (!registry.hitTimers.has(entity))
                        {
                            registry.hitTimers.emplace(entity);
                        }

						// Move enemy slightly back after collision (so that player doesn't die instantly)
						enemy_motion.position = enemy_motion.position + (enemy_motion.velocity * vec2(-0.5, -0.5));
						Mix_PlayChannel(-1, player_hit_sound, 0); // TODO: replace with "hit" sound
					}
					else {
						p.health = 0;
						registry.deathTimers.emplace(entity);
						// player should stop moving when health is <= 0
						registry.motions.get(player).velocity = { 0, 0 };
						Mix_PlayChannel(-1, player_death_sound, 0);
					}
				}
			
			}
			// Checking Player - Eatable collisions
			else if (registry.healers.has(entity_other)) {
				Inventory& player_inv = registry.inventory.get(entity);
				if (player_inv.holding[0] == nullval) {
					player_inv.holding[0] = registry.healers.get(entity_other).item_name;
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, mushroom_sound, 0);
				} else if (player_inv.holding[1] == nullval) {
					player_inv.holding[1] = registry.healers.get(entity_other).item_name;
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, mushroom_sound, 0);
				} else if (player_inv.holding[2] == nullval) {
					player_inv.holding[2] = registry.healers.get(entity_other).item_name;
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, mushroom_sound, 0);
				} else if (player_inv.holding[3] == nullval) {
					player_inv.holding[3] = registry.healers.get(entity_other).item_name;
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, mushroom_sound, 0);
				} else if (player_inv.holding[4] == nullval) {
					player_inv.holding[4] = registry.healers.get(entity_other).item_name;
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, mushroom_sound, 0);
				}
				else {
					Mix_PlayChannel(-1, inventory_full_sound, 0);
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

    // handle boundaries during daytime
    ScreenState& screen = registry.screenStates.components[0];
    if (screen.game_screen == game_screen_day) {
        Motion& player_motion = registry.motions.get(player);
        float x_pos = player_motion.position.x;
        float y_pos = player_motion.position.y;
        float half_width = PLAYER_WIDTH / 2;
        float half_height = PLAYER_HEIGHT / 2;
        float top = 165;
        float bottom = window_height_px - (0.8 * PLAYER_HEIGHT);
        float shed_side = 814;

        // check left
        if (x_pos - half_width < 0) {
            player_motion.position.x = half_width;
        }
        // check right
        else if (x_pos + half_width > window_width_px) {
            player_motion.position.x = window_width_px - half_width;
        }
        // check right shed
        else if (x_pos > shed_side && y_pos < 470) {
            if (playerDirection == RIGHT) {
                player_motion.position.x = shed_side;
            } else if (playerDirection == UP) {
                player_motion.position.y = 470;
            }
        }
        // check top left corner
        else if (y_pos < top && x_pos < 500) {
            if (playerDirection == LEFT) {
                player_motion.position.x = 500;
            } else if (playerDirection == UP) {
                player_motion.position.y = top;
            }
        }
        // check top right corner
        else if (y_pos < top && x_pos > 600) {
            if (playerDirection == RIGHT) {
                player_motion.position.x = 600;
            } else if (playerDirection == UP) {
                player_motion.position.y = top;
            }
        }
        else if (y_pos < half_height) {
            screen.game_screen = game_screen_night;
            restart_game();
        }
        // check bottom
        else if (y_pos > bottom) {
            player_motion.position.y = bottom;
        }

    }

}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT


	if (action == GLFW_RELEASE && key == GLFW_KEY_L) {
		chunk_border = !chunk_border;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_O) {
		for (const auto& chunk : registry.chunks.components) {
			std::cout << "x: " << chunk.x << " y: " << chunk.y << " offset_x: " << chunk.offset_x << " offset_y: " << chunk.offset_y << std::endl;
		}
	}

	// remove items from slot on num key press
	if (action == GLFW_RELEASE && key == GLFW_KEY_1) {
		registry.inventory.get(registry.players.entities[0]).select_state = 0;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_2) {
		registry.inventory.get(registry.players.entities[0]).select_state = 1;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_3) {
		registry.inventory.get(registry.players.entities[0]).select_state = 2;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_4) {
		registry.inventory.get(registry.players.entities[0]).select_state = 3;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_5) {
		registry.inventory.get(registry.players.entities[0]).select_state = 4;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_Z) {
		int selected_inv_state = registry.inventory.get(registry.players.entities[0]).select_state;
		registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_M) {
		//std::cout << "in here!" << std::endl;
		while (registry.enemies.entities.size() > 0)
			registry.remove_all_components_of(registry.enemies.entities.back());
	}

	// Exiting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		std::cout << "EXIT GAME" << std::endl;
		save_state.saveInventory();
		glfwSetWindowShouldClose(window, true);
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Pausing game
	if (action == GLFW_RELEASE && key == GLFW_KEY_P) {

		if (!game_paused) {
			pause_game();
		}
		else {
			resume_game();
		}
	}

	ScreenState& screen = registry.screenStates.components[0];
    // Changing game scene to day
    if (action == GLFW_RELEASE && key == GLFW_KEY_N) {
        printf("N key pressed, switching screen");
        
        if (screen.game_screen == game_screen_night) {
            screen.game_screen = game_screen_day;
			screen.torch_mode = 0;
			screen.night_counter_ms = 0;
			NightTimer& night_counter = registry.nightTimer.components[0];
			night_counter.counter_ms = 0;
        } else {
            screen.game_screen = game_screen_night;
        }
         restart_game();
    }

	if (action == GLFW_RELEASE && key == GLFW_KEY_X) {
		int selected_inv_state = registry.inventory.get(registry.players.entities[0]).select_state;
		if (registry.inventory.get(registry.players.entities[0]).is_open == false) {
			if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == food) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 20;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			}
			else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == torch) {
				registry.torches.emplace(Entity());
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			}
		}
		else {
			if (registry.inventory.get(registry.players.entities[0]).inUI.size() < 3) {
				registry.inventory.get(registry.players.entities[0]).inUI.push_back(registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state]);
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_U) {
		registry.inventory.get(registry.players.entities[0]).is_open = !registry.inventory.get(registry.players.entities[0]).is_open;
		registry.inventory.get(registry.players.entities[0]).inUI = {};
	}

    // Opening help screen
    if (action == GLFW_RELEASE && key == GLFW_KEY_H) {
        HelpScreen &screen = registry.helpScreen.components[0];
        if (screen.help == 1) {
           // printf("H key pressed, closing help screen\n");
            screen.help = 0;
            game_paused = false;
        } else {
            //printf("H key pressed, opening help screen\n");
            screen.help = 1;
            game_paused = true;
            for (Motion& motion : registry.motions.components) {
                motion.velocity = { 0, 0 };
            }
        }
    }

	// Torch Chnage this to %3 if adding new type of torch or need alternate night shader functionality
	if (key == GLFW_KEY_T && action == GLFW_RELEASE && screen.game_screen == game_screen_night) {
		screen.torch_mode = (screen.torch_mode + 1) %2;
		if (torch_timer_ms == 0.0 && screen.torch_mode == 1) {
			torch_timer_ms = TORCH_LENGTH;
		}

	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	if (!game_paused && !registry.deathTimers.has(player)) {
		auto& motion = registry.motions.get(player);

        ScreenState& screen = registry.screenStates.components[0];
        if (screen.game_screen == game_screen_night || screen.game_screen == game_screen_day) {
            // Character movement
            // Move Up
            if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
                playerDirection = UP;
                RenderRequest& r = registry.renderRequests.get(player);
                r.used_texture = (TEXTURE_ASSET_ID)(animation_cycle + 1);
                if (!blockedNorth) {
                    motion.velocity = vec2(0.f, -CHARACTER_SPEED);
                }
               // printf("pos: %f %f \n", motion.position.x, motion.position.y);
            }

            // Move Down
            if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
                playerDirection = DOWN;
                RenderRequest& r = registry.renderRequests.get(player);
                r.used_texture = (TEXTURE_ASSET_ID)(animation_cycle + 1);
                if (!blockedSouth) {
                    motion.velocity = vec2(0.f, CHARACTER_SPEED);
                }
                //printf("pos: %f %f \n", motion.position.x, motion.position.y);
            }

            // Move Right
            if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
                playerDirection = RIGHT;
                RenderRequest& r = registry.renderRequests.get(player);
                r.used_texture = (TEXTURE_ASSET_ID)(animation_cycle + 1);
                if (!blockedEast) {
                    motion.velocity = vec2(CHARACTER_SPEED, 0.f);
                    if (screen.game_screen == game_screen_day) {
                        motion.scale = vec2(2 * -PLAYER_WIDTH, 2 * PLAYER_HEIGHT);
                    } else {
                        motion.scale = vec2(-PLAYER_WIDTH, PLAYER_HEIGHT);
                    }
                }
                //printf("pos: %f %f \n", motion.position.x, motion.position.y);
            }

            // Move Left
            if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
                playerDirection = LEFT;
                RenderRequest &r = registry.renderRequests.get(player);
                r.used_texture = (TEXTURE_ASSET_ID) (animation_cycle + 1);
                if (!blockedWest) {
                    motion.velocity = vec2(-CHARACTER_SPEED, 0.f);
                    if (screen.game_screen == game_screen_day) {
                        motion.scale = vec2(2 * PLAYER_WIDTH, 2 * PLAYER_HEIGHT);
                    } else {
                        motion.scale = vec2(PLAYER_WIDTH, PLAYER_HEIGHT);
                    }
                }
                //printf("pos: %f %f \n", motion.position.x, motion.position.y);
            }
        }

		// Stop moving when key is released
		if (action == GLFW_RELEASE) {
			motion.velocity = vec2(0.f, 0.f);
			RenderRequest& r = registry.renderRequests.get(player);
			r.used_texture = TEXTURE_ASSET_ID::PLAYER_CHAR1;
		}
	}

    // space to start game
    if (screen.game_screen == game_screen_main_menu) {
        if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
            screen.game_screen = game_screen_day;
            restart_game();
        }
    }

    // space to restart
    if (screen.game_screen == game_screen_game_over) {
        if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
            printf("in game over screen on key \n");
            screen.game_screen = game_screen_main_menu;
            screen.torch_mode = 0;
            screen.night_counter_ms = 0;
            NightTimer& night_counter = registry.nightTimer.components[0];
            night_counter.counter_ms = 0;
            registry.nightTimer.clear();
            restart_game();
        }
    }

    // space to restart
    if (screen.game_screen == game_screen_win) {
        if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
            printf("in game win screen on key \n");
            screen.game_screen = game_screen_main_menu;
//            screen.torch_mode = 0;
            screen.night_counter_ms = 0;
            registry.nightTimer.clear();
            restart_game();
        }
    }

    // v to victory screen
   if (action == GLFW_RELEASE && key == GLFW_KEY_V) {
       if (screen.game_screen != game_screen_night) {
           screen.game_screen = game_screen_win;
           restart_game();
       }
    }

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		//printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		//printf("Current speed = %f\n", current_speed);
	}

	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// xpos and ypos are relative to the top-left of the window, the player's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	//vec2 mouse_pos = camera.adjust_to_player({ xpos, ypos });

	//std::cout << "x: " << mouse_pos.x << ", y: " << mouse_pos.y << std::endl;
	//int chunk_value = registry.get_chunk_value(camera.adjust_to_player(vec2{xpos, ypos}));
	//MazeLocation mazeLoc = registry.get_maze_location(camera.adjust_to_player(vec2{ xpos, ypos }));
	//std::cout << "x: " << mazeLoc.col << " y: " << mazeLoc.row << std::endl;
	//int chunk_value = registry.get_chunk_value(mazeLoc);
	//std::cout << "CHUNK VALUE: " << chunk_value << std::endl;

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
//		vec2 spawn_pos = registry.get_empty_spawn_location();
//		printf("Spawning object at x %f  y %f\n", spawn_pos.x , spawn_pos.y);
//		createPotion(renderer, camera.adjust_to_player(spawn_pos));
		createKeeper(renderer, camera.adjust_to_player(vec2{ xpos, ypos }));
		//if (points <= 5) {
		//	createPotion(renderer, camera.adjust_to_player(vec2{ xpos, ypos }));
		//}

	}
	
	
}

void WorldSystem::draw_chunk_borders() {
	if (!chunk_border) {
		return;
	}
	int maze_pixel_size = MAZE_SIZE * TILE_SIZE;
	int line_width = 10;
	//std::cout << position_vec.x << ", " << position_vec.y << std::endl;
	//vec2 scale_vec = 
	vec3 blue = { 0, 0, 1.0 };
	for (const auto& chunk : registry.chunks.components) {

		// for grid lines
		// 
		for (int i = 0; i < MAZE_SIZE; i++) {
			Entity vertical_line = createLine(vec2{ chunk.offset_x + i * TILE_SIZE, chunk.offset_y + maze_pixel_size / 2 }, vec2{line_width, maze_pixel_size});
			Entity horizontal_line = createLine(vec2{ chunk.offset_x + maze_pixel_size / 2, chunk.offset_y + i * TILE_SIZE }, vec2{ maze_pixel_size, line_width });
			registry.colors.insert(vertical_line, blue);
			registry.colors.insert(horizontal_line, blue);
		}

		Entity left_line = createLine(vec2{ chunk.offset_x, chunk.offset_y + maze_pixel_size / 2 }, vec2{ line_width, maze_pixel_size });
		Entity right_line = createLine(vec2{ chunk.offset_x + maze_pixel_size, chunk.offset_y + maze_pixel_size / 2 }, vec2{ line_width, maze_pixel_size });
		Entity top_line = createLine(vec2{ chunk.offset_x + maze_pixel_size / 2, chunk.offset_y }, vec2{ maze_pixel_size, line_width });
		Entity bottom_line = createLine(vec2{ chunk.offset_x + maze_pixel_size / 2, chunk.offset_y + maze_pixel_size }, vec2{ maze_pixel_size, line_width });

	}
}
