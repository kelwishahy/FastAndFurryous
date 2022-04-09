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

	void fixed_update();

	PhysicsSystem()
	{
	}

	void init(RenderSystem* renderer);

	void static translatePos(Entity e, glm::vec2 translation);

	void static applyForce(Entity e, glm::vec2 force);

private:
	void checkForCollisions();

	void transformBoxColliders();

	void applyMotions(float elapsed_ms);

	void moveBackEntity(Entity e, glm::vec2 normal, float depth);

	void transformChildedEntities();

	const float GRAVITY_CONST = 2.5f;
	const float TERMINAL_VELOCITY = 400.0f;
	const glm::vec2 GRAVITY_FORCE = { 0.0f, 0.5f };

	float fixed_update_accumulator = 0.0f;
	const float FIXED_UPDATE_STEP = 1000.0f / 60.0f;

	RenderSystem* renderer;
};