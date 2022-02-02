// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "..\hpp\tiny_ecs_registry.hpp"

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


void WorldSystem::init() {
	//this->renderer = renderer_arg;

	// Set all states to default
	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

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

	// Create a new cat
	player_cat = createCat(renderer, { window_width_px / 2, window_height_px - 200 });
	registry.colors.insert(player_cat, { 1, 0.8f, 0.8f });

}

// Should the game be over ?
bool WorldSystem::is_over() const {
	// needs to be replaced?
	return false;
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	Motion& catMotion = registry.motions.get(player_cat);
	// Left
	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_RELEASE) {
			catMotion.velocity.x = 0;
		}
		else {
			catMotion.velocity.x = -200;
			printf("Pressed left!!!\n");;
		}
	}
	// Right
	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_RELEASE) {
			catMotion.velocity.x = 0;
		}
		else if (action == GLFW_PRESS) {
			catMotion.velocity.x = 200;
			printf("Pressed right!!!\n");
		}
	}
	// Up
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_RELEASE) {
			catMotion.velocity.y = 0;
		}
		else {
			catMotion.velocity.y = -200;
			printf("Pressed up!!!\n");
		}
	}
	// Down
	if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_RELEASE) {
			catMotion.velocity.y = 0;
		}
		else {
			catMotion.velocity.y = 200;
			printf("Pressed down!!!\n");
		}
	}
	printf("Cat.x: %f, Cat.y: %f", catMotion.position.x, catMotion.position.y);

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