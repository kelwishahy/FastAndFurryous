#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "render_system.hpp"
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

	void init(RenderSystem* renderer);

	void static translatePos(Entity e, glm::vec2 translation);

private:
	void checkForCollisions();

	void transformBoxColliders();

	void applyMotions(float elapsed_ms);

	void moveBackEntity(Entity e, glm::vec2 normal, float depth);

	void transformAnchoredEntities();

	const float GRAVITY_CONST = 2.5f;
	const float TERMINAL_VELOCITY = 500.0f;

	RenderSystem* renderer;
};