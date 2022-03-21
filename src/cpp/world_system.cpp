// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"
#include "..\hpp\common.hpp"

// stlib
#include <cassert>

#include "..\hpp\tiny_ecs_registry.hpp"
#include <hpp/physics_system.hpp>


#include "hpp/audio_manager.hpp";
#include <hpp/options_system.hpp>
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


void WorldSystem::init(GLFWwindow* window) {
	this->window = window;
	this->camera = OrthographicCamera(0.f, screenResolution.x, screenResolution.y, 0.f);

	this->mapSystem = MapSystem();
	this->mapSystem.init();

	this->textManager = TextManager();
	textManager.initFonts();

	init_main_menu();
	audio.play_music(MUSIC_LIST::IN_GAME_BACKGROUND);

	set_user_input_callbacks();

	printf("window height is: %f px, window width is: %f px\n", screenResolution.x, screenResolution.y);
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	if (current_game.inAGame) {
		current_game.step(elapsed_ms_since_last_update);
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	//Initialize current game
	current_game.init(window, mapSystem.getMap(MAPS::INDUSTRIAL), camera, textManager);
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

	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = { (400.f / defaultResolution.x) * screenResolution.x, (80.f / defaultResolution.y) * screenResolution.y };
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1, onClick);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (500.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2, onClick);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (600.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3, onClick);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (700.f / defaultResolution.y) * screenResolution.y };
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
				remove_components();
				play_levels(button.callbacks);

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON3) {

				remove_components();
				play_tutorial(button.callbacks);

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON4) {

				remove_components();
				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON2) {

				remove_components();
				play_select(button.callbacks);

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::HOWTOMOVE) {

				remove_components();
				play_startscreen(button.callbacks);

				break;
			}

			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONC) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTOND)) {
				remove_components();
				play_options(button.callbacks, 20, 3);

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONLA) {

				int newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = decreaseTimer(e, 5);
				}

				int newplayers;
				for (Entity e : registry.players.entities) {
					OptionPlayers players = registry.players.get(e);
					newplayers = players.playersN;
				}

				remove_components();		
				play_options(button.callbacks, newtimer, newplayers);
				
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONRA) {

				int newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = increaseTimer(e, 5);
				}

				int newplayers;
				for (Entity e : registry.players.entities) {
					OptionPlayers players = registry.players.get(e);
					newplayers = players.playersN;		
				}

				remove_components();
				play_options(button.callbacks, newtimer, newplayers);
		
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONLB) {

				int newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}

				int newplayers;
				for (Entity e : registry.players.entities) {
					OptionPlayers players = registry.players.get(e);
					newplayers = decreasePlayers(e, 1);
				}

				remove_components();
				play_options(button.callbacks, newtimer, newplayers);
			
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONRB) {

				int newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}

				int newplayers;
				for (Entity e : registry.players.entities) {
					OptionPlayers players = registry.players.get(e);
					newplayers = increasePlayers(e, 1);
				}

				remove_components();
				
				play_options(button.callbacks, newtimer, newplayers);
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONGAME) {
				remove_components();
				restart_game();

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONCANCEL) {
				remove_components();
				play_options(button.callbacks, 20, 3);

				break;
			}
			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL1) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL2) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL3)) {
				remove_components();
				restart_game();

				break;
			}
		}
	}

}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

	mouse_pos = mouse_position;
}

void WorldSystem::set_user_input_callbacks() {
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(this->window, cursor_pos_redirect);

	auto mouse_input = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(this->window, mouse_input);
}

void WorldSystem::play_tutorial(std::vector<std::function<void()>> callbacks) {
	createButton(vec2{ screenResolution.x / 2, screenResolution.y / 2 }, vec2{ screenResolution.x, screenResolution.y },TEXTURE_IDS::HOWTOMOVE, callbacks);
}

void WorldSystem::play_select(std::vector<std::function<void()>> callbacks) {
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::SELECT, 0.75);

	vec2 pos_cat = { (defaultResolution.x / 4.f) / defaultResolution.x * screenResolution.x, (defaultResolution.y / 2.0) / defaultResolution.y * screenResolution.y };
	vec2 scale = { (500.f / defaultResolution.x) * screenResolution.x, (500.f / defaultResolution.y) * screenResolution.y };
	createButton(pos_cat, scale,TEXTURE_IDS::BUTTONC, onClick);
	
	vec2 pos_dog = { (3.1f * defaultResolution.x / 4.f) / defaultResolution.x * screenResolution.x, (defaultResolution.y / 2.0) / defaultResolution.y * screenResolution.y };
	createButton(pos_dog, scale,TEXTURE_IDS::BUTTOND, onClick);
}

