#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"

using namespace glm;

Entity createCat(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_IDS::CAT);
	registry.meshPtrs.emplace(entity, &mesh);

	//Make player a rigidbody
	registry.rigidBodies.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 100.f, 100.f };


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

	//Adding terrain component
	registry.terrains.emplace(entity);

	//Adding rigidbody component
	Rigidbody& rb = registry.rigidBodies.emplace(entity);

	//Adding a box shaped collider
	Boxcollider& bc = registry.boxColliders.emplace(entity);
	float left = -width / 2;
	float right = width / 2;
	float up = -height / 2;
	float down = height / 2;

	bc.verticies.push_back(pos + vec2{left, up}); //topleft
	bc.verticies.push_back(pos + vec2{right, up}); //topright
	bc.verticies.push_back(pos + vec2{right, down}); //downright
	bc.verticies.push_back(pos + vec2{left, down}); //downleft

	bc.triangles.push_back(0); //topleft
	bc.triangles.push_back(1); //topright
	bc.triangles.push_back(2); //bottomright
	bc.triangles.push_back(0); //topleft
	bc.triangles.push_back(2); //bottomright
	bc.triangles.push_back(3); //bottomleft


	rb.type = STATIC;

	return entity;
}

Entity createAI(RenderSystem* renderer, vec2 pos)
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
	//motion.scale = mesh.original_size * 300.f;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.ais.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::TOTAL, // textureCount indicates that no txture is needed
			SHADER_PROGRAM_IDS::TOTAL,
			GEOMETRY_BUFFER_IDS::TOTAL });

	return entity;
}