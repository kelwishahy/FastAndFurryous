#include <hpp/Game_Mechanics/shooting_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/world_init.hpp>
#include <hpp/physics_system.hpp>

ShootingSystem::ShootingSystem() {
	
}

ShootingSystem::~ShootingSystem() {
	//do something when this object is destroyed
}

//kinda wanna get rid of this if possible
void ShootingSystem::init(RenderSystem* renderer) {
	this->renderer = renderer;
}

void ShootingSystem::step(float elapsed_time) {
	auto& projectileRegistry = registry.projectiles; 
	for (uint i = 0; i < projectileRegistry.components.size(); i++) {
		// A projectile entity
		Entity entity = projectileRegistry.entities[i];

		Projectile& projectile = projectileRegistry.components[i];
		Motion& motion = registry.motions.get(entity);

		float step_seconds = elapsed_time / 1000.0f;

		projectile.delta_time = (projectile.delta_time + step_seconds >= 1) ? 1 : projectile.delta_time + step_seconds;
		if (projectile.delta_time == 1 && !registry.rigidBodies.has(entity)) {
			Rigidbody& rb = registry.rigidBodies.emplace(entity);
			rb.type = NORMAL;
			motion.velocity = projectile.end_tangent;
		}
		if (!registry.rigidBodies.has(entity)) {
			vec2 translation = vec2{ calculate_point(projectile.trajectoryAx, projectile.delta_time) , calculate_point(projectile.trajectoryAy, projectile.delta_time) } - motion.position;
			PhysicsSystem::translatePos(entity, translation);
		}
	}
}

void ShootingSystem::aimUp(Entity e) {


	WeaponBase& weapon = registry.weapons.get(e);
	Motion& motion = registry.motions.get(e);
	SHOOT_ORIENTATION orientation = (registry.animations.get(e).facingLeft) ? SHOOT_ORIENTATION::LEFT : SHOOT_ORIENTATION::RIGHT;

	if (orientation == SHOOT_ORIENTATION::LEFT) {
		weapon.aim_angle = (weapon.aim_angle - 0.1 <= pi - weapon.MAX_ANGLE) ? pi - weapon.MAX_ANGLE : weapon.aim_angle - 0.1;
	}
	else {
		weapon.aim_angle = (weapon.aim_angle + 0.1 >= weapon.MAX_ANGLE) ? weapon.MAX_ANGLE : weapon.aim_angle + 0.1;
	}
	printf("weapons aim angle: %f \n", weapon.aim_angle);
	setAimLoc(e);
}

void ShootingSystem::aimDown(Entity e) {

	WeaponBase& weapon = registry.weapons.get(e);
	Motion& motion = registry.motions.get(e);
	SHOOT_ORIENTATION orientation = (registry.animations.get(e).facingLeft) ? SHOOT_ORIENTATION::LEFT : SHOOT_ORIENTATION::RIGHT;

	if (orientation == SHOOT_ORIENTATION::LEFT) {
		weapon.aim_angle = (weapon.aim_angle + 0.1 >= pi - weapon.MIN_ANGLE) ? pi - weapon.MIN_ANGLE : weapon.aim_angle + 0.1;
	}
	else {
		weapon.aim_angle = (weapon.aim_angle - 0.1 <= weapon.MIN_ANGLE) ? weapon.MIN_ANGLE : weapon.aim_angle - 0.1;
	}
	printf("weapons aim angle: %f \n", weapon.aim_angle);
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
		x_end = motion.position.x + weapon.distance + weapon.area;
		x_begin = motion.position.x + weapon.distance - weapon.area;
		if (weapon.aim_angle > pio2) {
			weapon.aim_angle -= pio2;
		}
	}
	//left facing
	else {
		x_end = motion.position.x - weapon.distance - weapon.area;
		x_begin = motion.position.x - weapon.distance + weapon.area;
		if (weapon.aim_angle < pio2) {
			weapon.aim_angle += pio2;
		}
	}

	if (orientation == SHOOT_ORIENTATION::RIGHT) {
		float move_step = (x_end - x_begin) * (weapon.aim_angle / (weapon.MAX_ANGLE - weapon.MIN_ANGLE));
		weapon.aim_loc_x = (x_end - move_step <= x_begin) ? x_begin : x_end - move_step;
	}
	else {
		float move_step = (x_begin - x_end) * (weapon.aim_angle - pio2 / (weapon.MAX_ANGLE - weapon.MIN_ANGLE));
		weapon.aim_loc_x = (x_end + move_step >= x_begin) ? x_begin : x_end + move_step;
	}
	printf("weapons aim loc: %f\n", weapon.aim_loc_x);
}

void ShootingSystem::shoot(Entity e) {

	assert(registry.weapons.has(e));
	WeaponBase& weapon = registry.weapons.get(e);

	if (weapon.type == RIFLE) {
		float x1 = registry.motions.get(e).position.x;
		printf("x1 %f, ", x1);
		float x2 = weapon.aim_loc_x;
		printf("x2 %f, ", x2);
		float x1p = cos(weapon.aim_angle) * 2;
		float x2p = x1p;

		float plane = registry.motions.get(e).position.y;

		float y1 = plane;
		//Hard coded value
		float y2 = plane;
		//Multiplier is hard coded
		float y1p = -sin(weapon.aim_angle) * 2000.0f;
		//Currently the second tangent just mirrors first tangent, we can change this
		float y2p = -y1p;
		vec4 xt = calculate_A(x1, x2, x1p, x2p);
		vec4 yt = calculate_A(y1, y2, y1p, y2p);
		
		createProjectile(renderer, e, xt, yt, vec2{x2p * 100.0f, y2p});
	}


}

vec4 ShootingSystem::calculate_A(float c1, float c2, float c1p, float c2p) {

	return hermite_matrix * vec4{c1, c2, c1p, c2p};

}

float ShootingSystem::calculate_point(vec4 A, float deltaTime) {
	return (A[0] * pow(deltaTime , 3)) + (A[1] * pow(deltaTime, 2)) + ((double)A[2] * deltaTime) + A[3];
}