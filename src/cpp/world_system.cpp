// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>

#include "..\hpp\tiny_ecs_registry.hpp"
#include <hpp/physics_system.hpp>

// Create the bug world
WorldSystem::WorldSystem() {
	// seeding rng with random device
}

WorldSystem::~WorldSystem() {
	// Destroy all created components
	registry.clear_all_components();
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}


void WorldSystem::init(RenderSystem* renderer) {
	this->renderer = renderer;

	// Set all states to default

	//creates a wall on the left side of the screen
	printf("window height is: %i px, window width is: %i px", renderer->getScreenHeight(), renderer->getScreenWidth());

	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	// Check if the player is still alive
	if (registry.health.get(player_cat).hp == 0) restart_game();

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// change the animation type depending on the velocity
	Motion& catMotion = registry.motions.get(player_cat);
	Player& catPlayer = registry.players.get(player_cat);
	if (catMotion.velocity.x == 0 ) {
		catPlayer.animation_type = IDLE;
	}
	if (catMotion.velocity.x != 0) {
		catPlayer.animation_type = WALKING;
	}
	if (catMotion.velocity.y < 0) {
		catPlayer.animation_type = JUMPING;
	}
	if (catMotion.velocity.x < 0) {
		catPlayer.facingLeft = 1;
	}
	if (catMotion.velocity.x > 0) {
		catPlayer.facingLeft = 0;
	}

	// FOR AI Animation
	Motion& aiMotion = registry.motions.get(ai_cat);
	Player& aiCat = registry.players.get(ai_cat);
	if (aiMotion.velocity.x == 0) {
		aiCat.animation_type = IDLE;
	}
	if (aiMotion.velocity.x != 0) {
		aiCat.animation_type = WALKING;
	}
	if (aiMotion.velocity.y < 0) {
		aiCat.animation_type = JUMPING;
	}
	if (aiMotion.velocity.x < 0) {
		aiCat.facingLeft = 1;
	}
	if (aiMotion.velocity.x > 0) {
		aiCat.facingLeft = 0;
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();

	// Create a new cat
	player_cat = createCat(renderer, { width / 2 - 200, height - 400 });
	// Create ai cat
	ai_cat = createAI(renderer, { (width / 2) , height - 250 });
	//createAI(renderer, { (window_width_px / 2) , window_height_px - 250 });

	//Floor
	createWall(renderer, { width / 2, height }, width, 50);

	//Left Wall
	createWall(renderer, { 0, height / 2 }, 50, height);

	//Right Wall
	createWall(renderer, { width, height / 2 }, 50, height);

	//Ceiling
	createWall(renderer, { width / 2, 0 }, width, 50);

	printf("starting cat.x is: %i px, starting cat.y is: %i px", width / 2, height - 200);
	registry.colors.insert(player_cat, { 1, 0.8f, 0.8f });
	registry.list_all_components_of(player_cat);

}

void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Not resolving rigidbody here
		if (registry.players.has(entity)) {
			if (registry.rigidBodies.has(entity_other)) {
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	// needs to be replaced?
	return false;
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	Motion& catMotion = registry.motions.get(player_cat);
	vec2 oldVelocity = catMotion.velocity;
	
	current_speed = fmax(100.0f, current_speed);
	if (action == GLFW_PRESS && key == GLFW_KEY_W) {
		if (catMotion.position.y > renderer->getScreenHeight() - 100) {
			catMotion.velocity.y = - current_speed;
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_S) {
		catMotion.velocity.y = current_speed;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_D) {
		catMotion.velocity.x = current_speed;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_A) {
		catMotion.velocity.x = -current_speed;
	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A && catMotion.velocity.x < 0) {
			catMotion.velocity.x = 0.0f;
		}
		if (key == GLFW_KEY_D && catMotion.velocity.x > 0) {
			catMotion.velocity.x = 0.0f;
		}
	}

	/*if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP && catMotion.velocity.y < 0) {
			catMotion.velocity.y = 0.0f;
		}
		if (key == GLFW_KEY_DOWN && catMotion.velocity.y > 0) {
			catMotion.velocity.y = 0.0f;
		}
	}*/
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		restart_game();
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	
	// shoot
}