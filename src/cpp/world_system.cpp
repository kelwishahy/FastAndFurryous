// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "..\hpp\tiny_ecs_registry.hpp"
#include <hpp/physics_system.hpp>

//#include <hpp/game_controller.hpp>

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


void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window) {
	this->renderer = renderer;
	this->window = window;

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
	//
	if (current_game.inAGame) {
		current_game.step(elapsed_ms_since_last_update);
	}


	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	//registry.list_all_components();
	//printf("Restarting\n");

	//// Reset the game speed
	//current_speed = 1.f;

	//// Remove all entities that we created
	//// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	//// Debugging for memory/component leaks
	//registry.list_all_components();

	// Create a new cat
	//createAI(renderer, { (width / 2) , height - 250 });
	//createAI(renderer, { (window_width_px / 2) , window_height_px - 250 });
	////createAI(renderer, { (window_width_px / 2) , window_height_px - 250 });

	//printf("starting cat.x is: %i px, starting cat.y is: %i px", window_width_px / 2, window_height_px - 200);
	//registry.colors.insert(player_cat, { 1, 0.8f, 0.8f });
	//registry.list_all_components_of(player_cat);

	//Load Title screen
	//Character Select

	//INITIALIZE Current game
	current_game.init(renderer, window);
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
	//ALL PLAYER MOVE FUNCTIONALITY IS MOVED TO game_controller
	//THIS AREA SHOULD BE FOR CONTROLS THAT ARE FOR NAVIGATING THE MAIN MENU etc...


}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	
	// shoot
}
