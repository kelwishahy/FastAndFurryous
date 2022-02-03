#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"

using namespace glm;

Entity createCat(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_IDS::CAT);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 50.f, 50.f };


	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT, // textureCount indicates that no txture is needed
			SHADER_PROGRAM_IDS::CAT,
			GEOMETRY_BUFFER_IDS::CAT });

	return entity;
}

Entity createWall(RenderSystem* renderer, vec2 pos, int width, int height) {
	
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = {width, height}; 

	registry.terrains.emplace(entity);
	return entity;
}