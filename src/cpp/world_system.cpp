// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"
#include "..\hpp\common.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>

#include "..\hpp\tiny_ecs_registry.hpp"
#include <hpp/physics_system.hpp>

#include "hpp/audio_manager.hpp";

//#include <hpp/game_controller.hpp>

void testCallback() {
	printf("HELLO WORLD!!!!\n");
}

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

	init_main_menu();
	audio.play_music(MUSIC_LIST::IN_GAME_BACKGROUND);

	set_user_input_callbacks();

	//creates a wall on the left side of the screen
	printf("window height is: %i px, window width is: %i px\n", renderer->getScreenHeight(), renderer->getScreenWidth());

	//restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	//for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
	//	Motion& motion = motions_registry.components[i];
	//	if (motion.position.x + abs(motion.scale.x) < 0.f) {
	//		if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
	//			registry.remove_all_components_of(motions_registry.entities[i]);
	//	}
	//}
	//

	// Removing out of screen entities
	auto& motions_registry = registry.motions;
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

	//Load Title screen
	//Character Select

	//INITIALIZE Current game
	current_game.init(renderer, window);
}

void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system

	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void WorldSystem::init_main_menu() {
	createMenu(MENU_TYPES::START, 0.75);
	std::vector<std::function<void()>> onClick;
	onClick.push_back(testCallback);
	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (400.f / defaultResolution.y) * renderer->getScreenHeight() };
	vec2 scale = { (400.f / defaultResolution.x) * renderer->getScreenWidth(), (80.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1, onClick);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2, onClick);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (600.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3, onClick);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (700.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos4, scale,TEXTURE_IDS::BUTTON4, onClick);
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	// needs to be replaced?
	return false;
}

// On key callback
void WorldSystem::on_key(int button, int action, int mods) {
	//ALL PLAYER MOVE FUNCTIONALITY IS MOVED TO game_controller
	//THIS AREA SHOULD BE FOR CONTROLS THAT ARE FOR NAVIGATING THE MAIN MENU etc...
	
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		// printf("hello");
		check_for_button_presses();
	}
}

//I need to refactor everything from this point and beyond
void WorldSystem::check_for_button_presses() {

	//fk it build AABB
	for (Entity e : registry.buttons.entities) {
		Clickable button = registry.buttons.get(e);
		if (mouse_pos.x > button.vertecies[0].x && mouse_pos.x < button.vertecies[1].x && mouse_pos.y > button.vertecies[0].y && mouse_pos.y < button.vertecies[3].y) {
			
		
			if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON1) {
				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				play_levels(button.callbacks);
				
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON3) {

				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}

				play_tutorial(button.callbacks);
				
				break;
				/*for (auto callback : button.callbacks) {
					callback();
				}*/
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON4) {

				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				
				//if (glfwWindowShouldClose(window);
				//glfwCloseWindow(window);
				break;
				/*for (auto callback : button.callbacks) {
					callback();
				}*/
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON2) {
				for (Entity e : registry.menus.entities){
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				play_select(button.callbacks);
				
				break;
			}
		
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::HOWTOMOVE) {
				
				// registry.remove_all_components_of(e);
				// restart_game();
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				play_startscreen(button.callbacks);
				
				break;
			}
			
			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONC) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTOND)) {
				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				play_options(button.callbacks);
				
				break;
			}
			
			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONR) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL)) {
				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				restart_game();
				
				break;
			}
			
			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL1) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL2) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL3)) {
				for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
				}
				for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
				restart_game();
				
				break;
			}
		}
	}

}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	mouse_pos = mouse_position;
	// printf("mouse pos: %f, %f\n", mouse_pos.x, mouse_pos.y);
}

void WorldSystem::set_user_input_callbacks() {
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(this->window, cursor_pos_redirect);

	//auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2); };
	//glfwSetKeyCallback(wthis->window, key_redirect);*/

	auto mouse_input = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(this->window, mouse_input);
}

void WorldSystem::play_tutorial(std::vector<std::function<void()>> callbacks) {
	createButton(vec2{ renderer->getScreenWidth() / 2, renderer->getScreenHeight() / 2 }, vec2{ renderer->getScreenWidth(), renderer->getScreenHeight() },TEXTURE_IDS::HOWTOMOVE, callbacks);
}

