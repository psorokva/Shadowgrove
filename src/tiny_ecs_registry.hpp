#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry {
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<DeathTimer> deathTimers;
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

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry() {
		registry_list.push_back(&deathTimers);
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
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
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
		location = vec2(chunk.offset_x + c * TILE_SIZE, chunk.offset_y + r * TILE_SIZE);
		return location;
	}


};

extern ECSRegistry registry;