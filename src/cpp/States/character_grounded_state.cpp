#include "hpp/States/character_grounded_state.hpp"

#include "GLFW/glfw3.h"
#include "hpp/tiny_ecs_registry.hpp"
#include "hpp/ui_system.hpp"
#include "hpp/Game_Mechanics/health_system.hpp"
#include "hpp/Game_Mechanics/shooting_system.hpp"
#include <hpp/world_system.hpp>

#include "hpp/physics_system.hpp"
#include "hpp/world_init.hpp"

#pragma region
//Super class 
CharacterGroundedState::CharacterGroundedState(Entity e) : CharacterState(e) {

}

CharacterGroundedState::~CharacterGroundedState() = default;


void CharacterGroundedState::enter() {
	CharacterState::enter();
}

void CharacterGroundedState::exit() {
	CharacterState::exit();
}

void CharacterGroundedState::step(float elapsed_ms) {
	CharacterState::step(elapsed_ms);
}

void CharacterGroundedState::on_player_key(int key, int, int action, int mod){
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_E) {
			next_turn = true;
		}
	}
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_SPACE) {
			WorldSystem::pause_flag = !WorldSystem::pause_flag;
			return;
		}
	}
}

void CharacterGroundedState::doChecks() {
	CharacterState::doChecks();
}

void CharacterGroundedState::handle_explosion_collision() {
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.explosions.has(entity)) {
			Explosion explosion = registry.explosions.get(entity); //you can hit yourself with an explosion
			if (entity_other == character) {
				Character* c = registry.characters.get(character);
				c->state_machine.changeState(c->damage_state);
			}
		}
	}
}
#pragma endregion Super-CharacterGroundedState

#pragma region
//Idle Sub class
CharacterIdleState::CharacterIdleState(Entity e) : CharacterGroundedState(e) {

}

CharacterIdleState::~CharacterIdleState() = default;


void CharacterIdleState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_idle();
}

void CharacterIdleState::exit() {
	CharacterGroundedState::exit();
}

void CharacterIdleState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
	handle_bullet_collisions();
	handle_explosion_collision();
	Character* chara = registry.characters.get(character);
	if (chara->state_machine.isSelected()) {
		chara->state_machine.changeState(chara->aim_state);
	}
}

void CharacterIdleState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterIdleState::on_player_key(int key, int, int action, int mod) {


}
void CharacterIdleState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterIdleState::on_mouse_click(int button, int action, int mods) {

}

void CharacterIdleState::handle_bullet_collisions() {
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.projectiles.has(entity)) {// Projectile hit terrain
			Projectile& pj = registry.projectiles.get(entity);
			if (entity_other == character && character != pj.origin) { // Projectile hit another player
				Character* c = registry.characters.get(character);
				c->state_machine.changeState(c->damage_state);
			}
		}
	}
}
#pragma endregion Sub-CharacterIdleState

#pragma region CharacterDamagedState
CharacterDamageState::CharacterDamageState(Entity e) : CharacterGroundedState(e) {

}


void CharacterDamageState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_hurt();
}

void CharacterDamageState::exit() {
	CharacterGroundedState::exit();
	stringtoformat.clear();
}