void WorldSystem::play_startscreen(std::vector<std::function<void()>> callbacks) {
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::START, 0.75);

	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = { (400.f / defaultResolution.x) * screenResolution.x, (80.f / defaultResolution.y) * screenResolution.y };
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1, onClick);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (500.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2, onClick);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (600.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3, onClick);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (700.f / defaultResolution.y) * screenResolution.y };
	createButton(pos4, scale,TEXTURE_IDS::BUTTON4, onClick);

}

void WorldSystem::play_options(std::vector<std::function<void()>> callbacks, int newtimer, int newPlayers) {
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::OPTIONS, -0.5);

	vec2 pos1 = {(defaultResolution.x - 980.f) / defaultResolution.x * screenResolution.x, (450.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = { (100.f / defaultResolution.x) * screenResolution.x, (100.f / defaultResolution.y) * screenResolution.y };
	createButton(pos1, scale,TEXTURE_IDS::BUTTONLA, onClick);

	vec2 pos2 = {(defaultResolution.x - 730.f) / defaultResolution.x * screenResolution.x, (450.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTONRA, onClick);
		
	vec2 pos3 = {(defaultResolution.x - 980.f) / defaultResolution.x * screenResolution.x, (670.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTONLB, onClick);

	vec2 pos4 = {(defaultResolution.x - 730.f) / defaultResolution.x * screenResolution.x, (670.f / defaultResolution.y) * screenResolution.y };
	createButton(pos4, scale,TEXTURE_IDS::BUTTONRB, onClick);

	vec2 pos5 = { (defaultResolution.x - 1520) / defaultResolution.x * screenResolution.x, (850.f / defaultResolution.y) * screenResolution.y };
	createButton(pos5, scale, TEXTURE_IDS::BUTTONGAME, onClick);
	
	vec2 pos6 = { (defaultResolution.x - 300.f) / defaultResolution.x * screenResolution.x, (850.f / defaultResolution.y) * screenResolution.y };
	createButton(pos6, scale, TEXTURE_IDS::BUTTONCANCEL, onClick);

	createTimerCounter(newtimer, textManager);
	//createText({ 1025.0f, 390.0f }, 2.0f, { 0.0f, 0.0f, 0.0f }, "20");
	
	createPlayersCounter(newPlayers,textManager);
	//createText({ 1045.0f, 615.0f }, 2.0f, { 0.0f, 0.0f, 0.0f }, "3");

}


void WorldSystem::play_levels(std::vector<std::function<void()>> callbacks) {
	std::vector<std::function<void()>> onClick;
	createMenu(MENU_TYPES::LEVELS, 0.75);

	vec2 pos1 = {(defaultResolution.x - 1400.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = { (100.f / defaultResolution.x) * screenResolution.x, (100 / defaultResolution.y) * screenResolution.y };
	createButton(pos1, scale,TEXTURE_IDS::BUTTONL1, onClick);

	vec2 pos2 = {(defaultResolution.x - 1000.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTONL2, onClick);

	vec2 pos3 = {(defaultResolution.x - 600.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTONL3, onClick);

}

void WorldSystem::remove_components() {
	if (registry.menus.size() > 0) {
		for (Entity e : registry.menus.entities) {
					registry.remove_all_components_of(e);
		}
	}
	if (registry.buttons.size() > 0) {
		for (Entity e : registry.buttons.entities) {
					registry.remove_all_components_of(e);
				}
	}
	if (registry.timer.size() > 0) {
		for (Entity e : registry.timer.entities) {
					registry.remove_all_components_of(e);
				}
	}
	if (registry.players.size() > 0) {
		for (Entity e : registry.players.entities) {
					registry.remove_all_components_of(e);
				}
	}
	if (registry.texts.size() > 0) {
		for (Entity e : registry.texts.entities) {
					registry.remove_all_components_of(e);
				}
	}

}