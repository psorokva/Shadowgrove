#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem {
public:
	void step();
private:
	const int enemy_velocity_multipler = 80;
};