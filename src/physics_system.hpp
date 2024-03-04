#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem {
private:
	Entity* compass_entity;
	Immobile* compass_immobile;

public:
	void step(float elapsed_ms);

	PhysicsSystem() {}

	//PhysicsSystem(Entity* entity, Immobile* immobile) {
	//	compass_entity = entity;
	//	compass_immobile = immobile;
	//}

	void setCompass(Entity* entity, Immobile* immobile);

};