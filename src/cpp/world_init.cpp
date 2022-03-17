#include "..\hpp\world_init.hpp"
#include "..\hpp\tiny_ecs_registry.hpp"
#include "hpp/ANIMATION_CONSTANTS.hpp"
#include "hpp/common.hpp"
#include "hpp/physics_system.hpp"

using namespace glm;

void calculateBoxVerticesAndSetTriangles(vec2 pos, vec2 scale, Boxcollider& box) {
	const float left = -scale.x / 2;
	const float right = scale.x / 2;
	const float up = -scale.y / 2;
	const float down = scale.y / 2;

	box.vertices.push_back(pos + vec2{ left, up }); //topleft
	box.vertices.push_back(pos + vec2{ right, up }); //topright
	box.vertices.push_back(pos + vec2{ right, down }); //downright
	box.vertices.push_back(pos + vec2{ left, down }); //downleft
}

void remove_children(Entity e) {
	if (registry.parentEntities.has(e)) {
		const ChildEntities children = registry.parentEntities.get(e);

		for (const auto& pair : children.child_data_map) {
			remove_children(pair.second);
			registry.remove_all_components_of(pair.second);
		}
	}

}

Entity createCat(vec2 pos) {
	auto head = Entity();
	auto frontArm = Entity();
	const auto entity = Entity();

	ChildEntities& children = registry.parentEntities.emplace(entity);

	/* Each cat - related entity needs the "selected" component.
	 * This value of this component is passed to the fragment shader and is
	 * used in rendering.
	 */
	registry.selected.emplace(head);
	registry.selected.emplace(entity);

	//---Head animation subentity---- putting this in front so that head gets rendered ontop of body
	// Motion& h_anchor_motion = registry.motions.emplace(head_anchor);
	//h_anchor_motion.position = motion.position;

	//---Head animation subentity---- putting this in front so that head gets rendered ontop of body
	int index = 0;
	children.child_data_map.emplace(index, head);
	children.normal_dists.emplace(index, vec2( 9.5f, -41.0f ));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, vec2(9.5f, -41.0f));


	Motion& headmotion = registry.motions.emplace(head);
	headmotion.position = pos + children.normal_dists.at(0);
	headmotion.scale = { 64.f, 64.f * 0.72803f}; //Look at the dimensions of the sprite sheet to get the right ratio

	Animation& headanim = registry.animations.emplace(head);
	headanim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_BLINK, CAT_FRONT_BLINK_CONSTANTS });
	headanim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_BLINK, CAT_SIDE_BLINK_CONSTANTS });
	headanim.animation_states_constants.insert({ TEXTURE_IDS::CAT_HURT_FACE, CAT_HURT_FACE_CONSTANTS });
	headanim.anim_state = TEXTURE_IDS::CAT_FRONT_BLINK;
	headanim.name = "cat_head";

	registry.renderRequests.insert(
		head,
		{ TEXTURE_IDS::CAT_FRONT_BLINK,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	//---Front arm animation subentity---- putting this in front so that front arm gets rendered ontop of body
	index = 1;
	children.child_data_map.emplace(index, frontArm);
	children.normal_dists.emplace(index, vec2(4.5f, 11.0f));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, vec2(4.5f, 11.0f));

	Motion& frontArmMotion = registry.motions.emplace(frontArm);
	frontArmMotion.position = pos + children.normal_dists.at(1);
	frontArmMotion.scale = { 54.f / 3, 128.f / 3 };

	Animation& frontArmAnim = registry.animations.emplace(frontArm);
	frontArmAnim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_ARM, STABILIZED_CONSTANTS });
	frontArmAnim.anim_state = TEXTURE_IDS::CAT_FRONT_ARM;
	frontArmAnim.name = "cat_front_arm";
	registry.renderRequests.insert(
		frontArm,
		{ TEXTURE_IDS::CAT_FRONT_ARM,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	registry.cats.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((64.f / defaultResolution.x) * screenResolution.x);
	motion.scale = { scale, scale * 1.655f }; //Look at the dimensions of the sprite sheet to get the right ratio

	// Add health component
	Health& health = registry.health.emplace(entity);

	//Make player a rigidbody
	Rigidbody& rb = registry.rigidBodies.emplace(entity);

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::CAT_FRONT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	registry.weapons.insert(entity, Rifle());

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({TEXTURE_IDS::CAT_FRONT_IDLE, CAT_IDLE_CONSTANTS});
	anim.animation_states_constants.insert({TEXTURE_IDS::CAT_WALK, CAT_WALK_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_JUMP, CAT_JUMP_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_HURT, CAT_HURT_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_DEAD, STABILIZED_CONSTANTS });
	anim.anim_state = TEXTURE_IDS::CAT_FRONT_IDLE;
	anim.name = "cat_body";

	return entity;
}

Entity createDog(vec2 pos) {

	auto head = Entity();
	const auto entity = Entity();
	auto frontArm = Entity();
	auto backArm = Entity();

	ChildEntities& children = registry.parentEntities.emplace(entity);
	//---Head animation subentity---- putting this in front so that head gets rendered ontop of body
	int index = 0;
	children.child_data_map.emplace(index, head );
	children.normal_dists.emplace(index, vec2(9.5f, -51.0f));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, vec2(9.5f, -51.0f));


	Motion& headmotion = registry.motions.emplace(head);
	headmotion.position = pos + children.normal_dists.at(0);
	headmotion.scale = { 64.f, 64.f * 0.72803f }; //Look at the dimensions of the sprite sheet to get the right ratio

	Animation& headanim = registry.animations.emplace(head);
	headanim.animation_states_constants.insert({ TEXTURE_IDS::DOG_FRONT_BLINK, CAT_FRONT_BLINK_CONSTANTS });
	headanim.animation_states_constants.insert({ TEXTURE_IDS::DOG_SIDE_BLINK, CAT_SIDE_BLINK_CONSTANTS });
	headanim.animation_states_constants.insert({ TEXTURE_IDS::DOG_HURT_FACE, CAT_HURT_FACE_CONSTANTS });
	headanim.anim_state = TEXTURE_IDS::DOG_FRONT_BLINK;
	headanim.name = "dog_head";

	registry.renderRequests.insert(
		head,
		{ TEXTURE_IDS::DOG_FRONT_BLINK,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	//---Front arm animation subentity---- putting this in front so that front arm gets rendered ontop of body
	index = 1;
	children.child_data_map.emplace(index, frontArm);
	children.normal_dists.emplace(index, vec2(4.5f, 11.0f));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, vec2(4.5f, 11.0f));


	Motion& frontArmMotion = registry.motions.emplace(frontArm);
	frontArmMotion.position = pos + children.normal_dists.at(1);
	frontArmMotion.scale = { 64.f / 3, 138.f / 3 };

	Animation& frontArmAnim = registry.animations.emplace(frontArm);
	frontArmAnim.animation_states_constants.insert({ TEXTURE_IDS::DOG_FRONT_ARM, STABILIZED_CONSTANTS });
	frontArmAnim.anim_state = TEXTURE_IDS::DOG_FRONT_ARM;

	registry.renderRequests.insert(
		frontArm,
		{ TEXTURE_IDS::DOG_FRONT_ARM,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	//---Back arm animation subentity---- putting this in back so that front arm gets rendered on back of body
	//AnchoredEntities& back_arm_anchor = registry.anchors.emplace_with_duplicates(entity);
	//back_arm_anchor.child = backArm;
	//back_arm_anchor.normal_distance = { 4.5f, 11.0f };
	////backArmBone.offset_from_parent = { +30.0f, 0.0f };


	//Motion& backArmMotion = registry.motions.emplace(backArm);
	//backArmMotion.position = pos + back_arm_anchor.normal_distance;
	//backArmMotion.scale = { 64.f / 3, 138.f / 3 };

	//Animation& backArmAnim = registry.animations.emplace(backArm);
	//backArmAnim.animation_states_constants.insert({ TEXTURE_IDS::DOG_BACK_ARM, STABILIZED_CONSTANTS });
	//backArmAnim.anim_state = TEXTURE_IDS::DOG_BACK_ARM;
	//registry.renderRequests.insert(
	//	backArm,
	//	{ TEXTURE_IDS::DOG_BACK_ARM,
	//		SHADER_PROGRAM_IDS::ANIMATION,
	//		GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });
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
	float scale = ceil((64.f / defaultResolution.x) * screenResolution.x);
	motion.scale = { scale, scale * 1.125f }; //Look at the dimensions of the sprite sheet to get the right ratio

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::DOG_FRONT_IDLE,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	registry.weapons.insert(entity, Rifle());

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_FRONT_IDLE, CAT_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_WALK, CAT_WALK_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_JUMP, CAT_JUMP_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_HURT, CAT_HURT_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_DEAD, STABILIZED_CONSTANTS });
	anim.anim_state = TEXTURE_IDS::DOG_FRONT_IDLE;
	anim.name = "dog_body";

	return entity;
}


Entity createWall(vec2 pos, float width, float height) {
	const auto entity = Entity();

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

	return entity;
}

Entity createTile(float tileScale, vec2 tilePosition, int numTilesInARow) {
	const vec2 position = { ((tilePosition.y + 1) * tileScale) + ((tileScale * numTilesInARow) / 2.0), tilePosition.x * tileScale + (tileScale / 2.0)};
	return createWall(position, tileScale * numTilesInARow, tileScale);
}

Entity createAI(vec2 pos) {
	const auto entity = Entity();

	// registry.selected.emplace(head);
	registry.selected.emplace(entity);

	// Add health component
	Health& health = registry.health.emplace(entity);

	//Make player a rigidbody
	Rigidbody& rb = registry.rigidBodies.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((64.f / defaultResolution.x) * screenResolution.x);
	motion.scale = { scale, scale };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

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
	anim.anim_state = TEXTURE_IDS::CAT_FRONT_IDLE;
	// Add a behaviour tree

	return entity;
}

Entity createProjectile(Entity originE, vec2 force) {

	const auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = registry.motions.get(originE).position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((10.f / defaultResolution.x) * screenResolution.x);
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

	Rigidbody& rb = registry.rigidBodies.emplace(entity);
	rb.type = KINEMATIC;

	PhysicsSystem::applyForce(entity, force);


	return entity;
}

Entity createMenu(MENU_TYPES menu, float layer) {

	const auto entity = Entity();

	MenuItem& menu_comp =  registry.menus.emplace(entity);
	RenderRequest request;
	
	if (menu == MENU_TYPES::START) {
		request = { TEXTURE_IDS::START_MENU,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD };
	}

	// character select screen
	if (menu == MENU_TYPES::SELECT) {
		request = { TEXTURE_IDS::SELECT_MENU,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD };
	}

	// option screen
	if (menu == MENU_TYPES::OPTIONS) {
		request = { TEXTURE_IDS::OPTIONS_MENU,
					SHADER_PROGRAM_IDS::TEXTURE,
					GEOMETRY_BUFFER_IDS::TEXTURED_QUAD };
	}
	// level screen
	if (menu == MENU_TYPES::LEVELS) {
		request = { TEXTURE_IDS::LEVELS_MENU,
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

	const auto entity = Entity();

	Clickable& button = registry.buttons.emplace(entity);
	button.callbacks = callbacks;

	const float left = -scale.x / 2;
	const float right = scale.x / 2;
	const float up = -scale.y / 2;
	const float down = scale.y / 2;

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
	const auto entity = Entity();

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

Entity createCrosshair(Entity origin, bool iscat) {

	auto entity = Entity();
	auto crosshair_sub = Entity();

	if (!registry.parentEntities.has(origin)) {
		registry.parentEntities.emplace(origin);
	}

	ChildEntities& children = registry.parentEntities.get(origin);

	// Main entity
	Motion& motion = registry.motions.emplace(entity);
	motion.position = registry.motions.get(origin).position;
	motion.scale = { 1.0f, 1.0f };

	registry.uiElements.insert(
		entity, { UI_ELEMENT::CROSSHAIR }
	);

	int index = children.child_data_map.size();
	children.child_data_map.insert({ index, entity });
	children.normal_dists.insert({ index, vec2(0.0f, 0.0f) });
	children.tags.insert({ index, "crosshair" });


	ChildEntities& crosshair = registry.parentEntities.emplace(entity);
	crosshair.child_data_map.emplace(0,crosshair_sub);
	crosshair.normal_dists.insert({ 0, vec2(200.0f, -200.0f) });
	crosshair.tags.insert({ 0, "crosshair" });
	//Sub entity
	Motion& cross_hair_motion = registry.motions.emplace(crosshair_sub);
	cross_hair_motion.position = motion.position + crosshair.normal_dists.at(0);
	cross_hair_motion.scale = { 70.0f, 70.0f };

	registry.renderRequests.insert(
		crosshair_sub,
		{ iscat ? TEXTURE_IDS::CAT_CROSSHAIR : TEXTURE_IDS::DOG_CROSSHAIR,
				SHADER_PROGRAM_IDS::TEXTURE,
				GEOMETRY_BUFFER_IDS::TEXTURED_QUAD }
	);

	return origin;

}

Entity createHealthCounter(Entity origin, int health) {

	const vec3 color = registry.cats.has(origin) ? vec3{ 0.862f, 0.525f, 0.517f } : vec3{ 0.039, 0.454, 1 };

	const auto entity = Entity();

	HealthBox& healthbox = registry.healthboxes.emplace(entity);
	healthbox.parent = origin;
	healthbox.text = createText({ 0.0f,0.0f }, 1.0f, color, std::to_string(health));

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = { 100.0f, 100.0f };

	return entity;

}
