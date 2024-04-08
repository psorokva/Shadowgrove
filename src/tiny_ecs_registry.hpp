#pragma once
#include <vector>
#include <iostream>

#include "tiny_ecs.hpp"
#include "components.hpp"

inline int get_chunk_value(MazeLocation location);

class ECSRegistry {
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<NightTimer> nightTimer;
	ComponentContainer<Motion> motions;
	ComponentContainer<Immobile> immobiles;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Healer> healers;
	ComponentContainer<Escaper> escapers;
    ComponentContainer<Deadly> deadlys;
	ComponentContainer<Interactable> interactables;
	ComponentContainer<Tile> tiles;
	ComponentContainer<Impenetrable> impenetrables;
	ComponentContainer<Chunk> chunks;
	ComponentContainer<Inventory> inventory;
    ComponentContainer<HelpScreen> helpScreen;
    ComponentContainer<HitTimer> hitTimers;
    ComponentContainer<SpeechTimer> speechTimers;
    ComponentContainer<TextTimer> textTimers;
    ComponentContainer<Invincible> invincibles;
	ComponentContainer<TextRenderRequest> textRenderRequests;
    ComponentContainer<SpeechRenderRequest> speechRenderRequests;
    ComponentContainer<Daytime> daytimes;
	ComponentContainer<Torches> torches;
    ComponentContainer<Pause> pauses;
	ComponentContainer<UIComp> uicomps;
	ComponentContainer<AnimatedFaceOject> face_animation;
	ComponentContainer<AnimatedDogFace> dog_face_animation;
	ComponentContainer<TutorialSequence> tutorialSequence;
	ComponentContainer<AnimatedSparkle> sparkle_animation;
	ComponentContainer<AnimatedPlayerSparkle> player_sparkle_animation;
	ComponentContainer<AnimatedTorchOn> torch_on_animation;
	ComponentContainer<AnimatedTorchOff> torch_off_animation;
	ComponentContainer<Paired> pairs;
	ComponentContainer<Rotator> rotators;
    ComponentContainer<Permanent> permanents;
	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry() {
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&nightTimer);
		registry_list.push_back(&motions);
		registry_list.push_back(&immobiles);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&enemies);
		registry_list.push_back(&healers);
		registry_list.push_back(&escapers);
        registry_list.push_back(&deadlys);
		registry_list.push_back(&interactables);
		registry_list.push_back(&chunks);
		registry_list.push_back(&tiles);
		registry_list.push_back(&impenetrables);
		registry_list.push_back(&inventory);
        registry_list.push_back(&helpScreen);
        registry_list.push_back(&hitTimers);
        registry_list.push_back(&speechTimers);
        registry_list.push_back(&textTimers);
        registry_list.push_back(&invincibles);
		registry_list.push_back(&textRenderRequests);
        registry_list.push_back(&speechRenderRequests);
        registry_list.push_back(&daytimes);
		registry_list.push_back(&torches);
        registry_list.push_back(&pauses);
		registry_list.push_back(&uicomps);
		registry_list.push_back(&face_animation); 
		registry_list.push_back(&dog_face_animation);
		registry_list.push_back(&tutorialSequence);
		registry_list.push_back(&sparkle_animation);
		registry_list.push_back(&player_sparkle_animation);
		registry_list.push_back(&torch_on_animation);
		registry_list.push_back(&torch_off_animation);
		registry_list.push_back(&pairs);
		registry_list.push_back(&rotators);
        registry_list.push_back(&permanents);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		if (pairs.has(e)) {
			Paired& pair = pairs.get(e);
			Entity& other = pair.other;
			//printf("Removing Pair");
			for (ContainerInterface* reg : registry_list) {
				reg->remove(e);
				reg->remove(other);
			}
		} else {
			for (ContainerInterface* reg : registry_list) {
				reg->remove(e);
			}
		}
	}

	// 
	vec2 get_empty_spawn_location() {
		vec2 location;
		int num_chunks = chunks.size();
		if (num_chunks < 1) {
			return vec2(0.0, 0.0);
		}
		Entity& ent = chunks.entities[rand() % num_chunks];
		// if (num_chunks == 1) { ent = chunks.entities[0]; } // no need for [0] because %1 == 0
		int count = 0;
		Chunk& chunk = chunks.get(ent);
		int r = rand() % MAZE_SIZE;
		int c = rand() % MAZE_SIZE;
		while (chunk.grid[r][c] > 0) {
			r = rand() % MAZE_SIZE;
			c = rand() % MAZE_SIZE;
			count++;
			// just in case a bad full chunk
			if (count > 16) {
				count = 0;
				ent = chunks.entities[rand() % num_chunks];
				chunk = chunks.get(ent);
			}
		}
		//Immobile& zero = immobiles.components[0];
		//printf("Desired Chunk x %x y %x \n", chunk.x, chunk.y);
		//printf("Desired Chunk x %f y %f \n", chunk.campos_x, chunk.campos_y);
		//printf("Desired Chunk x %x y %x \n", chunk.x, chunk.y);
		//printf("Zero cam pos x %f y %f \n", zero.camera_position.x, zero.camera_position.y);
		//printf("Zero pos x %f y %f \n", zero.position.x, zero.position.y);
		location = vec2(chunk.x * MAZE_SIZE * TILE_SIZE + c * TILE_SIZE + TILE_SIZE / 2, chunk.y * MAZE_SIZE * TILE_SIZE + r * TILE_SIZE + TILE_SIZE / 2);
		return location;
	}

	vec2 get_empty_spawn_keeper_location() {
		Entity ent;
		Chunk chunk;
		vec2 location;
		int num_chunks = chunks.size();
		if (num_chunks < 1) {
			return vec2(0.0, 0.0);
		}
		bool notfound = true;
		for (int c = 0; c < chunks.entities.size() && notfound; c++ ) {
			Chunk& temp_chunk = chunks.components[c];
			if (temp_chunk.keeper_spawned_here) {
				temp_chunk.keeper_spawned_here = true;
				notfound = false;
				chunk = temp_chunk;
			}
		}
		if (notfound) {
			ent = chunks.entities[rand() % num_chunks];
			chunk = chunks.get(ent);
		}


		// if (num_chunks == 1) { ent = chunks.entities[0]; } // no need for [0] because %1 == 0
		int count = 0;
		//Chunk& chunk = chunks.get(ent);
		int r = rand() % MAZE_SIZE;
		int c = rand() % MAZE_SIZE;
		bool location_found = false;
		while (!location_found) {
			r = rand() % MAZE_SIZE;
			c = rand() % MAZE_SIZE;
			count++;
			int maze_val_below = get_chunk_value(MazeLocation{ chunk.x * MAZE_SIZE + c, chunk.y * MAZE_SIZE + r });
			int maze_val_above = get_chunk_value(MazeLocation{ chunk.x * MAZE_SIZE + c, chunk.y * MAZE_SIZE + r - 1 });

			if (maze_val_below == 0 && maze_val_above == 0)
			{
				location_found = true;
				break;
			}
			//if (chunk.grid[])
			// just in case a bad full chunk
			if (count > 16) {
				count = 0;
				ent = chunks.entities[rand() % num_chunks];
				chunk = chunks.get(ent);
			}
		}
		location = vec2(chunk.x * MAZE_SIZE * TILE_SIZE + c * TILE_SIZE + TILE_SIZE / 2, chunk.y * MAZE_SIZE * TILE_SIZE + r * TILE_SIZE + TILE_SIZE / 2);
		return location;
	}


	// this function doesn't work yet ... trying to figure out difference between mouse clicks and world spacd
	bool is_clear(vec2 location, Immobile* , const Entity& player) {
		//float comp_x = compass_immobile->camera_position.x;
		//float comp_y = compass_immobile->camera_position.y;
		Motion& m = motions.get(player);
		float p_x = m.position.x;
		float p_y = m.position.y;
		
		ChunkLocation* cl = new ChunkLocation(); // + TILE_SIZE/2 ??
		cl->offset_x = ((size_t)((-location.x + p_x  ) / TILE_SIZE) % MAZE_SIZE);
		cl->offset_y = ((size_t)((-location.y + p_y  ) / TILE_SIZE) % MAZE_SIZE);
		cl->index_x = ((size_t)((-location.x + p_x ) / TILE_SIZE) / MAZE_SIZE);
		cl->index_y = ((size_t)((-location.y + p_y  ) / TILE_SIZE) / MAZE_SIZE);
		auto& ccc = chunks.components;
		for (uint i = 0; i < ccc.size(); i++) {
			// if correct chunk
			if (ccc[i].x == cl->index_x && ccc[i].y == cl->index_y) {
				//printf("CLICK: x:%f y:%f\n", location.x, location.y);
				//printf("\tCOMPASS: x:%f y:%f\n", p_x, p_y);
				//printf("\tCHUNK: x:%zu y:%zu\n", cl->index_x, cl->index_y);
				//printf("\tCHUNK CELL: x:%zu y:%zu \n", cl->offset_x, cl->offset_y);
				// if spot clear 
				if ( ccc[i].grid[cl->offset_y][cl->offset_x] == 0 ) {
					return true;
				}
				
			}
		}
		return false;
	}

};

extern ECSRegistry registry;