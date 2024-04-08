// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "tiny_ecs.hpp"

#include <ctime>
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

Direction playerDirection;
const uint NUM_POTIONS = 10;
const float CHARACTER_SPEED = 300.0f;
const float ANIMATION_SPEED = 1000.0 / 16.0;
const int SPARKLE_SLOWDOWN_FPS = 1;
int sparkle_slowdown = 0;
float keeper_rate = 8.0;
float KEEPER_SPAWN_FREQ = keeper_rate * 1000.0;
float keeper_spawn_time = 0.0f;
int num_of_item_on_screen = 0;
uint animation_cycle = 0;
float animation_elpased_ms = 0.0f;
int level = 0;
const int max_level = 9;
bool morning_warning = false;
//const float TORCH_LENGTH = 25 * 1000.0;
//float torch_timer_ms = TORCH_LENGTH;

bool game_paused = false;
bool is_in_debug = false;
bool already_paused = false;

bool moving_x = false;
bool moving_y = false;

bool orange_crafted = false;
bool green_crafted = false;
bool purple_crafted = false;

bool go_up_key_pressed = false;
bool go_down_key_pressed = false;
bool go_right_key_pressed = false;
bool go_left_key_pressed = false;

// Create the bug world
WorldSystem::WorldSystem() : points(0), chunk_border(false) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
	srand(time(0));
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
	if (page_turn_sound != nullptr)
		Mix_FreeChunk(page_turn_sound);
	if (menu_close_sound != nullptr)
		Mix_FreeChunk(menu_close_sound);
	if (rooster_sound != nullptr)
		Mix_FreeChunk(rooster_sound);
    if (torch_sound != nullptr)
        Mix_FreeChunk(torch_sound);
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
	auto scroll_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_scroll(_0, _1); };

	glfwSetKeyCallback(window, key_redirect);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_click_redirect);
	glfwSetScrollCallback(window, scroll_redirect);

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
  	rooster_sound = Mix_LoadWAV(audio_path("rooster.wav").c_str());
    torch_sound = Mix_LoadWAV(audio_path("torch.wav").c_str());
	  page_turn_sound = Mix_LoadWAV(audio_path("page_turn.wav").c_str());
	  menu_close_sound = Mix_LoadWAV(audio_path("menu_close.wav").c_str());

	if (night_music == nullptr || day_music == nullptr || bird_sounds == nullptr || mushroom_sound == nullptr ||
        player_death_sound == nullptr || inventory_full_sound == nullptr || rooster_sound == nullptr || torch_sound == nullptr
		|| page_turn_sound == nullptr || menu_close_sound == nullptr ) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s \n make sure the data directory is present",
            audio_path("forest_night.wav").c_str(),
            audio_path("day.wav").c_str(),
            audio_path("bird_noises.wav").c_str(),
            audio_path("mushroom.wav").c_str(),
            audio_path("player_death.wav").c_str(),
            audio_path("inventory_full.wav").c_str(),
			audio_path("player_hit.wav").c_str(),
      audio_path("rooster.wav").c_str(),
      audio_path("torch.wav").c_str(),
			audio_path("page_turn.wav").c_str(),
			audio_path("menu_close.wav").c_str());

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

	// Don't do anything if game is paused
	if (game_paused) return true;

	if (registry.players.get(player).moving) {
		RenderRequest& r = registry.renderRequests.get(player);
		r.used_texture = (TEXTURE_ASSET_ID)(animation_cycle + 1);
	}

	//	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];
    Player& player_component = registry.players.components[0];

	// tabbing out of the game or minimizing the window will pause the game
	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
	if (!focused && screen.game_screen == game_screen_night) pause_game();

    // don't do anything if player is dead
    if (player_component.alive == 0) return true;

    if (registry.nightTimer.size() > 0) {
        float min_night_counter_ms = NIGHT_DURATION;
        NightTimer& night_counter = registry.nightTimer.components[0];
        // progress timer
        night_counter.counter_ms -= elapsed_ms_since_last_update;
        if (night_counter.counter_ms < min_night_counter_ms) {
            min_night_counter_ms = night_counter.counter_ms;
        }

		// if we are nearing day and we haven't played the rooster sound yet
		if (night_counter.counter_ms < NIGHT_DURATION / 10 && !night_counter.rooster_played)
		{
			Mix_PlayChannel(-1, rooster_sound, 0);
			night_counter.rooster_played = true;
		}

		// switch to day after night timer expired
		if (night_counter.counter_ms < 0) {
			registry.nightTimer.clear();

            if (level == 1) {
                screen.game_screen = cut_scene_3;
            } else if (level == 7) {
                screen.game_screen = cut_scene_warning;
            } else if (level == 9) {
                screen.game_screen = cut_scene_too_late;
            } else {
                screen.game_screen = game_screen_day;
            }
			screen.torch_mode = 0;

			// save inventory when returning to day
			GameState state = {
				registry.inventory.get(registry.players.entities[0]).holding,
				orange_crafted,
				green_crafted,
				purple_crafted,
				level
			};
			save_state.saveGame(state);

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
		sparkle_slowdown++;
		if (sparkle_slowdown > SPARKLE_SLOWDOWN_FPS) {
			sparkle_slowdown = 0;
		}

		if (sparkle_slowdown == 0 && registry.player_sparkle_animation.entities.size() > 0) {
			// also do for dog
			for (Entity& animated_entity : registry.player_sparkle_animation.entities) {
				AnimatedPlayerSparkle& face = registry.player_sparkle_animation.get(animated_entity);
				// No need to check if render request since should always have a render request
				RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
				face.current_cell++;
				if (face.current_cell >= face.size) {
					face.current_cell = 0;
				}
				anim_rr.used_texture = face.cells[face.current_cell];
			}
		}

		if (registry.torch_off_animation.entities.size() > 0) {
			// also do for dog
			for (Entity& animated_entity : registry.torch_off_animation.entities) {
				AnimatedTorchOff& face = registry.torch_off_animation.get(animated_entity);
				// No need to check if render request since should always have a render request
				RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
				face.current_cell++;
				if (face.current_cell >= face.size) {
					face.current_cell = 0;
				}
				anim_rr.used_texture = face.cells[face.current_cell];
			}
		}
		if (registry.torch_on_animation.entities.size() > 0) {
			// also do for dog
			for (Entity& animated_entity : registry.torch_on_animation.entities) {
				AnimatedTorchOn& face = registry.torch_on_animation.get(animated_entity);
				// No need to check if render request since should always have a render request
				RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
				face.current_cell++;
				if (face.current_cell >= face.size) {
					face.current_cell = 0;
				}
				anim_rr.used_texture = face.cells[face.current_cell];
			}
		}
		if (sparkle_slowdown == 0 && registry.sparkle_animation.entities.size() > 0) {
			// also do for dog
			for (Entity& animated_entity : registry.sparkle_animation.entities) {
				AnimatedSparkle& face = registry.sparkle_animation.get(animated_entity);
				// No need to check if render request since should always have a render request
				RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
				face.current_cell++;
				if (face.current_cell >= face.size) {
					face.current_cell = 0;
				}
				anim_rr.used_texture = face.cells[face.current_cell];
			}
		}

		// DO INTRO STUFF LIKE ANIMATION

		if (screen.game_screen == cut_scene_0 || screen.game_screen == cut_scene_1
            || screen.game_screen == cut_scene_2 || screen.game_screen == cut_scene_3
            || screen.game_screen == cut_scene_4 || screen.game_screen == cut_scene_orange
            || screen.game_screen == cut_scene_green || screen.game_screen == cut_scene_purple
            || screen.game_screen == cut_scene_warning || screen.game_screen == cut_scene_too_late
            || screen.game_screen == cut_scene_win0 || screen.game_screen == cut_scene_win1) {
			if (registry.face_animation.entities.size() > 0) {
				for (Entity& animated_entity : registry.face_animation.entities) {
					AnimatedFaceOject& face = registry.face_animation.get(animated_entity);
					// No need to check if render request since should always have a render request
					RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
					face.current_cell++;
					if (face.current_cell >= face.size) {
						face.current_cell = 0;
					}
					anim_rr.used_texture = face.cells[face.current_cell];
				}
			}
			if (registry.dog_face_animation.entities.size() > 0) {
				// also do for dog
				for (Entity& animated_entity : registry.dog_face_animation.entities) {
					AnimatedDogFace& face = registry.dog_face_animation.get(animated_entity);
					// No need to check if render request since should always have a render request
					RenderRequest& anim_rr = registry.renderRequests.get(animated_entity);
					face.current_cell++;
					if (face.current_cell >= face.size) {
						face.current_cell = 0;
					}
					anim_rr.used_texture = face.cells[face.current_cell];
				}
			}


		}

	}
	//const float KEEPER_SPAWN_FREQ = 10.0 * 1000.0;
	//f//loat keeper_spawn_time = 0.0f;
	if (keeper_spawn_time > KEEPER_SPAWN_FREQ && screen.game_screen == game_screen_night) {
		keeper_spawn_time = 0.0;
		vec2 spawn_pos = registry.get_empty_spawn_keeper_location();

		// between 0 and 1
		float rand_num = uniform_dist(rng);
		if (rand_num > (level / max_level) * 0.5)
		{
			createKeeper(renderer, spawn_pos);
		}
		else
		{
			createFloatingKeeper(renderer, spawn_pos);
		}
	}
	
	if ( registry.rotators.size() > 0) {
		//const float degrees = 360.0;
		const float full_spin = M_PI * 2.0;
		Rotator& rotation = registry.rotators.components[0];
		rotation.angle += (elapsed_ms_since_last_update / NIGHT_DURATION) * full_spin;
		if (!morning_warning && registry.nightTimer.size() > 0 && registry.nightTimer.components[0].counter_ms < 2000.0) {
			morning_warning = true;
			Entity morning = Entity();
			registry.textRenderRequests.insert(morning, { "Good Morning!", vec2(0.32 * window_width_px, 0.64 * window_height_px) });
		}
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

    ///  put crafting list back
    Motion& crafting_motion = registry.motions.get(craftingList);
    Immobile& crafting_immobile = registry.immobiles.get(craftingList);
    vec2 crafting_pos = vec2(window_width_px*0.5, window_height_px*0.5);
    crafting_motion.position = crafting_pos;
    crafting_motion.camera_position = crafting_pos;
    crafting_immobile.position = crafting_pos;
    crafting_immobile.camera_position = crafting_pos;

	if ( screen.game_screen == game_screen_win) {
		Motion& mot = registry.motions.get(win_dance);
		mot.position = { 550.0, 550.0 + 1.0*(animation_cycle % 2) };
		RenderRequest& render_request = registry.renderRequests.get(win_dance);
		TEXTURE_ASSET_ID animation_frame = (TEXTURE_ASSET_ID) (  (int)TEXTURE_ASSET_ID::WIN0 + (animation_cycle % 2) );
		render_request.used_texture = animation_frame;
	}

    ai.step();
//    Entity p = registry.players.entities[0];
    Motion& p_motion = registry.motions.get(player);

    // Displaying health bar
    auto healthBG = createLine(vec2(250, 60), vec2(408, 48));
    registry.colors.insert(healthBG, { 0, 0, 0 });

    auto healthBar = createLine(vec2(50 + 2 * player_component.health, 60), vec2(4*player_component.health, 40));
    registry.colors.insert(healthBar, { 1, 0, 0 });

    if (screen.game_screen == game_screen_night) {
        // inventory bar position on screen
        vec2 pos = {720, 60};
        createInventory(pos);

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
		//createShadowOverlay(renderer);

		if (num_of_item_on_screen < NUM_POTIONS) {
			std::vector<int> randomizer = { 0,1,2,3,4,5,6,7,8 };
			for (int i = 0; i < 5; i++) {
				Item& item = registry.inventory.get(player).holding[i];
				switch (item) {
				case (magical_mushroom):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 0), randomizer.end());
					break;
				case (magical_shell):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 1), randomizer.end());
					break;
				case (magical_strawberry):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 2), randomizer.end());
					break;
				case (magical_grapes):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 3), randomizer.end());
					break;
				case (magical_apple):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 4), randomizer.end());
					break;
				case (red_lotus):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 5), randomizer.end());
					break;
				case (white_lotus):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 6), randomizer.end());
					break;
				case (magical_carrot):
					randomizer.erase(std::remove(randomizer.begin(), randomizer.end(), 7), randomizer.end());
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

		if (registry.torches.entities.size() > 0) {
			Entity torch_entity = registry.torches.entities[0];
			float torch_x_offset = -30.0;
			float torch_y_offset = -5.0;
			float torch_x_base = window_width_px * 0.5;
			float torch_y_base = window_height_px * 0.5;
			// animate torch
			Motion& player_motion = registry.motions.get(player);
			UIComp& torch_ui = registry.uicomps.get(torch_entity);
			RenderRequest& player_rr = registry.renderRequests.get(player);
			RenderRequest& torch_rr = registry.renderRequests.get(torch_entity);
			torch_rr.used_texture = (TEXTURE_ASSET_ID)((int)TEXTURE_ASSET_ID::TORCH_00 + (animation_cycle % 3));

			if (player_rr.used_texture == TEXTURE_ASSET_ID::PLAYER_CHAR2) {
				//torch_y_offset = 0.0; //+= 5.0;
				torch_x_offset -= 5.0;
			} else if (player_rr.used_texture == TEXTURE_ASSET_ID::PLAYER_CHAR3) {
				torch_y_offset -= 10.0;
				torch_x_offset -= 5.0;
			} else if (player_rr.used_texture == TEXTURE_ASSET_ID::PLAYER_CHAR4) {
				torch_y_offset -= 10.0;
				//torch_x_offset += 5.0;
			} else if (player_rr.used_texture == TEXTURE_ASSET_ID::PLAYER_CHAR5) {
				torch_y_offset -= 15.0;
				torch_x_offset -= 5.0;
			} else {
				torch_y_offset -= 2.0;
				torch_x_offset += 2.0;
			}

			if (player_motion.scale.x > 0) {
				// standard direction
				torch_ui.position.x = torch_x_base + torch_x_offset;
				torch_ui.position.y = torch_y_base + torch_y_offset;
				if (torch_ui.scale.x < 0) {
					torch_ui.scale.x *= -1.0;
				}
			} else {
				// other direction
				torch_x_offset *= -1;
				torch_ui.position.x = torch_x_base + torch_x_offset;
				torch_ui.position.y = torch_y_base + torch_y_offset;
				if (torch_ui.scale.x > 0) {
					torch_ui.scale.x *= -1.0;
				}
				
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
//			game_paused = true;
            createCraftingUI({0.5 * window_width_px, 0.5 * window_height_px});
            for (int i = 0; i < registry.inventory.get(player).inUI.size(); i++) {
                Item item = registry.inventory.get(player).inUI[i];
                Entity e = createInventoryPotion(item, { pos.x - 492 + (137 * i), pos.y + 137});
                registry.immobiles.get(e).scale = { 80, 80 };
            }
            checkCrafted();
            vec2 allCrafted = registry.inventory.get(registry.players.entities[0]).crafted;
			if (allCrafted.x == 1) {
				Item item = red_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			} else if (allCrafted.x == 2) {
				Item item = yellow_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			} else if (allCrafted.x == 3) {
				Item item = blue_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			} else if (allCrafted.x == 4) {
				Item item = orange_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			} else if (allCrafted.x == 5) {
				Item item = green_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			} else if (allCrafted.x == 6) {
				Item item = purple_potion;
				Entity e = createInventoryPotion(item, { pos.x - 75, pos.y + 137 });
				registry.immobiles.get(e).scale = { 80, 80 };
			}
            //if (rec1 != allCrafted.end() && rec2 != allCrafted.end() && rec3 != allCrafted.end()) {
			//	screen.game_screen = game_screen_win;
			//	registry.inventory.get(registry.players.entities[0]).crafted = {};
			//	restart_game();
            //}
        }

        if (orange_crafted && green_crafted && purple_crafted) {
            Player& player_component = registry.players.components[0];
            if (player_component.won == 0) {
                // for dog
                createSparkle(renderer, { 890, 590 });
                createSparkle(renderer, { 950, 580 });
                // for potions
                createSparkle(renderer, {850, 660 });
                createSparkle(renderer, {900, 660 });
                createSparkle(renderer, {950, 660 });
//                Entity save = Entity();
//                registry.textRenderRequests.insert(save,{"Go to Sparky to save", {450, 50}});
//                registry.textRenderRequests.get(save).scale = 0.5;
                player_component.won = 1;
            }
            vec2 dog_pos = { 0.85 * window_width_px, 0.75 * window_height_px };
            vec2 player_pos = registry.motions.get(player).position;
            float dis = distance(player_pos, dog_pos);

            if (dis < 60) {
                screen.game_screen = cut_scene_win0;
                restart_game();
            }
            if (dis < 500) {
                float multiplier = 500 / dis;
                for (Entity e : registry.sparkle_animation.entities) {
                    registry.motions.get(e).scale = vec2({ 50*multiplier, 50 * multiplier });
                }
            }
            if (dis > 500) {
                for (Entity e : registry.sparkle_animation.entities) {
                    registry.motions.get(e).scale = vec2({ 50, 50 });
                }
            }
        }
    }

    float min_counter_ms = 1500.f;
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
			// player loses inventory on death
			registry.inventory.components[0].crafted = {};
			registry.inventory.components[0].inUI = {};
			registry.inventory.components[0].index = {};
			registry.inventory.components[0].holding = {};
            player_component.alive = 0;
            restart_game();
			return true;
		}
	}

	for (Entity entity : registry.torches.entities) {
		// progress timer
		Torches& counter = registry.torches.get(entity);
		counter.t_timer -= elapsed_ms_since_last_update;
		if (counter.t_timer > 0.0) {
			screen.torch_timer_ms = counter.t_timer;
		} else {
			screen.torch_timer_ms = 0.0;
		}
		if (counter.t_timer > 0) {
			screen.torch_mode = 1;
		} else {
			screen.torch_mode = 0;
			registry.remove_all_components_of(entity);
			//registry.torches.remove(entity);
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
            while (registry.player_sparkle_animation.entities.size() > 0)
                registry.remove_all_components_of(registry.player_sparkle_animation.entities.back());
            return true;
        }
    }

    // dialogue positons
    vec2 player_line1 = vec2(0.28 * window_width_px, 0.69 * window_height_px);
    vec2 player_line2 = vec2(0.28 * window_width_px, 0.64 * window_height_px);
    vec2 player_line3 = vec2(0.28 * window_width_px, 0.59 * window_height_px);
    vec2 dog_line1 = vec2(0.52 * window_width_px, 0.4 * window_height_px);

    // moving dialogue in cutscenes 0-2
    if (screen.game_screen == cut_scene_0) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(entity, {"Oh no Sparky,", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(entity, {"you're sick!", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(entity, {"woof! woof!", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_1) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(entity,{"I’ll search the forest", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(entity, {"for ingredients for", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(entity, {"some healing potions!", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(entity, {"woof!", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_2) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"An orange, green, and", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"purple potion should", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"do the trick!", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof!", dog_line1});
                } if (counter.timestamp == 5) {
                    vec2 purple_potion_pos = {675, 330};
                    createConversationPotion(renderer, purple_potion_pos, 1);
                }  if (counter.timestamp == 6) {
                    vec2 green_potion_pos = {625, 330 };
                    createConversationPotion(renderer, green_potion_pos, 0);
                }  if (counter.timestamp == 7) {
                    vec2 orange_potion_pos = {575, 330 };
                    createConversationPotion(renderer, orange_potion_pos, 2);
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_3) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"The sun came up!", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"Items are only visible", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"in the moonlight!", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_4) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I don't have everything", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"I need yet. I'll have", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"to go back in", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_orange) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I made the orange", player_line1});
                } if (counter.timestamp == 2) {
                    if (!green_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the green", player_line2});
                    } else if (!purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the purple", player_line2});
                    } else {
                        registry.speechRenderRequests.insert(Entity(), {"potion! That's all", player_line2});
                    }
                } if (counter.timestamp == 3) {
                    if (!green_crafted && !purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"and purple to go!", player_line3});
                    } else if (!green_crafted && purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    } else if (green_crafted && !purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    } else {
                        registry.speechRenderRequests.insert(Entity(), {"of them!", player_line3});
                    }
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_green) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I made the green", player_line1});
                } if (counter.timestamp == 2) {
                    if (!orange_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the orange", player_line2});
                    } else if (!purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the purple", player_line2});
                    } else {
                        registry.speechRenderRequests.insert(Entity(), {"potion! That's all of them", player_line2});
                    }
                } if (counter.timestamp == 3) {
                    if (!orange_crafted && !purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"and purple to go!", player_line3});
                    } else if (!orange_crafted && purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    } else if (orange_crafted && !purple_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    }
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_purple) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I made the purple", player_line1});
                } if (counter.timestamp == 2) {
                    if (!green_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the green", player_line2});
                    } else if (!orange_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"potion! Only the orange", player_line2});
                    } else {
                        registry.speechRenderRequests.insert(Entity(), {"potion! That's all of them", player_line2});
                    }
                } if (counter.timestamp == 3) {
                    if (!green_crafted && !orange_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"and orange to go!", player_line3});
                    } else if (!green_crafted && orange_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    } else if (green_crafted && !orange_crafted) {
                        registry.speechRenderRequests.insert(Entity(), {"left to go!", player_line3});
                    }
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_warning) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"Oh no Sparky, you ", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"look bad... I'm ", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"running out of time", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"arf", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_too_late) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I couldn't do it", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"I'm sorry Sparky", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"rip...", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"...", dog_line1});
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_win0) {
        float speech_counter_ms = 500.f;
        for (Entity entity : registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(),{"I did it Sparky!! I made", player_line1});
                } if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"you an orange, green,", player_line2});
                } if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"and purple potion!", player_line3});
                } if (counter.timestamp == 4) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof!", dog_line1});
                } if (counter.timestamp == 5) {
                    vec2 purple_potion_pos = {375, 425};
                    createConversationPotion(renderer, purple_potion_pos, 1);
                }  if (counter.timestamp == 6) {
                    vec2 green_potion_pos = {350, 425 };
                    createConversationPotion(renderer, green_potion_pos, 0);
                }  if (counter.timestamp == 7) {
                    vec2 orange_potion_pos = {325, 425 };
                    createConversationPotion(renderer, orange_potion_pos, 2);
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
    }
    if (screen.game_screen == cut_scene_win1) {
        float speech_counter_ms = 500.f;
        for (Entity entity: registry.speechTimers.entities) {
            SpeechTimer &counter = registry.speechTimers.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < speech_counter_ms) {
                speech_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                if (counter.timestamp == 1) {
                    registry.speechRenderRequests.insert(Entity(), {"Here Sparky, drink ", player_line1});
                }
                if (counter.timestamp == 2) {
                    registry.speechRenderRequests.insert(Entity(), {"them quick!", player_line2});
                }
                if (counter.timestamp == 3) {
                    registry.speechRenderRequests.insert(Entity(), {"woof woof!", dog_line1});
                } if (counter.timestamp == 5) {
                    vec2 purple_potion_pos = {375, 425};
                    createConversationPotion(renderer, purple_potion_pos, 1);
                }  if (counter.timestamp == 6) {
                    vec2 green_potion_pos = {350, 425 };
                    createConversationPotion(renderer, green_potion_pos, 0);
                }  if (counter.timestamp == 7) {
                    vec2 orange_potion_pos = {325, 425 };
                    createConversationPotion(renderer, orange_potion_pos, 2);
                }
                registry.speechTimers.remove(entity);
                return true;
            }
        }
        // rolling potions
        float potion_counter_ms = 200.f;
        for (Entity entity: registry.permanents.entities) {
            Permanent &counter = registry.permanents.get(entity);
            counter.counter_ms -= elapsed_ms_since_last_update;

            if (counter.counter_ms < potion_counter_ms) {
                potion_counter_ms = counter.counter_ms;
            }
            if (counter.counter_ms < 0) {
                float x_pos = registry.uicomps.get(entity).position.x;
                if (x_pos < 900) {
                    registry.uicomps.get(entity).position.x += 2;
                }
                if (x_pos >= 900) {
                    registry.remove_all_components_of(entity);
                }
            }
        }
    }

    // rolling credits
    float text_counter_ms = 2000.f;
    for (Entity entity : registry.textTimers.entities) {
        TextTimer &counter = registry.textTimers.get(entity);
        counter.counter_ms -= elapsed_ms_since_last_update;

        if (counter.counter_ms < text_counter_ms) {
            text_counter_ms = counter.counter_ms;
        }
        if (counter.counter_ms < 0) {
            // credits
            if (registry.textRenderRequests.has(entity)) {
                float y_pos = registry.textRenderRequests.get(entity).position.y;
                if (y_pos > 1100) {
                    // check if out of bounds
                    registry.remove_all_components_of(entity);
                } else {
                    registry.textRenderRequests.get(entity).position.y += 1;
                }
            }
            // speech
            if (registry.speechRenderRequests.has(entity)) {
                float y_pos = registry.speechRenderRequests.get(entity).position.y;
                if (y_pos > 1100) {
                    // check if out of bounds
                    registry.remove_all_components_of(entity);
                } else {
                    registry.speechRenderRequests.get(entity).position.y += 1;
                }
            }
        }
    }

	return true;
}

