#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"

class SaveState {
public:
	void saveInventory();
	void loadInventory(RenderSystem* renderer);
	void clearSave();

private:
	std::string filePath = "../../../data/saveState/saveState.txt";
//    std::string filePath = "/Users/madpenner/Desktop/Team6-Shadowgrove/data/saveState/saveState.txt";
};