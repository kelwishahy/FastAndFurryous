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

//#include <hpp/game_controller.hpp>

void testCallback() {
	printf("HELLO WORLD!!!!\n");
}

WorldSystem::WorldSystem() {
	// seeding rng with random device
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

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

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return;
	}

	background_music = Mix_LoadMUS(audio_path("background-music.wav").c_str());

	if (background_music == nullptr ) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("background-music.wav").c_str());
		return;
	}

	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(this->window, cursor_pos_redirect);

	/*auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2); };
	glfwSetKeyCallback(wthis->window, key_redirect);*/

	//creates a wall on the left side of the screen
	printf("window height is: %i px, window width is: %i px", renderer->getScreenHeight(), renderer->getScreenWidth());

	//restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	cooldown += elapsed_ms_since_last_update;
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
	if (!current_game.inAGame) {
		if (cooldown > 2000.0f) {
			glfwGetWindowUserPointer(this->window);
			int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS)
			{
				check_for_button_presses();
				cooldown = 0;
			}
		}
	}
	if (current_game.inAGame) {
		current_game.step(elapsed_ms_since_last_update);
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	//registry.list_all_components();

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

void WorldSystem::init_main_menu() {

	createMenu(MENU_TYPES::START, 0.75);

	//Init buttons
	//add onClickCallbacks
	std::vector<std::function<void()>> onClick;
	onClick.push_back(testCallback);
	//std::function<void()> test = std::bind(&restart_game);
	//onClick.push_back(restart_game);
	createButton(vec2{ renderer->getScreenWidth() - 300, 400 }, vec2{400, 80},TEXTURE_IDS::BUTTON1, onClick);

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
				play_tutorial(button.callbacks);
				/*for (auto callback : button.callbacks) {
					callback();
				}*/
			}
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::HOWTOMOVE) {
				registry.remove_all_components_of(e);
				restart_game();
			}
		}
	}

}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	mouse_pos = mouse_position;
	printf("mouse pos: %f, %f\n", mouse_pos.x, mouse_pos.y);
}

void WorldSystem::play_tutorial(std::vector<std::function<void()>> callbacks) {
	createButton(vec2{ renderer->getScreenWidth() / 2, renderer->getScreenHeight() / 2 }, vec2{ renderer->getScreenWidth(), renderer->getScreenHeight() },TEXTURE_IDS::HOWTOMOVE, callbacks);
}