void CharacterDamageState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
	hurt_timer -= elapsed_ms;
	auto& collisionsRegistry = registry.collisions;
	Character* chara = registry.characters.get(character);
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.projectiles.has(entity) && entity_other == character) {
			Projectile& pj = registry.projectiles.get(entity);
			WeaponBase weap = registry.weapons.get(pj.origin);
			float travel_dist_normalized = min(length(registry.motions.get(entity).position - registry.motions.get(pj.origin).position) / weap.max_dist, 0.9f);
			float dmg = naive_lerp(weap.min_damage, weap.max_damage, (1-travel_dist_normalized));
			stringtoformat = std::to_string((int)dmg);
			decreaseHealth(character, (int)dmg);
			Motion& motion = registry.motions.get(character);
			//Entity damage_taken = createText(std::to_string(dmg), { motion.position.x, motion.position.y - 200.f }, 1.f, { 0.917, 0.262, 0.776 });
			if (registry.health.get(character).hp == 0) {
				chara->state_machine.changeState(chara->dead_state);
			}
			registry.characters.get(character)->play_hurt_sfx();
			hurt_timer = 1500.0f;

			// Blood splatter using particles
			auto& charPosition = registry.motions.get(entity_other).position;
			auto& projectilePosition = registry.motions.get(entity).position;
			vec2 vel = { 200.f, 180.f };
			vel.x = (projectilePosition.x < charPosition.x) ? vel.x : -vel.x;
			chara->particleSystem->emit(100, charPosition, vel);

			registry.remove_all_components_of(entity);
		} else if (registry.explosions.has(entity) && entity_other == character) {
			Explosion explosion = registry.explosions.get(entity);
			stringtoformat = std::to_string((int)explosion.damage);
			decreaseHealth(character, (int)explosion.damage);
			if (registry.health.get(character).hp == 0) {
				chara->state_machine.changeState(chara->dead_state);
			}
			registry.characters.get(character)->play_hurt_sfx();
			hurt_timer = 1500.0f;
			registry.boxColliders.remove(entity);
		}
	}
	if (hurt_timer <= 0.0f) {
		chara->state_machine.changeState(chara->idle_state);
	}
}

void CharacterDamageState::doChecks() {
	CharacterGroundedState::doChecks();
}
#pragma endregion


//Move sub class
#pragma region
CharacterMoveState::CharacterMoveState(Entity e) : CharacterGroundedState(e) {

}

CharacterMoveState::~CharacterMoveState() = default;


void CharacterMoveState::enter() {
	CharacterGroundedState::enter();
	Character* chara = registry.characters.get(character);
	chara->animate_walk();
}

void CharacterMoveState::exit() {
	CharacterGroundedState::exit();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = 0.0f;
}

void CharacterMoveState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);

}

void CharacterMoveState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterMoveState::on_player_key(int key, int, int action, int mod) {

}
void CharacterMoveState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveState

#pragma region
CharacterMoveLeftState::CharacterMoveLeftState(Entity e) : CharacterMoveState(e) {

}

void CharacterMoveLeftState::enter() {
	CharacterMoveState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = -speed;
}

void CharacterMoveLeftState::step(float elapsed_ms) {
	CharacterMoveState::step(elapsed_ms);
	if (registry.motions.get(character).velocity.x == 0.0f) {
		Character* chara = registry.characters.get(character);
		chara->state_machine.changeState(chara->aim_state);
	}

}

void CharacterMoveLeftState::doChecks() {
	CharacterMoveState::doChecks();
}

void CharacterMoveLeftState::on_player_key(int key, int, int action, int mod) {
	CharacterMoveState::on_player_key(key, 0, action, mod);
	Character* c = registry.characters.get(character);
	CharacterGroundedState::on_player_key(key, 0, action, mod);
	if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_A) {
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = 0.0f;
		}
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			c->state_machine.changeState(c->airborne_move_left);
		}
	}
}
void CharacterMoveLeftState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveLeftState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveLeftState

#pragma region
CharacterMoveRightState::CharacterMoveRightState(Entity e) : CharacterMoveState(e) {

}

void CharacterMoveRightState::enter() {
	CharacterMoveState::enter();
	Motion& motion = registry.motions.get(character);
	motion.velocity.x = speed;

}


void CharacterMoveRightState::step(float elapsed_ms) {
	CharacterMoveState::step(elapsed_ms);
	if (registry.motions.get(character).velocity.x == 0.0f) {
		Character* chara = registry.characters.get(character);
		chara->state_machine.changeState(chara->aim_state);
	} 

}

void CharacterMoveRightState::doChecks() {
	CharacterMoveState::doChecks();
}

