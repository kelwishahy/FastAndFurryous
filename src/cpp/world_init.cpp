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


Entity createCat(WEAPON_TYPES weapon, vec2 pos, int health, GLFWwindow* window) {
	auto head = Entity();
	auto frontArm = Entity();
	const auto entity = Entity();

	ChildEntities& children = registry.parentEntities.emplace(entity);

	/* Each cat - related entity needs the "selected" component.
	 * This value of this component is passed to the fragment shader and is
	 * used in rendering.
	 */
	registry.selected.emplace(head);
	registry.selected.emplace(frontArm);
	registry.selected.emplace(entity);

	//---Head animation subentity---- putting this in front so that head gets rendered ontop of body
	int index = 0;
	children.child_data_map.emplace(index, head);
	children.normal_dists.emplace(index, scaleToScreenResolution(vec2( 9.5f, -41.0f )));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, scaleToScreenResolution(vec2(9.5f, -41.0f)));


	Motion& headmotion = registry.motions.emplace(head);
	headmotion.position = pos + children.normal_dists.at(0);
	headmotion.scale = scaleToScreenResolution({ 64.f, 64.f * 0.72803f}); //Look at the dimensions of the sprite sheet to get the right ratio

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
	children.normal_dists.emplace(index, scaleToScreenResolution(vec2(4.5f, 11.0f)));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, scaleToScreenResolution(vec2(4.5f, 11.0f)));

	Motion& frontArmMotion = registry.motions.emplace(frontArm);
	frontArmMotion.position = pos + children.normal_dists.at(1);
	frontArmMotion.scale = scaleToScreenResolution({ 54.f / 3, 128.f / 3 });

	Animation& frontArmAnim = registry.animations.emplace(frontArm);
	frontArmAnim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_ARM, STABILIZED_CONSTANTS });
	frontArmAnim.anim_state = TEXTURE_IDS::CAT_FRONT_ARM;
	frontArmAnim.name = "cat_front_arm";
	registry.renderRequests.insert(
		frontArm,
		{ TEXTURE_IDS::CAT_FRONT_ARM,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	Character* c = new Cat();
	Character* cat = registry.characters.insert(entity, c);
	cat->character = entity;
	

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((64.f / defaultResolution.x) * screenResolution.x);
	motion.scale = scaleToScreenResolution({ scale, scale * 1.655f }); //Look at the dimensions of the sprite sheet to get the right ratio

	// Add health component
	Health& healthval = registry.health.emplace(entity);
	healthval.hp = health;

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

	if (weapon == WEAPON_TYPES::RIFLE) {
		registry.weapons.insert(entity, Rifle());
	} else if (weapon == SHOTGUN) {
		registry.weapons.insert(entity, Shotgun());
	} else if (weapon == AWP) {
		registry.weapons.insert(entity, Awp());
	} else if (weapon == LAUNCHER) {
		registry.weapons.insert(entity, Launcher());
	}

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_FRONT_IDLE, CAT_IDLE_CONSTANTS});
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_IDLE_AK, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_IDLE_SG, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_IDLE_GL, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_SIDE_IDLE_AWP, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_WALK, CAT_WALK_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_JUMP, CAT_JUMP_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::CAT_HURT, CAT_HURT_CONSTANTS });
	anim.anim_state = TEXTURE_IDS::CAT_FRONT_IDLE;
	anim.name = "cat_body";

	return entity;
}

Entity createDog(WEAPON_TYPES weapon, vec2 pos, int health, GLFWwindow* window) {

	auto head = Entity();
	const auto entity = Entity();
	auto frontArm = Entity();

	ChildEntities& children = registry.parentEntities.emplace(entity);

	registry.selected.emplace(head);
	registry.selected.emplace(frontArm);
	registry.selected.emplace(entity);
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
	children.normal_dists.emplace(index, vec2(0.0f, 0.0f));
	children.tags.emplace(index, "animation");
	children.original_dists.emplace(index, vec2(0.0f, 0.0f));


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


	//----------------------------------------------
	Character* d = new Dog();
	Character* dog = registry.characters.insert(entity, d);
	dog->character = entity;

	// Add health component
	Health& healthbar = registry.health.emplace(entity);
	healthbar.hp = health;

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

	if (weapon == WEAPON_TYPES::RIFLE) {
		registry.weapons.insert(entity, Rifle());
	}
	else if (weapon == SHOTGUN) {
		registry.weapons.insert(entity, Shotgun());
	}
	else if (weapon == AWP) {
		registry.weapons.insert(entity, Awp());
	}
	else if (weapon == LAUNCHER) {
		registry.weapons.insert(entity, Launcher());
	}

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_FRONT_IDLE, CAT_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_SIDE_IDLE_AK, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_SIDE_IDLE_SG, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_SIDE_IDLE_GL, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_SIDE_IDLE_AWP, CAT_SIDE_IDLE_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_WALK, CAT_WALK_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_JUMP, CAT_JUMP_CONSTANTS });
	anim.animation_states_constants.insert({ TEXTURE_IDS::DOG_HURT, CAT_HURT_CONSTANTS });
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

