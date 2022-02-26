#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

using namespace glm;

void calculateBoxVerteciesAndSetTriangles(vec2 pos, vec2 scale, Boxcollider& box) {
	float left = -scale.x / 2;
	float right = scale.x / 2;
	float up = -scale.y / 2;
	float down = scale.y / 2;

	box.vertices.push_back(pos + vec2{ left, up }); //topleft
	box.vertices.push_back(pos + vec2{ right, up }); //topright
	box.vertices.push_back(pos + vec2{ right, down }); //downright
	box.vertices.push_back(pos + vec2{ left, down }); //downleft

	box.triangles.push_back(2); //topleft
	box.triangles.push_back(0); //topright
	box.triangles.push_back(3); //bottomright
	box.triangles.push_back(2); //topleft
	box.triangles.push_back(1); //bottomright
	box.triangles.push_back(0); //bottomleft
}

Entity createCat(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Add health component
	Health& health = registry.health.emplace(entity);

	//Make player a rigidbody
	Rigidbody& rb = registry.rigidBodies.emplace(entity);
	//rb.type = STATIC;

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 100.f, 100.f };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerteciesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	registry.weapons.insert(entity, Rifle());

	return entity;
}

Entity createWall(vec2 pos, int width, int height) {
	
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
	rb.type = STATIC;

	//Adding a box shaped collider
	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerteciesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::TOTAL,
			SHADER_PROGRAM_IDS::WALL,
			GEOMETRY_BUFFER_IDS::QUAD });

	return entity;
}

Entity createTile(TILE_TYPES type, float tileScale, vec2 tilePosition) {
	// TODO:
	// Each tile entity should only have the components required
	// for collision handling. This includes Rigidbody and boxcollider.
	// The position of a tile can be inferred from the tilemap dimensions and tileScale.

	// auto entity = Entity();

	// Add tile component
	// Tile& tile = registry.tiles.emplace(entity);
	// tile.type = type;
	vec2 position = { (tilePosition.y * tileScale) + (tileScale/2), tilePosition.x * tileScale};
	printf("\nTile position is {%f, %f}]\n", position.x, position.y);


	// //Adding rigidbody component
	// Rigidbody& rb = registry.rigidBodies.emplace(entity);
	// rb.type = STATIC;
	//
	// //Adding a box shaped collider
	// Boxcollider& bc = registry.boxColliders.emplace(entity);
	// calculateBoxVerteciesAndSetTriangles(tile.position, {tileScale, tileScale} , bc);
	// bc.transformed_required = true;
	//
	// return entity;

	return createWall(position, (int)tileScale, (int)tileScale);
}

Entity createAI(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Add health component
	Health& health = registry.health.emplace(entity);

	//Make player a rigidbody
	Rigidbody& rb = registry.rigidBodies.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 60.f, 60.f };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerteciesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	registry.players.emplace(entity);
	registry.ais.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	return entity;
}

Entity createProjectile(RenderSystem* renderer, glm::vec2 pos, glm::vec4 coefficientsX, glm::vec4 coefficientsY) {

	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 5.0f, 5.0f };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerteciesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	//Using wall textures for now
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::TOTAL, 
			SHADER_PROGRAM_IDS::WALL,
			GEOMETRY_BUFFER_IDS::WALL });

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.trajectoryAx = coefficientsX;
	projectile.trajectoryAy = coefficientsY;

	return entity;
}