void WorldSystem::play_select(std::vector<std::function<void()>> callbacks) {
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::SELECT, 0.75);
	vec2 pos_cat = { (defaultResolution.x / 4.f) / defaultResolution.x * renderer->getScreenWidth(), (defaultResolution.y / 2.0) / defaultResolution.y * renderer->getScreenHeight() };
	vec2 scale = { (500.f / defaultResolution.x) * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos_cat, scale,TEXTURE_IDS::BUTTONC, onClick);
	
	vec2 pos_dog = { (3.1f * defaultResolution.x / 4.f) / defaultResolution.x * renderer->getScreenWidth(), (defaultResolution.y / 2.0) / defaultResolution.y * renderer->getScreenHeight() };
	createButton(pos_dog, scale,TEXTURE_IDS::BUTTOND, onClick);
}

void WorldSystem::play_startscreen(std::vector<std::function<void()>> callbacks) {
	//createButton(vec2{ renderer->getScreenWidth() / 2, renderer->getScreenHeight() / 2 }, vec2{ renderer->getScreenWidth(), renderer->getScreenHeight() }, TEXTURE_IDS::HOWTOMOVE, callbacks);
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::START, 0.75);

	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (400.f / defaultResolution.y) * renderer->getScreenHeight() };
	vec2 scale = { (400.f / defaultResolution.x) * renderer->getScreenWidth(), (80.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1, onClick);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2, onClick);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (600.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3, onClick);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * renderer->getScreenWidth(), (700.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos4, scale,TEXTURE_IDS::BUTTON4, onClick);

}



void WorldSystem::play_options(std::vector<std::function<void()>> callbacks) {
		//createButton(vec2{ renderer->getScreenWidth() / 2, renderer->getScreenHeight() / 2 }, vec2{ renderer->getScreenWidth(), renderer->getScreenHeight() }, TEXTURE_IDS::HOWTOMOVE, callbacks);
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::OPTIONS, 0.75);

	vec2 pos1 = {(defaultResolution.x - 980.f) / defaultResolution.x * renderer->getScreenWidth(), (335.f / defaultResolution.y) * renderer->getScreenHeight() };
	vec2 scale = { (1200.f / defaultResolution.x) * renderer->getScreenWidth(), (800.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos1, scale,TEXTURE_IDS::BUTTONL, onClick);

	vec2 pos2 = {(defaultResolution.x - 900.f) / defaultResolution.x * renderer->getScreenWidth(), (335.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos2, scale,TEXTURE_IDS::BUTTONR, onClick);
		
	vec2 pos3 = {(defaultResolution.x - 980.f) / defaultResolution.x * renderer->getScreenWidth(), (550.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos3, scale,TEXTURE_IDS::BUTTONL, onClick);

	vec2 pos4 = {(defaultResolution.x - 900.f) / defaultResolution.x * renderer->getScreenWidth(), (550.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos4, scale,TEXTURE_IDS::BUTTONR, onClick);
}


void WorldSystem::play_levels(std::vector<std::function<void()>> callbacks) {
		//createButton(vec2{ renderer->getScreenWidth() / 2, renderer->getScreenHeight() / 2 }, vec2{ renderer->getScreenWidth(), renderer->getScreenHeight() }, TEXTURE_IDS::HOWTOMOVE, callbacks);
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::LEVELS, 0.75);

	vec2 pos1 = {(defaultResolution.x - 1100.f) / defaultResolution.x * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	vec2 scale = { (1200.f / defaultResolution.x) * renderer->getScreenWidth(), (800.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos1, scale,TEXTURE_IDS::BUTTONL1, onClick);

	vec2 pos2 = {(defaultResolution.x - 800.f) / defaultResolution.x * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos2, scale,TEXTURE_IDS::BUTTONL2, onClick);

	vec2 pos3 = {(defaultResolution.x - 500.f) / defaultResolution.x * renderer->getScreenWidth(), (500.f / defaultResolution.y) * renderer->getScreenHeight() };
	createButton(pos3, scale,TEXTURE_IDS::BUTTONL3, onClick);

}