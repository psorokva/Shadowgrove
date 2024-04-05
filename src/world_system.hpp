#pragma once

// internal
#include "common.hpp"
#include "camera_system.hpp"
#include "ai_system.hpp"
#include "save_state.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "map_generator.hpp"
#include "render_system.hpp"
#include "physics_system.hpp"
// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem {
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer, MapGen* generator, PhysicsSystem* physics);

	// Releases all associated resources
	~WorldSystem();

    static void createInventory(vec2 pos);

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	void checkCrafted();

	// Pause/unpause game
	void pause_game();
	void resume_game();

	// Check for collisions
	void handle_collisions(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mods);
	void draw_chunk_borders();

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;

	// chunk borders
	bool chunk_border;

	// Game state
	SaveState save_state;
	CameraSystem camera;
	AISystem ai;
	RenderSystem* renderer;
	MapGen* generator;
	PhysicsSystem* physics;
	float current_speed;
	Entity player;
	Entity help;
	Entity win_dance;
	Immobile* compass_immobile;
    int inventory;
    Entity inventory1;
	int num_item_on_screen;
	Entity torchEntity;

	// music references
	Mix_Music* night_music;
    Mix_Music* day_music;
	Mix_Chunk* bird_sounds;
    Mix_Chunk* mushroom_sound;
    Mix_Chunk* player_death_sound;
    Mix_Chunk* inventory_full_sound;
	Mix_Chunk* player_hit_sound;
	Mix_Chunk* page_turn_sound;
	Mix_Chunk* menu_close_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
