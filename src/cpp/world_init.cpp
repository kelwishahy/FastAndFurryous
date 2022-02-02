#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"

Entity createCat(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	//Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CAT);
	//registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	//motion.scale = mesh.original_size * 300.f;
	motion.scale.y *= -1; // point front to the right

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.players.emplace(entity);
	//registry.renderRequests.insert(
	//	entity,
	//	{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
	//		EFFECT_ASSET_ID::PLAYER,
	//		GEOMETRY_BUFFER_ID::PLAYER });

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