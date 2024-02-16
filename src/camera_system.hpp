#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class CameraSystem {
public:
	void step();
	vec2 adjust_to_player(vec2 pos);
};