void CharacterMoveRightState::on_player_key(int key, int, int action, int mod) {
	CharacterMoveState::on_player_key(key, 0, action, mod);
	Character* c = registry.characters.get(character);
	CharacterGroundedState::on_player_key(key, 0, action, mod);
	if (action == GLFW_RELEASE) {

		if (key == GLFW_KEY_D) {
			Motion& motion = registry.motions.get(character);
			motion.velocity.x = 0.0f;
		}
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			c->state_machine.changeState(c->airborne_move_right);
		}
	}
}
void CharacterMoveRightState::on_mouse_move(glm::vec2 mouse_pos) {

}
void CharacterMoveRightState::on_mouse_click(int button, int action, int mods) {

}
#pragma endregion CharacterMoveRightState

//Aiming
#pragma region
CharacterAimState::CharacterAimState(Entity e) : CharacterGroundedState(e) {

}


void CharacterAimState::enter() {
	CharacterGroundedState::enter();
	Motion& motion = registry.motions.get(character);
	original_xscale = motion.scale.x;
	motion.scale.x = motion.scale.x * 4;
	Character* chara = registry.characters.get(character);
	chara->animate_aim();
	UISystem::show_crosshair(character);

	//hide extra arm
	ChildEntities& children = registry.parentEntities.get(character);
	Entity arm = children.child_data_map.at(1);
	for (Entity& e : registry.renderRequests.entities) {
		if (arm == e) {
			e.setDepth(-1.f);
		}
	}
}

void CharacterAimState::exit() {
	CharacterGroundedState::exit();
	Motion& motion = registry.motions.get(character);
	motion.scale.x = original_xscale;
	UISystem::hide_crosshair(character);
	ChildEntities& children = registry.parentEntities.get(character);
	Entity arm = children.child_data_map.at(1);
	for (Entity& e : registry.renderRequests.entities) {
		if (arm == e) {
			e.setDepth(0.5f);
		}
	}
}

void CharacterAimState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
	ShootingSystem::setAimLoc(character);

}

void CharacterAimState::doChecks() {
	CharacterGroundedState::doChecks();
}

void CharacterAimState::on_player_key(int key, int, int action, int mod) {
	Character* chara = registry.characters.get(character);
	CharacterGroundedState::on_player_key(key, 0, action, mod);
	if (!chara->state_machine.isAI()) {
		if (action == GLFW_PRESS) {

			if (key == GLFW_KEY_UP) {
				ShootingSystem::aimUp(character, 0.05f);
			}
			if (key == GLFW_KEY_DOWN) {
				ShootingSystem::aimDown(character, 0.05f);
			}
			if (key == GLFW_KEY_W) {
				chara->state_machine.changeState(chara->airborne_state);
			}
			if (key == GLFW_KEY_D) {
				chara->state_machine.changeState(chara->move_right_state);
			}
			if (key == GLFW_KEY_A) {
				chara->state_machine.changeState(chara->move_left_state);
			}
			
		}
	}
}
void CharacterAimState::on_mouse_move(glm::vec2 mouse_pos) {
	/*WeaponBase& weapon = registry.weapons.get(character);
	Motion motion = registry.motions.get(character);
	float y = -mouse_pos.y + motion.position.y;
	float x = mouse_pos.x - motion.position.x;
	weapon.aim_angle = atan2f(y, x);*/
}
void CharacterAimState::on_mouse_click(int button, int action, int mods) {
	if (WorldSystem::pause_flag){
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			Character* chara = registry.characters.get(character);
			chara->state_machine.changeState(chara->shooting_state);
		}
	}
}

void CharacterAimState::on_mouse_scroll(double xoffset, double yoffset) {
	if (WorldSystem::pause_flag){
		return;
	}
	if (yoffset > 0) {
		ShootingSystem::aimUp(character, (float)(0.05 * abs(yoffset)));
	}
	else {
		ShootingSystem::aimDown(character, (float)(0.05 * abs(yoffset)));
	}
}

#pragma endregion CharacterAimState

#pragma region CharacterShootingState
CharacterShootingState::CharacterShootingState(Entity e) : CharacterGroundedState(e) {

}


