#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class SaveState {
public:
	SaveState();

	void saveInventory();
	void loadInventory();

private:
	std::string filePath = "../../../data/saveState/saveState.txt";
	int loaded;
};