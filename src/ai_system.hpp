#pragma once

#include <vector>
#include <map>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include <random>
#include <algorithm>


class AISystem {
public:
	AISystem();
	void step();
private:
	inline bool check_step(const Motion& motion, const Immobile& immobile, const vec2& direction);

	const int enemy_velocity_multipler = 80;
	const int step_size = 30;
	const int frames_till_update = 20;
	std::unordered_map<Direction, vec2> direction_to_velocity = {
		{LEFT, {-enemy_velocity_multipler, 0} },
		{RIGHT, {enemy_velocity_multipler, 0} },
		{UP, {0, -enemy_velocity_multipler} },
		{DOWN, {0, enemy_velocity_multipler} }
	};
	std::default_random_engine rng;
};