#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	static Entity castRay(glm::vec2 origin, float direction, float distance, float max_depth);

	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}

private:
	void applyMotions(float elapsed_ms);

	const float GRAVITY_CONST = 0.5f;
};