Entity createButton(vec2 pos, vec2 scale, TEXTURE_IDS tex_id) {

	const auto entity = Entity();

	Clickable& button = registry.buttons.emplace(entity);

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

Entity createText(TextManager& textManager, std::string text, vec2 pos, float scale, glm::vec3 color) {
	const auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = scaleToScreenResolution({ scale, scale });

	Text& textfield = registry.texts.emplace(entity);
	textfield.text = text;
	textfield.color = color;

	registry.renderRequests.insert(
		entity,
		{	TEXTURE_IDS::TOTAL,
			SHADER_PROGRAM_IDS::FONT,
			GEOMETRY_BUFFER_IDS::TOTAL });

	//this is capital 'A' for reference
	float captialsize = textManager.getGlyphs()[65].size.y;

	// iterate through all characters
	std::string::const_iterator c;
	float acc = motion.position.x;
	for (c = textfield.text.begin(); c != textfield.text.end(); c++) {
		Glyph ch = textManager.getGlyphs()[*c];
		acc += (ch.advance >> 6) * motion.scale.x; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	textfield.scale.x = acc - motion.position.x; // set x scale
	textfield.scale.y = captialsize * motion.scale.y; // set y scale

	// center text to position
	motion.position.x = pos.x - textfield.scale.x / 2.f;

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

Entity createHealthCounter(Entity origin, int health, TextManager& textManager) {

	const vec3 color = registry.characters.get(origin)->team_color;

	const auto entity = Entity();

	HealthBox& healthbox = registry.healthboxes.emplace(entity);
	healthbox.parent = origin;
	healthbox.text = createText(textManager, std::to_string(health), { 0.0f,0.0f }, 1.0f, color);

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = { 100.0f, 100.0f };

	return entity;

}

Entity createTimerCounter(float timer, TextManager& textManager) {

	auto entity = Entity();

	OptionTimer& timer0 = registry.timer.emplace(entity);

	timer0.timerC = timer;
	
	int n = static_cast<int>(timer0.timerC);
	//timer0.text = createText({ 1025.0f, 390.0f }, 2.0f, { 0.0f, 0.0f, 0.0f }, std::to_string(timer0.timerC));
	createText(textManager, std::to_string(n), { 1060.0f, 390.0f }, 2.0f, { 0.0f, 0.0f, 0.0f });

	return entity;

}

Entity createGrenade(Entity originE, vec2 force) {

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

	Grenade& projectile = registry.grenades.emplace(entity);
	projectile.origin = originE;

	Rigidbody& rb = registry.rigidBodies.emplace(entity);
	rb.type = KINEMATIC;
	rb.inertia = 1;

	PhysicsSystem::applyForce(entity, force);


	return entity;
}

Entity createExplosion(float radius, vec2 position, Entity origin) {

	const auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	float scale = ceil((10.f / defaultResolution.x) * screenResolution.x);
	motion.scale = { radius, radius };

	Boxcollider& bc = registry.boxColliders.emplace(entity);
	calculateBoxVerticesAndSetTriangles(motion.position, motion.scale, bc);
	bc.transformed_required = true;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_IDS::EXPLOSION,
			SHADER_PROGRAM_IDS::ANIMATION,
			GEOMETRY_BUFFER_IDS::TEXTURED_QUAD });

	Animation& anim = registry.animations.emplace(entity);
	anim.animation_states_constants.insert({ TEXTURE_IDS::EXPLOSION, EXPLOSION_CONSTANTS });
	anim.anim_state = TEXTURE_IDS::EXPLOSION;

	Timer& timer = registry.entityTimers.emplace(entity);
	timer.time = 1000.f;
	timer.counter = 900.f;

	Explosion& explode = registry.explosions.emplace(entity);
	explode.origin = origin;

	return entity;
}
