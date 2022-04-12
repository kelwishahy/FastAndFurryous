// internal
#include "../hpp/physics_system.hpp"
#include "../hpp/world_init.hpp"

#include <hpp/render_system.hpp>
#include <hpp/components.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "glm/ext.hpp"

#define OUT //for clarity

using namespace glm;

void PhysicsSystem::init(RenderSystem* renderer) {
	this->renderer = renderer;
}

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

void project_vertices(std::vector<vec2> box_vertices, vec2 axis, OUT float& min, OUT float& max) {

	min = 99999999.0f;
	max = -99999999.0f;

	for (uint i = 0; i < box_vertices.size(); i++) {

		vec2 v = box_vertices[i];
		float proj = dot(v, axis);

		if (proj < min) {
			min = proj;
		}
		if (proj > max) {
			max = proj;
		}
	}

}

//Used to correctly orient the normals on a given edge of a box
vec2 find_arithmetic_mean(std::vector<vec2> vertices) {
	
	float sumX = 0.0f;
	float sumY = 0.0f;

	for (uint i = 0; i < vertices.size(); i++) {
		vec2 vertex = vertices[i];
		sumX += vertex.x;
		sumY += vertex.y;
	}

	return vec2(sumX / vertices.size(), sumY / vertices.size());
}

//Detects polygons via the Seperating axis theorm
//Only box colliders as params right now but this should work for any polygon that is in any orientation
bool box_collision(const Boxcollider collider, const Boxcollider collider2, OUT vec2& normal, OUT float& depth) {
	std::vector<vec2> box1 = collider.vertices;
	std::vector<vec2> box2 = collider2.vertices;

	for (uint i = 0; i < box1.size(); i++) {
		vec2 va = box1[i];
		vec2 vb = box1[(i + 1) % box1.size()];

		vec2 edge = vb - va;
		vec2 axis = vec2{ -edge.y, edge.x };

		float min1 = 0;
		float max1 = 0;
		float min2 = 0;
		float max2 = 0;
		project_vertices(box1, axis, min1, max1);
		project_vertices(box2, axis, min2, max2);

		if (min1 >= max2 || min2 >= max1) {
			return false;
		}

		float axisdepth = min(max2 - min1, max1 - min2);
		if (axisdepth < depth) {
			depth = axisdepth;
			normal = axis;
		}
	}

	for (uint i = 0; i < box2.size(); i++) {
		vec2 va = box2[i];
		vec2 vb = box2[(i + 1) % box2.size()];

		vec2 edge = vb - va;
		vec2 axis = vec2{ -edge.y, edge.x };

		float min1 = 0;
		float max1 = 0;
		float min2 = 0;
		float max2 = 0;
		project_vertices(box1, axis, min1, max1);
		project_vertices(box2, axis, min2, max2);

		if (min1 >= max2 || min2 >= max1) {
			return false;
		}

		float axisdepth = min(max2 - min1, max1 - min2);
		if (axisdepth < depth) {
			depth = axisdepth;
			normal = axis;
		}

	}
	depth /= length(normal);
	normal = normalize(normal);

	vec2 center1 = find_arithmetic_mean(box1);
	vec2 center2 = find_arithmetic_mean(box2);

	if (dot(center1 - center2, normal) < 0.0f) {
		normal = -normal;
	}

	return true;
}

//If an entity gets transformed and they have a boxcollider, we need to recalculate the vertices
void PhysicsSystem::transformBoxColliders() {
	ComponentContainer<Boxcollider>& collider_container = registry.boxColliders;

	// Iterate through all the boxcolliders
	for (uint i = 0; i < collider_container.components.size(); i++) {
		Boxcollider& collider = collider_container.components[i];
		// Entity entity = collider_container.entities[i];

		if (collider.transformed_required) {
			// Motion motion = registry.motions.get(entity);
			// float angle = motion.angle;
			mat4 transform = mat4(1.0);

			//TODO do rotations
			//transform = rotate();
			transform = translate(transform, vec3(collider.deltaPos, 0.0f));
			//transform = scale(transform, vec3(motion.scale, 0.0f));
			for (uint i = 0; i < collider.vertices.size(); i++) {
				vec4 coord = transform * vec4(collider.vertices[i], 0.0f, 1.0f);
				collider.vertices[i] = vec2{ coord.x, coord.y };
			}
			collider.transformed_required = false;
		}

	}
}

