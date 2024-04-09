#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"


const int inventory_size = 5;
const int potions_size = 3;
const int levels_size = 1;

struct GameState 
{
	std::vector<Item> player_inventory = { Item::torch, Item::nullval, Item::nullval, Item::nullval, Item::nullval };
	bool orange_crafted = false;
	bool green_crafted = false;
	bool purple_crafted = false;
	int level = 0;
};

class SaveState 
{
public:
	void saveGame(GameState state);
	GameState loadGame();
	void clearSave();

private:
	std::string filePath = "../../../data/saveState/saveState.txt";
//    std::string filePath = "/Users/madpenner/Desktop/Team6-Shadowgrove/data/saveState/saveState.txt";
};