void CharacterShootingState::enter() {
	CharacterGroundedState::enter();
	ShootingSystem::shoot(character);
	has_shot = true;
	Motion& motion = registry.motions.get(character);
	original_xscale = motion.scale.x;
	motion.scale.x = motion.scale.x * 4;
}

void CharacterShootingState::exit() {
	CharacterGroundedState::exit();
	Motion& motion = registry.motions.get(character);
	motion.scale.x = original_xscale;
}

void CharacterShootingState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
	for (Grenade& pj : registry.grenades.components) {
		pj.bounce_cooldown -= elapsed_ms;
	}
	handleGrenadeBounce();
	handle_explosion_collision();
}

void CharacterShootingState::doChecks() {
	CharacterGroundedState::doChecks();
	if (has_shot) {
		if (registry.projectiles.components.empty() && registry.grenades.components.empty() && registry.explosions.components.empty()) {
			next_turn = true;
		}
	}
}

void CharacterShootingState::handle_explosion_collision() {
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.explosions.has(entity)) {
			Explosion explosion = registry.explosions.get(entity); //you can hit yourself with an explosion
			if (entity_other == character) {
				Character* c = registry.characters.get(character);
				c->state_machine.changeState(c->damage_state);
				if (explosion.origin == character) {
					next_turn = true;
				}
			}
		}
	}
}


void CharacterShootingState::handleGrenadeBounce() { //abusing the statemachines a little bit
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		if (registry.grenades.has(entity)) {
			Motion& motion = registry.motions.get(entity);
			Grenade& pj = registry.grenades.get(entity);
			if (registry.terrains.has(entity_other) && entity_other != pj.origin) { // Projectile hit terrain
				if (pj.bounce_cooldown <= 0.0f) {
					pj.bounces -= 1;
					pj.bounce_cooldown = pj.COOLDOWN;
				}
				if (pj.bounces < 0) {
					createExplosion(275.f, motion.position + vec2{0, -100}, pj.origin);
					registry.remove_all_components_of(entity);
					registry.grenades.remove(entity); //????? WHY ??????? WHY DOES THE REGISTRY NEED THIS
				} else {
					Rigidbody& rb = registry.rigidBodies.get(entity);
					//PhysicsSystem::applyForce(entity, vec2{ (int)rb.collision_normal.x * 30 * pj.bounces, (int)rb.collision_normal.y * 30 * pj.bounces });
					if (motion.velocity.x > 0) {
						PhysicsSystem::applyForce(entity, vec2{ -7,0 });
					} else {
						PhysicsSystem::applyForce(entity, vec2{ 7,0 });
					}
					if ((int)rb.collision_normal.x == -1) {
						PhysicsSystem::applyForce(entity, {-30 * pj.bounces, 0});
					} else if ((int)rb.collision_normal.x == 1) {
						PhysicsSystem::applyForce(entity, { 30 * pj.bounces, 0 });
					} else if ((int)rb.collision_normal.y == -1) {
						PhysicsSystem::applyForce(entity, { 0, -20 * pj.bounces });
					} else if ((int)rb.collision_normal.y == 1) {
						PhysicsSystem::applyForce(entity, { 0, 20 * pj.bounces });
					}
				}
			}
		}
	}
}
#pragma endregion

#pragma region CharacterDeadState
//Idle Sub class
CharacterDeadState::CharacterDeadState(Entity e) : CharacterGroundedState(e) {

}


void CharacterDeadState::enter() {
	CharacterGroundedState::enter();
	remove_children(character);
	registry.animations.remove(character);
	registry.rigidBodies.remove(character);
	Motion& motion = registry.motions.get(character);
	motion.velocity = vec2(.0f, .0f);
	Character* c = registry.characters.get(character);
	c->animate_dead();
	c->isDead = true;
}

void CharacterDeadState::exit() {
	CharacterGroundedState::exit();
}

void CharacterDeadState::step(float elapsed_ms) {
	CharacterGroundedState::step(elapsed_ms);
}

void CharacterDeadState::doChecks() {
	CharacterGroundedState::doChecks();
}
#pragma endregion