void PhysicsSystem::checkForCollisions() {
	ComponentContainer<Boxcollider>& box_collider_container = registry.boxColliders;
	for (uint i = 0; i < box_collider_container.components.size(); i++)
	{
		Boxcollider& collider_i = box_collider_container.components[i];
		Entity& entity_i = box_collider_container.entities[i];

		Motion* motion_i = &registry.motions.get(entity_i);
		vec2* position_i = &motion_i->position;

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < box_collider_container.components.size(); j++) {
			Entity& entity_j = box_collider_container.entities[j];

			// If this is a tile-tile collision, skip
			if (registry.terrains.has(entity_i) && registry.terrains.has(entity_j)) continue;

			// Don't do anything if the character is dead
			if (registry.characters.has(entity_i) && registry.characters.get(entity_i)->isDead ||
				registry.characters.has(entity_j) && registry.characters.get(entity_j)->isDead)
				continue;

			Boxcollider& collider_j = box_collider_container.components[j];

			Motion* motion_j = &registry.motions.get(entity_j);
			vec2* position_j = &motion_j->position;

			vec2 normal;
			float depth = 9999999;
			if (box_collision(collider_i, collider_j, normal, depth)) {
				//If its a rigid body collision resolve right away
				//if (registry.rigidBodies.has(entity_i)) { //More ugly stuff
				//	Rigidbody& entity_i_rb = registry.rigidBodies.get(entity_i);
				//	entity_i_rb.collision_normal = normal;
				//}
				//if (registry.rigidBodies.has(entity_j)) { //Ugly stuff on my part
				//	Rigidbody& entity_j_rb = registry.rigidBodies.get(entity_j);
				//	entity_j_rb.collision_normal = -normal;
				//}
				if (registry.rigidBodies.has(entity_i) && registry.rigidBodies.has(entity_j)) {
					Rigidbody& entity_i_rb = registry.rigidBodies.get(entity_i);
					Rigidbody& entity_j_rb = registry.rigidBodies.get(entity_j);
					if (entity_i_rb.type == NORMAL && entity_j_rb.type == STATIC || entity_i_rb.type == KINEMATIC && entity_j_rb.type == STATIC) {
						moveBackEntity(entity_i, normal, depth);
					}
					else if (entity_i_rb.type == NORMAL && entity_j_rb.type == NORMAL) {
						moveBackEntity(entity_i, normal, depth/2);
						moveBackEntity(entity_j, -normal, depth/2);
					}
					else if (entity_i_rb.type == STATIC && entity_j_rb.type == NORMAL || entity_i_rb.type == STATIC && entity_j_rb.type == KINEMATIC) {
						moveBackEntity(entity_j, -normal, depth);
					}
					transformBoxColliders();
				}
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				// Calculate the normal and intersection depth of CIRCLE collisions

				Collision& collision1 = registry.collisions.emplace_with_duplicates(entity_i, entity_j);

				Collision& collision2 = registry.collisions.emplace_with_duplicates(entity_j, entity_i);
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

		// Don't do anything if the character is dead
		if (registry.characters.has(entity) && registry.characters.get(entity)->isDead)
			continue;

		float step_seconds = elapsed_ms / 1000.f;
		if (registry.rigidBodies.has(entity)) {
			Rigidbody& rb = registry.rigidBodies.get(entity);
			if (registry.rigidBodies.has(entity)) {
				Rigidbody& rb = registry.rigidBodies.get(entity);
				if (rb.type == KINEMATIC) {
					//we are applying gravity forces here

					if (rb.gravity_affected) {
						applyForce(entity, GRAVITY_FORCE);
					}
					//sum of Forces + inv(mass) + deltaTime
					motion.velocity = rb.force_accumulator * (1.0f / rb.mass) * elapsed_ms;
				}
				if (rb.type == STATIC) {
					motion.velocity = vec2{ 0,0 };
				}
				if (rb.type == NORMAL) {
					if (motion.velocity.y >= TERMINAL_VELOCITY && !registry.projectiles.has(entity)) {
						motion.velocity.y = TERMINAL_VELOCITY;
					}
					else {
						motion.velocity.y += GRAVITY_CONST;
					}
				}
			}
		}
		translatePos(entity, motion.velocity * step_seconds);
	}
}

void PhysicsSystem::step(float elapsed_ms) {
	fixed_update_accumulator += elapsed_ms - FIXED_UPDATE_STEP;
	//printf("accumulator: %f\n", fixed_update_accumulator);

	fixed_update();
	if (fixed_update_accumulator >= FIXED_UPDATE_STEP) {
		fixed_update();
		fixed_update_accumulator = 0.0f;
		printf("extra is being called fixed step\n");
	}
}

void PhysicsSystem::fixed_update() {
	transformBoxColliders();

	//1/60th of a second
	applyMotions(FIXED_UPDATE_STEP);

	transformChildedEntities();

	transformBoxColliders();

	// Check for collisions between all moving entities
	checkForCollisions();
}

//translation is the distance to be moved
void PhysicsSystem::translatePos(Entity e, vec2 translation) {
	if (registry.boxColliders.has(e)) {
		Boxcollider& collider = registry.boxColliders.get(e);
		Motion& motion = registry.motions.get(e);

		vec2 oldpos = motion.position;
		motion.position += translation;
		collider.deltaPos = motion.position - oldpos;
		collider.transformed_required = true;
	} else if (registry.particles.has(e)) {
		Motion& motion = registry.motions.get(e);
		motion.position += translation;
		motion.velocity.y += 2.6f;
	}
}

void PhysicsSystem::moveBackEntity(Entity e, vec2 normal, float depth) {
	translatePos(e, normal * depth);

	Rigidbody& rb = registry.rigidBodies.get(e);
	rb.collision_depth = depth;
	rb.collision_normal = normal;
}

void  PhysicsSystem::applyForce(Entity e, glm::vec2 force) {

	assert(registry.motions.has(e));
	assert(registry.rigidBodies.has(e));

	Rigidbody& rb = registry.rigidBodies.get(e);
	rb.force_accumulator += force;

}
void PhysicsSystem::transformChildedEntities() {

	for (Entity e : registry.parentEntities.entities) {
		// Don't do anything if the character is dead
		if (registry.characters.has(e) && registry.characters.get(e)->isDead)
			continue;

		ChildEntities children = registry.parentEntities.get(e);

		for (int i = 0; i < children.child_data_map.size(); i++) {
			Entity child = children.child_data_map.at(i);
			if (registry.motions.has(children.child_data_map.at(i))) {
				Motion& child_motion = registry.motions.get(children.child_data_map.at(i));
				Motion anchor_motion = registry.motions.get(e);
				if (anchor_motion.angle > 0.0f) {
					child_motion.position = anchor_motion.position + vec2{ cos(anchor_motion.angle), sin(anchor_motion.angle) } *children.normal_dists.at(i);
				}
				else {
					child_motion.position = anchor_motion.position + children.normal_dists.at(i);
				}
			}
		}

	}

}