void WorldSystem::checkCrafted() {
	std::vector<int> cc = {-1, -1, -1};

	// receipe 1
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == magical_strawberry) {
			cc[0] = 1;
		} else if (i == magical_carrot) {
			cc[1] = 2;
		} else if (i == red_lotus) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 1, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = {-1, -1, -1};

	// receipe 2
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == magical_shell && cc[0] == -1) {
			cc[0] = 1;
		}
		else if (i == magical_shell && cc[0] == 1) {
			cc[1] = 2;
		}
		else if (i == magical_mushroom) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 2, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = { -1, -1, -1 };

	// receipe 3
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == white_lotus) {
			cc[0] = 1;
		}
		else if (i == magical_apple) {
			cc[1] = 2;
		}
		else if (i == magical_mushroom) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 3, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = { -1, -1, -1 };

	// receipe 4
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == magical_shell) {
			cc[0] = 1;
		}
		else if (i == red_lotus) {
			cc[1] = 2;
		}
		else if (i == magical_strawberry) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 4, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = { -1, -1, -1 };

	// receipe 5
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == white_lotus) {
			cc[0] = 1;
		}
		else if (i == magical_shell) {
			cc[1] = 2;
		}
		else if (i == magical_mushroom) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 5, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = { -1, -1, -1 };

	// receipe 6
	for (Item i : registry.inventory.get(registry.players.entities[0]).inUI) {
		if (i == magical_carrot) {
			cc[0] = 1;
		}
		else if (i == magical_apple) {
			cc[1] = 2;
		}
		else if (i == magical_grapes) {
			cc[2] = 3;
		}
	}
	if (cc[0] == 1 && cc[1] == 2 && cc[2] == 3) {
		registry.inventory.get(registry.players.entities[0]).crafted = { 6, registry.inventory.get(registry.players.entities[0]).index[0] };
		registry.inventory.get(registry.players.entities[0]).inUI = {};
		registry.inventory.get(registry.players.entities[0]).index = {};
		return;
	}
	cc = { -1, -1, -1 };

}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	//registry.list_all_components();

	printf("Restarting\n");
	morning_warning = false;
    game_paused = false;
	// Reset the game speed
	current_speed = 1.f;
	num_of_item_on_screen = 0;
	Inventory i = Inventory();
	if (registry.inventory.components.size() > 0) {
		i = registry.inventory.components[0];
	}
	else {
//        i.holding = { torch, nullval, nullval, nullval, nullval };
        i.holding = { magical_shell, red_lotus, magical_strawberry, white_lotus, magical_shell }; // orange
//        i.holding = { torch, white_lotus, magical_shell, magical_muchroom, nullval }; // green
//        i.holding = { torch, magical_carrot, magical_apple, magical_grapes, nullval }; // purple
	}

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
    while (registry.speechRenderRequests.entities.size() > 0)
        registry.remove_all_components_of(registry.speechRenderRequests.entities.back());
    while (registry.speechTimers.entities.size() > 0)
        registry.remove_all_components_of(registry.speechTimers.entities.back());
    while (registry.textTimers.entities.size() > 0)
        registry.remove_all_components_of(registry.textTimers.entities.back());
	while (registry.torches.entities.size() > 0)
		registry.remove_all_components_of(registry.torches.entities.back());
    while (registry.uicomps.entities.size() > 0)
        registry.remove_all_components_of(registry.uicomps.entities.back());
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

	//registry.list_all_components();

	// Create a new player
	float start_x = MAZE_SIZE * TILE_SIZE * 1.5;
	float start_y = MAZE_SIZE * TILE_SIZE * 1.5;

	player = createPlayer(renderer, vec2{0,0}); // TODO: update this after
	// createPlayerSparkle(renderer, vec2{ window_width_px / 2, window_height_px / 2 });
	
	// vec2(start_x, start_y);
	//if (level > 1) {
	//	registry.inventory.insert(player,tempInventory);
	//} else {
	registry.inventory.emplace(player);
	registry.inventory.get(player) = i;

	//registry.inventory.get(player).holding = { nullval, red_lotus, magical_strawberry, magical_carrot, nullval, nullval };
    help = createHelp();
    craftingList = createCraftingList();

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
        createEnd();

        auto over = Entity();
        registry.textRenderRequests.insert(over,
                                           {"Game over", vec2(0.33 * window_width_px, 0.5 * window_height_px)});
        registry.textRenderRequests.get(over).scale = 1.2;
        auto spaceKey = Entity();
        registry.textRenderRequests.insert(spaceKey,
                                           {
                                                   "Press space key to restart", vec2(0.27 * window_width_px, 0.4 * window_height_px) // might need to adjust position
                                           });
        registry.textRenderRequests.get(spaceKey).scale = 0.6;

        Entity end_keeper = createKeeper(renderer, { window_width_px / 2, window_height_px / 2 });
        Motion& keeper_motion = registry.motions.get(end_keeper);
        keeper_motion.position = { window_width_px / 2, window_height_px / 2 };
        keeper_motion.scale *= 4;
        keeper_motion.velocity = {0,0};

        player_component.alive = 0;
    }
    else if (screen.game_screen == game_screen_day || screen.game_screen == game_screen_main_menu) {
        // Playing background music indefinitely
        Mix_PlayMusic(day_music, -1);
       // fprintf(stderr, "Loaded music\n");
		createTutorial();

        Motion& player_motion = registry.motions.get(player);
        player_motion.position = { 0.6 * window_width_px, 0.75 * window_height_px };
        // put player alive again
        player_component.alive = 1;
        player_motion.scale = { 2 * 60, 2 * 80 };
       // printf("created player\n");

        if (screen.game_screen == game_screen_day) {
            createDay();
            createDog({ 0.85 * window_width_px, 0.75 * window_height_px });

            createSign();
            createSign2();

            if (orange_crafted) {
                vec2 orange_potion_pos = {850, 660 };
                Entity orange = createConversationPotion(renderer, orange_potion_pos, 2);
            }

            if (green_crafted) {
                vec2 green_potion_pos = {900, 660 };
                Entity green = createConversationPotion(renderer, green_potion_pos, 0);
            }
            if (purple_crafted) {
                vec2 purple_potion_pos = {950, 660};
                Entity purple = createConversationPotion(renderer, purple_potion_pos, 1);
            }

        } else {
            createIntroScreen();
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
    else if (screen.game_screen == cut_scene_0 || screen.game_screen == cut_scene_1 ||
            screen.game_screen == cut_scene_2 || screen.game_screen == cut_scene_3 ||
            screen.game_screen == cut_scene_4 || screen.game_screen == cut_scene_orange ||
            screen.game_screen == cut_scene_green || screen.game_screen == cut_scene_purple ||
            screen.game_screen == cut_scene_warning || screen.game_screen == cut_scene_too_late ||
            screen.game_screen == cut_scene_win0 || screen.game_screen == cut_scene_win1 ) {
        Entity day = createDay();
        registry.immobiles.get(day).scale = { 1.3 * window_width_px, 1.3 * window_height_px };
        //printf("day entity init %zu\n", day);

        // TODO: move to game_screen_intro
        /// ADD ANIMATED FACE !!
        createPortrait(renderer, vec2(150.0,400.0));
        createDogPortrait(renderer, vec2(900.0, 400.0));

        Entity player_bubble = createSpeechBubble(renderer, {525, 300}, 1);
        Entity dog_bubble = createSpeechBubble(renderer, {680, 470}, 1);
        registry.uicomps.get(dog_bubble).scale = {-260, -70};

        SpeechTimer& line1 = registry.speechTimers.emplace(Entity());
        line1.timestamp = 1;

        SpeechTimer& line2 = registry.speechTimers.emplace(Entity());
        line2.timestamp = 2;
        line2.counter_ms = 1000;

        SpeechTimer& line3 = registry.speechTimers.emplace(Entity());
        line3.timestamp = 3;
        line3.counter_ms = 1500;

        SpeechTimer& line4 = registry.speechTimers.emplace(Entity());
        line4.timestamp = 4;
        line4.counter_ms = 2200;

        auto spaceKey = Entity();
        vec2 spaceKey_line = vec2(0.3 * window_width_px, 0.2 * window_height_px);

        registry.textRenderRequests.insert(spaceKey,{"Press space key to continue!!", spaceKey_line});
        registry.textRenderRequests.get(spaceKey).scale = 0.5;

        if (screen.game_screen == cut_scene_2 || screen.game_screen == cut_scene_win0 ||
            screen.game_screen == cut_scene_win1) {
            SpeechTimer& orange = registry.speechTimers.emplace(Entity());
            orange.timestamp = 5;
            orange.counter_ms = 1500;
            SpeechTimer& green = registry.speechTimers.emplace(Entity());
            green.timestamp = 6;
            green.counter_ms = 1500;
            SpeechTimer& purple = registry.speechTimers.emplace(Entity());
            purple.timestamp = 7;
            purple.counter_ms = 1500;
        }
    }
    else if (screen.game_screen == game_screen_win) {
        //  change music to victory music
        Mix_PlayMusic(day_music, -1);
        //fprintf(stderr, "Loaded music\n");

		// clear save data after we win
		save_state.clearSave();

        createEnd();

		win_dance = createWinDance(renderer, { 0.5 * window_width_px, 0.75 * window_height_px });

        auto win = Entity();
        registry.textTimers.emplace(win);
        registry.textRenderRequests.insert(win,{"You win!", vec2(0.35 * window_width_px, 0.64 * window_height_px)});
        registry.textRenderRequests.get(win).scale = 1.4;
        auto savedDog = Entity();
        registry.textTimers.emplace(savedDog);
        registry.textRenderRequests.insert(savedDog,{"Congratulations! You saved your beloved dog!", vec2(0.12 * window_width_px, 0.56 * window_height_px)});
        registry.textRenderRequests.get(savedDog).scale = 0.6;
        auto spaceKey = Entity();
        registry.textRenderRequests.insert(spaceKey,{"Press space key to play again", vec2(0.335 * window_width_px, 0.12 * window_height_px)});
        registry.textRenderRequests.get(spaceKey).scale = 0.4;

        auto credit1 = Entity();
        registry.textTimers.emplace(credit1);
        registry.speechRenderRequests.insert(credit1, {"Super Awesome Game By", vec2(300, -50)});
        auto credit2 = Entity();
        registry.textTimers.emplace(credit2);
        registry.speechRenderRequests.insert(credit2, {"Madeleine", vec2(385, -130)});
        auto credit3 = Entity();
        registry.textTimers.emplace(credit3);
        registry.speechRenderRequests.insert(credit3, {"Peter", vec2(410, -200)});
        auto credit4 = Entity();
        registry.textTimers.emplace(credit4);
        registry.speechRenderRequests.insert(credit4, {"Polina", vec2(405, -270)});
        auto credit5 = Entity();
        registry.textTimers.emplace(credit5);
        registry.speechRenderRequests.insert(credit5, {"Luka", vec2(415, -340)});
        auto credit6 = Entity();
        registry.textTimers.emplace(credit6);
        registry.speechRenderRequests.insert(credit6, {"Aviral", vec2(405, -410)});
        auto credit7 = Entity();
        registry.textTimers.emplace(credit7);
        registry.speechRenderRequests.insert(credit7, {"thanks for playing!", vec2(305, -500)});
    }
    else if (screen.game_screen == game_screen_night) {
        //printf("in game night screen\n");
        // Playing background music indefinitely
        createNight();

        Mix_PlayMusic(night_music, -1);
        //fprintf(stderr, "Loaded music\n");
        generator->makeNoise(level);
        generator->createWorldMap();
        Motion& p_motion = registry.motions.get(player);
        p_motion.position = registry.get_empty_spawn_keeper_location();



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
		if (level < 9) {
			level++;
			keeper_rate -= 0.8;
			KEEPER_SPAWN_FREQ = keeper_rate * 1000.0;
		}
		for (int k = 0; k < level + 2; k++) {
			vec2 spawn_point = registry.get_empty_spawn_keeper_location();
			printf("KeeperSpawn at %f, %f\n", spawn_point.x, spawn_point.y);
			createKeeper(renderer, spawn_point);
		}
		for (int num_keepers = 0; num_keepers < level; num_keepers++) {
			createKeeper(renderer, registry.get_empty_spawn_location());
		}
		for (Entity& e : registry.enemies.entities) {
			Motion& spawn_point = registry.motions.get(e);
			printf("Keeperat %f, %f\n", spawn_point.position.x, spawn_point.position.y);
		}
    }

	if (screen.game_screen == game_screen_night) 	{
		vec2 player_spawn = registry.get_empty_spawn_keeper_location();
		registry.motions.get(player).position = player_spawn;
		Entity spinner = createSunMoonSpinner(renderer, vec2(window_width_px*0.5, window_height_px));
	}

	registry.list_all_components();

}

void WorldSystem::pause_game() {

	game_paused = true;

	auto entity = Entity();
    registry.pauses.emplace(entity);
	registry.textRenderRequests.insert(entity,
		{
			"PAUSED", vec2(0.4 * window_width_px, 0.5 * window_height_px) // might need to adjust position
		});
	auto entity2 = Entity();
	registry.pauses.emplace(entity2);
	registry.textRenderRequests.insert(entity2,
		{
			"Press P to unpause", vec2(0.38 * window_width_px, 0.5 * window_height_px - 30.f), 0.5f
		});
	for (Motion& motion : registry.motions.components) {
		motion.velocity = { 0, 0 };
	}
}

void WorldSystem::resume_game() {
	
	game_paused = false;
	// Remove "Paused" text 
	while (registry.pauses.size() > 0) {
		registry.remove_all_components_of(registry.pauses.entities[0]);
	}
}

// Compute collisions between entities
void WorldSystem::handle_collisions(float elapsed_ms) {
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

			// Checking Player - Enemy collisions
			if (registry.enemies.has(entity_other)) {
				Enemy& e = registry.enemies.get(entity_other);
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity) && !registry.invincibles.has(entity) && player_component.alive == 1) {
					Motion& enemy_motion = registry.motions.get(entity_other);
					p.health = p.health - e.attack;
					if (p.health > 0) {
						//p.health = p.health - e.attack;
                        p.hit = 1;
                        // make player temporarily invincible
                        if (!registry.invincibles.has(entity))
                        {
                            registry.invincibles.emplace(entity);
                        }

                        if (!registry.hitTimers.has(entity))
                        {
                            registry.hitTimers.emplace(entity);
                            createPlayerSparkle(renderer, registry.motions.get(player).camera_position);
                        }

						// Move enemy slightly back after collision (so that player doesn't die instantly)
						enemy_motion.position = enemy_motion.position + (enemy_motion.velocity * vec2(-0.5, -0.5));
						Mix_PlayChannel(-1, player_hit_sound, 0);
					}
					else {
						p.health = 0;
						registry.deathTimers.emplace(entity);
						// player should stop moving when health is <= 0
						registry.motions.get(player).velocity = { 0, 0 };
						Mix_PlayChannel(-1, player_death_sound, 0);

						// clear the save file if the player died
						save_state.clearSave();
					}
				}
			
			}
			// Checking Player - Enemy collisions
			else if (registry.floating_enemies.has(entity_other)) {
				FloatingEnemy& e = registry.floating_enemies.get(entity_other);
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity) && !registry.invincibles.has(entity) && player_component.alive == 1) {
					Motion& enemy_motion = registry.motions.get(entity_other);
					p.health = p.health - e.attack;
					if (p.health > 0) {
						//p.health = p.health - e.attack;
						p.hit = 1;
						// make player temporarily invincible
						if (!registry.invincibles.has(entity))
						{
							registry.invincibles.emplace(entity);
						}

						if (!registry.hitTimers.has(entity))
						{
							registry.hitTimers.emplace(entity);
							createPlayerSparkle(renderer, registry.motions.get(player).camera_position);
						}

						// Move enemy slightly back after collision (so that player doesn't die instantly)
						enemy_motion.position = enemy_motion.position + (enemy_motion.velocity * vec2(-0.5, -0.5));
						Mix_PlayChannel(-1, player_hit_sound, 0);
					}
					else {
						p.health = 0;
						registry.deathTimers.emplace(entity);
						// player should stop moving when health is <= 0
						registry.motions.get(player).velocity = { 0, 0 };
						Mix_PlayChannel(-1, player_death_sound, 0);

						// clear the save file if the player died
						save_state.clearSave();
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
					ScreenState& screen = registry.screenStates.components[0];

					// only play sound during night
					if (screen.game_screen == game_screen_night) {
						Mix_PlayChannel(-1, inventory_full_sound, 0);
					}
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();

    // handle boundaries during daytime
    ScreenState& screen = registry.screenStates.components[0];
    if (screen.game_screen == game_screen_day) {
		//resume_game();  // ?? what was this for?
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
			for (int i = 0; i < registry.inventory.get(registry.players.entities[0]).index.size(); i++) {
				registry.inventory.get(registry.players.entities[0]).holding[registry.inventory.get(registry.players.entities[0]).index[i]] = registry.inventory.get(registry.players.entities[0]).inUI[i];
			}
			registry.inventory.get(registry.players.entities[0]).index = {};
			registry.inventory.get(registry.players.entities[0]).inUI = {};
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
    HelpScreen& help_screen = registry.helpScreen.components[0];
    HelpScreen& craft_screen = registry.helpScreen.components[1];
    ScreenState& screen = registry.screenStates.components[0];

	if (action == GLFW_RELEASE && key == GLFW_KEY_F1) {
		GameState state = {
			registry.inventory.get(registry.players.entities[0]).holding,
			orange_crafted,
			green_crafted,
			purple_crafted,
			level
		};
		save_state.saveGame(state);
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_F2) {
		GameState state = save_state.loadGame();
		registry.inventory.get(registry.players.entities[0]).holding = state.player_inventory;
		orange_crafted = state.orange_crafted;
		green_crafted = state.green_crafted;
		purple_crafted = state.purple_crafted;
		level = state.level;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_F3) {
		save_state.clearSave();
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_L && is_in_debug) {
		chunk_border = !chunk_border;
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

	if (action == GLFW_RELEASE && key == GLFW_KEY_M && is_in_debug) {
		//std::cout << "in here!" << std::endl;
		while (registry.enemies.entities.size() > 0)
			registry.remove_all_components_of(registry.enemies.entities.back());
	}

	// Exiting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		if (registry.inventory.components[0].is_open) {
			registry.inventory.components[0].is_open = false;
			game_paused = false;
		} else if (help_screen.help == 1) {
            help_screen.help = 0;
            // craft screen == 2 means its on standby for when help closes, so dont unpause
            if (craft_screen.help == 2) {
                craft_screen.help = 1;
            } else if (!already_paused) {
                game_paused = false;
            }
        } else if (craft_screen.craftingList == 1) {
            craft_screen.craftingList = 0;
            // help screen == 2 means its on standby for when crafting closes, so dont unpause
            if (help_screen.help == 2) {
                help_screen.help = 1;
            } else if (!already_paused) {
                game_paused = false;
            }
        }
		else if(!registry.tutorialSequence.components[0].visible) {
			std::cout << "EXIT GAME" << std::endl;
			glfwSetWindowShouldClose(window, true);
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R && is_in_debug) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		resume_game(); // this removed paused text
        restart_game();
	}

	// Pausing game
	if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
		if (!game_paused) {
            already_paused = true;
			pause_game();
		}
		else {
            already_paused = false;
			resume_game();
		}
	}

    // Changing game scene to day
    if (action == GLFW_RELEASE && key == GLFW_KEY_N && is_in_debug) {
        // printf("N key pressed, switching screen");
		for (int i = 0; i < registry.inventory.get(registry.players.entities[0]).index.size(); i++) {
			registry.inventory.get(registry.players.entities[0]).holding[registry.inventory.get(registry.players.entities[0]).index[i]] = registry.inventory.get(registry.players.entities[0]).inUI[i];
		}
		registry.inventory.get(registry.players.entities[0]).index = {};
		registry.inventory.get(registry.players.entities[0]).inUI = {};
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
			if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_mushroom) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 5;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_apple) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 5;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_shell) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 10;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == red_lotus) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 10;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_strawberry) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 15;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == white_lotus) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 15;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_grapes) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 20;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == magical_carrot) {
				int new_health = registry.players.get(registry.players.entities[0]).health + 20;
				registry.players.get(registry.players.entities[0]).health = min(new_health, 100);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == red_potion) {
				registry.players.get(registry.players.entities[0]).health = 100;
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == yellow_potion) {
				// do yellowpotion stuff
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == blue_potion) {
				// do blue potion stuff
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			} else if (registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] == torch) {
				Entity torch_entity = Entity();
				registry.torches.emplace(torch_entity);
				UIComp& torch_ui = registry.uicomps.emplace(torch_entity);
				torch_ui.position = vec2(window_width_px*0.5,window_height_px*0.5);
				torch_ui.scale = vec2(PLAYER_WIDTH * 0.5, PLAYER_WIDTH * 0.5);
				RenderRequest& torch_rr = registry.renderRequests.insert(torch_entity, { TEXTURE_ASSET_ID::TORCH_00, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
				
                Mix_PlayChannel(-1, torch_sound, 0);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			}
		}
		else {
			if (registry.inventory.get(registry.players.entities[0]).inUI.size() < 3 && registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] != nullval) {
				registry.inventory.get(registry.players.entities[0]).inUI.push_back(registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state]);
				registry.inventory.get(registry.players.entities[0]).index.push_back(selected_inv_state);
				registry.inventory.get(registry.players.entities[0]).holding[selected_inv_state] = nullval;
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_U && registry.screenStates.components[0].game_screen == game_screen_day) {
		if (registry.inventory.get(registry.players.entities[0]).is_open) {
			vec2 craft = registry.inventory.get(registry.players.entities[0]).crafted;
			if (craft.x == 1) {
				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = red_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
			} else if (craft.x == 2) {
				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = yellow_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
			} else if (craft.x == 3) {
				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = blue_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
			} else if (craft.x == 4) {
//				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = orange_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
//                vec2 orange_potion_pos = {850, 660 };
//                Entity orange = createConversationPotion(renderer, orange_potion_pos, 2);
                orange_crafted = true;
                screen.game_screen = cut_scene_orange;
                restart_game();
			} else if (craft.x == 5) {
//				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = green_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
//                vec2 green_potion_pos = {900, 660 };
//                Entity green = createConversationPotion(renderer, green_potion_pos, 0);
                green_crafted = true;
                screen.game_screen = cut_scene_green;
                restart_game();
			} else if (craft.x == 6) {
//				registry.inventory.get(registry.players.entities[0]).holding[craft.y] = purple_potion;
				registry.inventory.get(registry.players.entities[0]).crafted = {};
//                vec2 purple_potion_pos = {950, 660};
//                Entity purple = createConversationPotion(renderer, purple_potion_pos, 1);
                purple_crafted = true;
                screen.game_screen = cut_scene_purple;
                restart_game();
			}
		}
		registry.inventory.get(registry.players.entities[0]).is_open = !registry.inventory.get(registry.players.entities[0]).is_open;
		for (int i = 0; i < registry.inventory.get(registry.players.entities[0]).index.size(); i++) {
			registry.inventory.get(registry.players.entities[0]).holding[registry.inventory.get(registry.players.entities[0]).index[i]] = registry.inventory.get(registry.players.entities[0]).inUI[i];
		}
		registry.inventory.get(registry.players.entities[0]).index = {};
		registry.inventory.get(registry.players.entities[0]).inUI = {};
	}

    // Opening help screen
    if (action == GLFW_RELEASE && key == GLFW_KEY_H) {
		if (registry.helpScreen.components.size() >0) {

			if (help_screen.help == 1) {
				help_screen.help = 0;

                // craft screen == 2 means its on standby for when help closes, so dont unpause
                if (craft_screen.help == 2) {
                    craft_screen.help = 1;
                } else if (!already_paused) {
                    game_paused = false;
                }
            }
            else if (!already_paused) {
                help_screen.help = 1;
                game_paused = true;
                for (Motion& motion : registry.motions.components) {
                    motion.velocity = { 0, 0 };
                }

                if (craft_screen.help == 1) {
                    craft_screen.help = 2;
                }
            }
		}
    }

    // Opening crafting screen at night
    if (action == GLFW_RELEASE && key == GLFW_KEY_U && screen.game_screen == game_screen_night) {
        if (registry.helpScreen.components.size() > 1) {
            if (craft_screen.craftingList == 1) {
                craft_screen.craftingList = 0;

                // help screen == 2 means its on standby for when crafting closes, so dont unpause
                if (help_screen.help == 2) {
                    help_screen.help = 1;
                } else if (!already_paused) {
                    game_paused = false;
                }
            } else if (!already_paused) {
                craft_screen.craftingList = 1;
                game_paused = true;
                for (Motion& motion : registry.motions.components) {
                    motion.velocity = { 0, 0 };
                }
                if (help_screen.help == 1) {
                    help_screen.help = 2;
                }
            }
        }
    }

	// Torch Chnage this to %3 if adding new type of torch or need alternate night shader functionality
	if (key == GLFW_KEY_T && action == GLFW_RELEASE && screen.game_screen == game_screen_night && is_in_debug) {
		screen.torch_mode = (screen.torch_mode + 1) %2;
		//if (torch_timer_ms == 0.0 && screen.torch_mode == 1) {
		//	//torch_timer_ms = TORCH_LENGTH;
		//}

	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_B) {
		is_in_debug = !is_in_debug;
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
			if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
				if (action == GLFW_PRESS) {
					go_up_key_pressed = true;
				}
				else if (action == GLFW_RELEASE) {
					go_up_key_pressed = false;
				}
			}

			if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
				if (action == GLFW_PRESS) {
					go_down_key_pressed = true;
				}
				else if (action == GLFW_RELEASE) {
					go_down_key_pressed = false;
				}
			}

			if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
				if (action == GLFW_PRESS) {
					go_right_key_pressed = true;
				}
				else if (action == GLFW_RELEASE) {
					go_right_key_pressed = false;
				}
			}

			if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
				if (action == GLFW_PRESS) {
					go_left_key_pressed = true;
				}
				else if (action == GLFW_RELEASE) {
					go_left_key_pressed = false;
				}
			}


			if (go_up_key_pressed && !go_down_key_pressed) {
				playerDirection = UP;
				moving_y = true;
				motion.velocity.y -= CHARACTER_SPEED;
			}
			else if (!go_up_key_pressed && go_down_key_pressed) {
				playerDirection = DOWN;
				moving_y = true;
				motion.velocity.y += CHARACTER_SPEED;
			}
			else {
				moving_y = false;
				motion.velocity.y = 0.f;
			}


			if (go_right_key_pressed && !go_left_key_pressed) {
				playerDirection = RIGHT;
				moving_x = true;
				motion.velocity.x += CHARACTER_SPEED;
				if (screen.game_screen == game_screen_day) {
					motion.scale = vec2(2 * -PLAYER_WIDTH, 2 * PLAYER_HEIGHT);
				} else {
					motion.scale = vec2(-PLAYER_WIDTH, PLAYER_HEIGHT);
				}
			}
			else if (!go_right_key_pressed && go_left_key_pressed) {
				playerDirection = LEFT;
				moving_x = true;
				motion.velocity.x -= CHARACTER_SPEED;
				if (screen.game_screen == game_screen_day) {
					motion.scale = vec2(2 * PLAYER_WIDTH, 2 * PLAYER_HEIGHT);
				}
				else {
					motion.scale = vec2(PLAYER_WIDTH, PLAYER_HEIGHT);
				}
			}
			else {
				moving_x = false;
				motion.velocity.x = 0.f;
			}
	
			motion.velocity.x = fmax(fmin(motion.velocity.x, CHARACTER_SPEED), -CHARACTER_SPEED);
			motion.velocity.y = fmax(fmin(motion.velocity.y, CHARACTER_SPEED), -CHARACTER_SPEED);
		}

		// For animation
		if (moving_x || moving_y) {
			registry.players.get(player).moving = true;
		}
		else {
			registry.players.get(player).moving = false;
			RenderRequest& r = registry.renderRequests.get(player);
			r.used_texture = TEXTURE_ASSET_ID::PLAYER_CHAR1;
		}
	}

	// Opening tutorial
	if (registry.tutorialSequence.entities.size() > 0) {
		TutorialSequence& tutorial = registry.tutorialSequence.components[0];

		if (action == GLFW_RELEASE && key == GLFW_KEY_T && registry.screenStates.components[0].game_screen == game_screen_day) {
			if (!tutorial.visible) {
				printf("T key pressed, opening tutorial screen\n");
				tutorial.visible = true;
				game_paused = true;
				Mix_PlayChannel(-1, page_turn_sound, 0);
			}
			else {
				printf("T key pressed, closing tutorial screen\n");
				tutorial.visible = false;
				game_paused = false;
				Mix_PlayChannel(-1, menu_close_sound, 0);
			}
		}
		if (tutorial.visible && registry.screenStates.components[0].game_screen == game_screen_day) {
			RenderRequest& tut_rr = registry.renderRequests.get(registry.tutorialSequence.entities[0]);

			if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT) {
				if (tutorial.current_page < tutorial.size - 1) {
					tutorial.current_page++;
					tut_rr.used_texture = tutorial.pages[tutorial.current_page];
					Mix_PlayChannel(-1, page_turn_sound, 0);
				}
			}

			if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT) {
				if (tutorial.current_page > 0) {
					tutorial.current_page--;
					tut_rr.used_texture = tutorial.pages[tutorial.current_page];
					Mix_PlayChannel(-1, page_turn_sound, 0);
				}
			}

			if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
				tutorial.current_page = 0;
				tut_rr.used_texture = tutorial.pages[tutorial.current_page];
				tutorial.visible = false;
				game_paused = false;
				Mix_PlayChannel(-1, menu_close_sound, 0);
			}
		}
	}

    // space to start game + cut scenes
    if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
        if (screen.game_screen == cut_scene_purple || screen.game_screen == cut_scene_green ||
            screen.game_screen == cut_scene_orange || screen.game_screen == cut_scene_warning ||
            screen.game_screen == cut_scene_4) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = game_screen_day;
                restart_game();
            }

        }
        if (screen.game_screen == cut_scene_3) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = cut_scene_4;
                restart_game();
            }

        }
        if (screen.game_screen == cut_scene_2) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = game_screen_day;

				// load in save data after opening cutscene
				GameState state = save_state.loadGame();
				registry.inventory.get(registry.players.entities[0]).holding = state.player_inventory;
				orange_crafted = state.orange_crafted;
				green_crafted = state.green_crafted;
				purple_crafted = state.purple_crafted;
				level = state.level;

                restart_game();
                registry.tutorialSequence.components[0].visible = true;
                game_paused = true;
                Mix_PlayChannel(-1, page_turn_sound, 0);
            }

        }
        if (screen.game_screen == cut_scene_1) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = cut_scene_2;
                restart_game();
            }
        }
        if (screen.game_screen == cut_scene_0) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = cut_scene_1;
                restart_game();
            }
        }
        if (screen.game_screen == game_screen_main_menu) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = cut_scene_0;
                restart_game();
            }
        }
        if (screen.game_screen == game_screen_game_over) {
            screen.game_screen = game_screen_main_menu;
            screen.torch_mode = 0;
            screen.night_counter_ms = 0;
            NightTimer& night_counter = registry.nightTimer.components[0];
            night_counter.counter_ms = 0;
            registry.nightTimer.clear();
            while (registry.renderRequests.entities.size() > 0)
                registry.remove_all_components_of(registry.renderRequests.entities.back());
            restart_game();
        }
        if (screen.game_screen == cut_scene_too_late) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = game_screen_game_over;
                restart_game();
            }

        }
        if (screen.game_screen == game_screen_win) {
            screen.game_screen = game_screen_main_menu;
//            screen.torch_mode = 0;
            screen.night_counter_ms = 0;
            registry.nightTimer.clear();
            orange_crafted = false;
            green_crafted = false;
            purple_crafted = false;
            Player& player_component = registry.players.components[0];
            player_component.won = 0;
            while (registry.renderRequests.entities.size() > 0)
                registry.remove_all_components_of(registry.renderRequests.entities.back());
            restart_game();

        }
        if (screen.game_screen == cut_scene_win1) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = game_screen_win;
                restart_game();
            }
        }
        if (screen.game_screen == cut_scene_win0 && orange_crafted && green_crafted && purple_crafted) {
            if (registry.speechTimers.size() > 0) {
                for (SpeechTimer& s: registry.speechTimers.components) {
                    s.counter_ms = 0;
                }
            } else {
                screen.game_screen = cut_scene_win1;
                restart_game();
            }
        }
    }

    // v to victory screen
   if (action == GLFW_RELEASE && key == GLFW_KEY_V && is_in_debug) {
       if (screen.game_screen != game_screen_night) {
           screen.game_screen = game_screen_day;
           orange_crafted = true;
           green_crafted = true;
           purple_crafted = true;
//           Player& player_component = registry.players.components[0];
//           player_component.won = 0;
//           while (registry.permanents.entities.size() > 0)
//               registry.remove_all_components_of(registry.permanents.entities.back());
           restart_game();
       }
   }

    // Control the current speed with `<` `>`
	//if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
	//	current_speed -= 0.1f;
		//printf("Current speed = %f\n", current_speed);
	//}
	//if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
	//	current_speed += 0.1f;
		//printf("Current speed = %f\n", current_speed);
	//}

	//current_speed = fmax(0.f, current_speed);
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

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE && is_in_debug) {
//		vec2 spawn_pos = registry.get_empty_spawn_location();
//		printf("Spawning object at x %f  y %f\n", spawn_pos.x , spawn_pos.y);
//		createPotion(renderer, camera.adjust_to_player(spawn_pos));
		createFloatingKeeper(renderer, camera.adjust_to_player(vec2{ xpos, ypos }));
		//if (points <= 5) {
		//	createPotion(renderer, camera.adjust_to_player(vec2{ xpos, ypos }));
		//}

	}

}

void WorldSystem::on_scroll(double x_offset, double y_offset)
{
	std::cout << "in scroll function" << std::endl;
	std::cout << "xoffset: " << x_offset << " yoffset: " << y_offset << std::endl;
	Inventory& inventory = registry.inventory.get(registry.players.entities[0]);
	if (y_offset == -1)
	{
		inventory.select_state = (inventory.select_state + 1) % inventory.holding.size();
	}
	else
	{
		inventory.select_state = ( (inventory.select_state + inventory.holding.size()) - 1) % inventory.holding.size();
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
