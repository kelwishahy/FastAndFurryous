#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

#include "hpp/common.hpp"

#include "hpp/ANIMATION_CONSTANTS.hpp"

using namespace glm;

void calculateBoxVerticesAndSetTriangles(vec2 pos, vec2 scale, Boxcollider& box) {
	float left = -scale.x / 2;
	float right = scale.x / 2;
	float up = -scale.y / 2;
	float down = scale.y / 2;

	box.vertices.push_back(pos + vec2{ left, up }); //topleft
	box.vertices.push_back(pos + vec2{ right, up }); //topright
	box.vertices.push_back(pos + vec2{ right, down }); //downright
	box.vertices.push_back(pos + vec2{ left, down }); //downleft
}

Entity createCat(RenderSystem* renderer, vec2 pos) {

  auto head = Entity();
	auto entity = Entity();

	//---Head animation subentity---- putting this in front so that head gets rendered ontop of body
	AnimationExtra& headbone = registry.animExtras.emplace(head);
	headbone.parent = entity;
	//This is hardcoded and theres not much we can do to change that
	headbone.offset_from_parent = { 9.5f, -41.0f };
	headbone.tag = "cat_head";

	Motion& headmotion = registry.motions.emplace(head);
	headmotion.position = pos + headbone.offset_from_parent;
	headmotion.scale = { 64.f, 64.f * 0.72803f}; //Look at the dimensions of the sprite sheet to get the right ratio

	Animation& headanim = registry.animations.emplace(head);
	headanim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_BLINK, CAT_FRONT_BLINK_CONSTANTS });
	headanim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_BLINK, CAT_SIDE_BLINK_CONSTANTS });
	headanim.anim_state = TEXTURE_IDS::CAT_FRONT_BLINK;

	registry.renderRequests.insert(
		head,
		{ TEXTURE_IDS::CAT_FRONT_BLINK,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
	//----------------------------------------------

	// Add health component
	Health& health = registry.health.emplace(entity);

	//Make player a rigidbody
	Rigidbody& rb = registry.rigidBodies.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((64.f / defaultResolution.x) * renderer->getScreenWidth());
	motion.scale = { scale, scale * 1.655f }; //Look at the dimensions of the sprite sheet to get the right ratio

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	auto& player = registry.players.emplace(entity);
	player.team = PLAYER_1_TEAM;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT_FRONT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	registry.weapons.insert(entity, Rifle());

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({TEXTURE_IDS::CAT_FRONT_IDLE, CAT_IDLE_CONSTANTS});
	anim.animation_states_constants.insert({TEXTURE_IDS::CAT_WALK, CAT_WALK_CONSTANTS });

	return entity;
}

Entity createWall(vec2 pos, float width, float height) {
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
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	// registry.renderRequests.insert(
	// 	entity,
	// 	{ TEXTURE_IDS::TOTAL,
	// 		SHADER_PROGRAM_IDS::WALL,
	// 		GEOMETRY_BUFFER_IDS::QUAD });

	return entity;
}

Entity createTile(float tileScale, vec2 tilePosition, int numTilesInARow) {
	vec2 position = { ((tilePosition.y + 1) * tileScale) + ((tileScale * numTilesInARow) / 2.0), tilePosition.x * tileScale + (tileScale / 2.0)};
	return createWall(position, tileScale * numTilesInARow, tileScale);
}

Entity createAI(RenderSystem* renderer, vec2 pos) {
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
	float scale = ceil((64.f / defaultResolution.x) * renderer->getScreenWidth());
	motion.scale = { scale, scale };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	auto& player = registry.players.emplace(entity);
	player.team = NPC_AI_TEAM;
	registry.ais.emplace(entity);
	registry.weapons.insert(entity, Rifle());
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT_FRONT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_IDLE, CAT_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_WALK, CAT_WALK_CONSTANTS });
	// Add a behaviour tree

	return entity;
}

Entity createProjectile(RenderSystem* renderer, Entity originE, vec4 coefficientsX, vec4 coefficientsY, vec2 endtangent) {

	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = registry.motions.get(originE).position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((10.f / defaultResolution.x) * renderer->getScreenWidth());
	motion.scale = { scale, scale };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	//Using wall textures for now
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::TOTAL, 
			SHADER_PROGRAM_IDS::WALL,
			GEOMETRY_BUFFER_IDS::QUAD });

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.origin = originE;
	projectile.trajectoryAx = coefficientsX;
	projectile.trajectoryAy = coefficientsY;
	projectile.end_tangent = endtangent;

	return entity;
}

Entity createMenu(MENU_TYPES menu, float layer) {

	auto entity = Entity();

	MenuItem& menu_comp =  registry.menus.emplace(entity);
	RenderRequest request;
	
	if (menu == MENU_TYPES::START) {
		request = { TEXTURE_IDS::START_MENU,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD };
	}

	//selectscreen
	if (menu == MENU_TYPES::SELECT) {
		request = { TEXTURE_IDS::SELECT_MENU,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD };
	}
	menu_comp.layer = layer;
	menu_comp.request = request;

	registry.renderRequests.insert(
		entity,
		request
	);

	return entity;
}

Entity createButton(vec2 pos, vec2 scale, TEXTURE_IDS tex_id, std::vector<std::function<void()>> callbacks) {

	auto entity = Entity();

	Clickable& button = registry.buttons.emplace(entity);
	button.callbacks = callbacks;

	float left = -scale.x / 2;
	float right = scale.x / 2;
	float up = -scale.y / 2;
	float down = scale.y / 2;

	button.vertecies.push_back(pos + vec2{ left, up }); //topleft
	button.vertecies.push_back(pos + vec2{ right, up }); //topright
	button.vertecies.push_back(pos + vec2{ right, down }); //downright
	button.vertecies.push_back(pos + vec2{ left, down }); //downleft

	registry.renderRequests.insert(
		entity,
		{ tex_id,
			SHADER_PROGRAM_IDS::TEXTURE,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };

	return entity;

}

Entity createText(vec2 pos, float scale, glm::vec3 color, std::string text) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { scale, scale };

	Text& textfield = registry.texts.emplace(entity);
	textfield.text = text;
	textfield.color = color;

	registry.renderRequests.insert(
		entity,
		{	TEXTURE_IDS::TOTAL,
			SHADER_PROGRAM_IDS::FONT,
			GEOMETRY_BUFFER_IDS::TOTAL });

	return entity;
}