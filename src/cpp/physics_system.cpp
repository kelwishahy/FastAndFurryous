// internal
#include "../hpp/physics_system.hpp"
#include "../hpp/world_init.hpp"

using namespace glm;

//Creates a ray entity and returns the ray
//Not sure what the best way to handle the ray is...
Entity castRay(vec2 origin, float direction, float distance, float max_depth) {

	Entity ray = Entity();

	Motion& ray_motion = registry.motions.emplace(ray);
	ray_motion.position = origin;
	ray_motion.angle = direction;
	ray_motion.scale.x = distance;

	RayCast& cast = registry.rayCasts.emplace(ray);
	cast.max_depth = max_depth;

	return ray;

}

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

bool circle_collision(const Motion& motion, const Motion& motion2) {
	
	float dist = distance(motion.position, motion2.position);
	float radii = sqrt(dot(motion.scale, motion.scale)) + sqrt(dot(motion2.scale, motion2.scale));

	if (dist >= radii) {
		return false;
	}
	else {
		return true;
	}

}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

void checkForCollisions() {
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (circle_collision(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				// Calculate the normal and intersection depth of CIRCLE collisions
				float dist = distance(motion_i.position, motion_j.position);
				float radii = sqrt(dot(motion_i.scale, motion_i.scale)) + sqrt(dot(motion_j.scale, motion_j.scale));
				float depth = radii - dist;
				vec2 normal = normalize(motion_j.position - motion_i.position);

				Collision& collision1 = registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				collision1.depth = depth;
				collision1.normal = normal;

				Collision& collision2 = registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				collision2.depth = depth;
				collision2.normal = -normal;
			}
		}
	}
}

void PhysicsSystem :: applyMotions(float elapsed_ms) {
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;
		if (registry.rigidBodies.has(entity)) {
			Rigidbody& rb = registry.rigidBodies.get(entity);
			if (rb.type == KINEMATIC) {
				//Do something not implemented
			} 
			if (rb.type == STATIC) {
				motion.velocity = vec2{ 0,0 };
			}
			if (rb.type == NORMAL) {
				motion.velocity += vec2{ 0, GRAVITY_CONST };
			}
		}
		//Apply motion at the end regardlesss of if Rigidbody or not
		motion.position += motion.velocity * step_seconds;
	}
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Check for collisions between all moving entities
	checkForCollisions();

	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	applyMotions(elapsed_ms);

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;


	// debugging of bounding boxes
	//if (debugging.in_debug_mode)
	//{
	//	uint size_before_adding_new = (uint)motion_container.components.size();
	//	for (uint i = 0; i < size_before_adding_new; i++)
	//	{
	//		Motion& motion_i = motion_container.components[i];
	//		Entity entity_i = motion_container.entities[i];

	//		// don't draw debugging visuals around debug lines
	//		if (registry.debugComponents.has(entity_i))
	//			continue;

	//		// visualize the radius with two axis-aligned lines
	//		const vec2 bonding_box = get_bounding_box(motion_i);
	//		float radius = sqrt(dot(bonding_box/2.f, bonding_box/2.f));
	//		vec2 line_scale1 = { motion_i.scale.x / 10, 2*radius };
	//		vec2 line_scale2 = { 2*radius, motion_i.scale.x / 10};
	//		vec2 position = motion_i.position;

	//		//This dosen't work at the moment because we need to setup the geometry for the lines
	//		//Entity line1 = createLine(motion_i.position, line_scale1);
	//		//Entity line2 = createLine(motion_i.position, line_scale2);

	//		// !!! TODO A2: implement debug bounding boxes instead of crosses
	//	}
	//}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}