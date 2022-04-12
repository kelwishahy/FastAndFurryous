#include <hpp/Game_Mechanics/shooting_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/world_init.hpp>
#include <hpp/physics_system.hpp>
#include "hpp/audio_manager.hpp"

ShootingSystem::ShootingSystem() {
	
}

ShootingSystem::~ShootingSystem() {
	//do something when this object is destroyed
}

void ShootingSystem::step(float elapsed_time) {
	auto& projectileRegistry = registry.projectiles; 
	for (uint i = 0; i < projectileRegistry.components.size(); i++) {
		// A projectile entity
		Entity entity = projectileRegistry.entities[i];

		Projectile& projectile = projectileRegistry.components[i];
		Motion& motion = registry.motions.get(entity);

		float step_seconds = elapsed_time / 1000.0f;

	}
}

void ShootingSystem::aimUp(Entity e, float amount) {


	WeaponBase& weapon = registry.weapons.get(e);
	Motion& motion = registry.motions.get(e);
	SHOOT_ORIENTATION orientation = (registry.animations.get(e).facingLeft) ? SHOOT_ORIENTATION::LEFT : SHOOT_ORIENTATION::RIGHT;

	if (orientation == SHOOT_ORIENTATION::LEFT) {
		weapon.aim_angle = (weapon.aim_angle - amount <= pi - weapon.MAX_ANGLE) ? pi - weapon.MAX_ANGLE : weapon.aim_angle - amount; 
	}
	else {
		if (weapon.aim_angle + amount > twopi) {
			weapon.aim_angle = 0.0f + amount;
		}
		else {
			if (weapon.aim_angle + amount > weapon.MAX_ANGLE && weapon.aim_angle < pio2) {
				weapon.aim_angle = weapon.MAX_ANGLE;
			} else {
				weapon.aim_angle += amount;
			}
		}
	}
	setAimLoc(e);
}

void ShootingSystem::aimDown(Entity e, float amount) {

	WeaponBase& weapon = registry.weapons.get(e);
	Motion& motion = registry.motions.get(e);
	SHOOT_ORIENTATION orientation = (registry.animations.get(e).facingLeft) ? SHOOT_ORIENTATION::LEFT : SHOOT_ORIENTATION::RIGHT;

	if (orientation == SHOOT_ORIENTATION::LEFT) {
		weapon.aim_angle = (weapon.aim_angle + amount >= threepio2 - (twopi - weapon.MIN_ANGLE)) ? threepio2 - (twopi - weapon.MIN_ANGLE) : weapon.aim_angle + amount;
	}
	else {
		if (weapon.aim_angle - amount < 0.0f) {
			weapon.aim_angle = twopi - amount;
		} else {
			if (weapon.aim_angle > threepio2 && weapon.aim_angle - amount < weapon.MIN_ANGLE) {
				weapon.aim_angle = weapon.MIN_ANGLE;
			} else {
				weapon.aim_angle -= amount;
			}
		}
	}
	// printf("weapons aim angle: %f \n", weapon.aim_angle);
	setAimLoc(e);
}

void ShootingSystem::setAimLoc(Entity e) {

	WeaponBase& weapon = registry.weapons.get(e);
	Motion& motion = registry.motions.get(e);
	SHOOT_ORIENTATION orientation = (registry.animations.get(e).facingLeft) ? SHOOT_ORIENTATION::LEFT : SHOOT_ORIENTATION::RIGHT;

	float x_end;
	float x_begin;
	//right facing
	if (orientation == SHOOT_ORIENTATION::RIGHT) {
		if (weapon.aim_angle > pio2 && weapon.aim_angle < threepio2) {
			if (weapon.aim_angle > pi) {
				weapon.aim_angle = threepio2 + (threepio2 - weapon.aim_angle);
			}
			else {
				weapon.aim_angle = pio2 - (weapon.aim_angle - pio2);
			}
		}
	}
	//left facing
	else {
		if (weapon.aim_angle < pio2 || weapon.aim_angle > threepio2) {
			if (weapon.aim_angle > threepio2) {
				weapon.aim_angle = threepio2 - (weapon.aim_angle - threepio2);
			}
			else {
				weapon.aim_angle = pio2 + (pio2 - weapon.aim_angle);
			}
		}
	}

}

void ShootingSystem::shoot(Entity e) {

	assert(registry.weapons.has(e));
	setAimLoc(e);
	WeaponBase& weapon = registry.weapons.get(e);

	if (weapon.type == RIFLE) {
		float xforce = cosf(weapon.aim_angle) * 40.0f;
		float yforce = -sinf(weapon.aim_angle) * 45.0f;

		createProjectile(e, { xforce, yforce });
		audio.play_sfx(SOUND_EFFECTS::GUNSHOT);
	} else if (weapon.type == SHOTGUN) {

		float xforce;
		float yforce;

		for (int i = 0; i < 8; i++) { //8 pellets are fired from shotgun
			xforce = cosf(weapon.aim_angle + 0.05f * (float)i) * 48.0f;
			yforce = -sinf(weapon.aim_angle + 0.05f * (float)i) * 40.0f;
			createProjectile(e, { xforce, yforce });
		}

		audio.play_sfx(SOUND_EFFECTS::GUNSHOT);
	} else if (weapon.type == AWP) {
		float xforce = cosf(weapon.aim_angle) * 40.0f;
		float yforce = -sinf(weapon.aim_angle) * 45.0f;

		Entity bullet = createProjectile(e, { xforce, yforce });
		Rigidbody& rb = registry.rigidBodies.get(bullet);
		rb.gravity_affected = false;
		audio.play_sfx(SOUND_EFFECTS::GUNSHOT);
	} else if (weapon.type == LAUNCHER) {
		float xforce = cosf(weapon.aim_angle) * 25.0f;
		float yforce = -sinf(weapon.aim_angle) * 20.0f;

		Entity grenade = createGrenade(e, { xforce, yforce });
		audio.play_sfx(SOUND_EFFECTS::GUNSHOT);
	}


}

void ShootingSystem::calculate_trajectory(Entity e) {

	assert(registry.weapons.has(e));
	WeaponBase& weapon = registry.weapons.get(e);

	if (weapon.type == RIFLE) {

	}
}

vec4 ShootingSystem::calculate_A(float c1, float c2, float c1p, float c2p) {

	return hermite_matrix * vec4{c1, c2, c1p, c2p};

}

float ShootingSystem::calculate_point(vec4 A, float deltaTime) {
	return (A[0] * pow(deltaTime , 3)) + (A[1] * pow(deltaTime, 2)) + ((double)A[2] * deltaTime) + A[3];
}