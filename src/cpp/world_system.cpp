// Header
#include "..\hpp\world_system.hpp"
#include "..\hpp\world_init.hpp"
#include "..\hpp\common.hpp"

// stlib
#include <cassert>

#include "..\hpp\tiny_ecs_registry.hpp"
#include <hpp/physics_system.hpp>
#include <hpp/options_system.hpp>
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
void WorldSystem::restart_game(Game level) {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	//Initialize current game
	current_game.init(window, mapSystem.getMap(level.getBackGround()), camera, textManager, level);
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

	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = scaleToScreenResolution(vec2(400.f,80.f));
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (500.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (600.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (700.f / defaultResolution.y) * screenResolution.y };
	createButton(pos4, scale,TEXTURE_IDS::BUTTON4);
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

void WorldSystem::check_for_button_presses() {

	//fk it build AABB
	for (Entity e : registry.buttons.entities) {
		Clickable button = registry.buttons.get(e);
		if (mouse_pos.x > button.vertecies[0].x && mouse_pos.x < button.vertecies[1].x && mouse_pos.y > button.vertecies[0].y && mouse_pos.y < button.vertecies[3].y) {

			if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON1) {
				remove_components();
				singlePlayer = true;
				play_levels();
				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON3) {
				remove_components();
				play_tutorial();
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON4) {

				remove_components();
				glfwSetWindowShouldClose(this->window, true);
				break;
			}

			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTON2) {

				remove_components();
				singlePlayer = false;
				play_select();
				break;
			}
		
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::HOWTOMOVE) {

				remove_components();
				play_startscreen();

				break;
			}
			
			else if ((registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONC) || (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTOND)) {
				remove_components();
				// change option timer type
				play_options(20.0f);
				break;
			}
			
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONLA) {

				float newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = decreaseTimer(e, 5.f);
				}

				remove_components();
				play_options(newtimer);

				break;
			}
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONRA) {

				float newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = increaseTimer(e, 5.f);
				}

				remove_components();
				play_options(newtimer);

				break;
			}
			// else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONLB) {

			// 	float newtimer;
			// 	for (Entity e : registry.timer.entities) {
			// 		OptionTimer timer = registry.timer.get(e);
			// 		newtimer = timer.timerC;
			// 	}

			// 	int newplayers;
			// 	for (Entity e : registry.players.entities) {
			// 		OptionPlayers players = registry.players.get(e);
			// 		newplayers = decreasePlayers(e, 1);
			// 	}

			// 	remove_components();
			// 	play_options(newtimer, newplayers);

			// 	break;
			// }

			// else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONRB) {

			// 	float newtimer;
			// 	for (Entity e : registry.timer.entities) {
			// 		OptionTimer timer = registry.timer.get(e);
			// 		newtimer = timer.timerC;
			// 	}

			// 	int newplayers;
			// 	for (Entity e : registry.players.entities) {
			// 		OptionPlayers players = registry.players.get(e);
			// 		newplayers = increasePlayers(e, 1);
			// 	}

			// 	remove_components();

			// 	play_options(newtimer, newplayers);
			// 	break;
			// }

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONGAME) {
				remove_components();
				play_levels();
				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONCANCEL) {
				remove_components();
				// change option timer type
				play_startscreen();

				break;
			}
			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL1) {
				float newtimer = 12.f;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}
				remove_components();
				restart_game(level_one(newtimer));
				audio.play_music(INDUSTRIAL);
				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL2) {
				float newtimer = 12.f;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}
				remove_components();
				restart_game(level_two(newtimer));
				audio.play_music(CYBERPUNK);
				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONL3) {
				float newtimer = 12.f;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}
				remove_components();
				restart_game(level_three(newtimer));// audio.stop_music();
				audio.play_music(CYBERPUNK);
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
	auto mouse_input = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(this->window, mouse_input);
}

void WorldSystem::play_tutorial() {
	createButton(vec2{ screenResolution.x / 2, screenResolution.y / 2 }, vec2{ screenResolution.x, screenResolution.y },TEXTURE_IDS::HOWTOMOVE);
	cancel_button();
}

void WorldSystem::play_select() {
	createMenu(MENU_TYPES::SELECT, 0.75);

	vec2 pos_cat = { (defaultResolution.x / 4.f) / defaultResolution.x * screenResolution.x, (defaultResolution.y / 2.0) / defaultResolution.y * screenResolution.y };
	vec2 scale = set_scale(500.f,500.f);
	createButton(pos_cat, scale,TEXTURE_IDS::BUTTONC);
	
	vec2 pos_dog = { (3.1f * defaultResolution.x / 4.f) / defaultResolution.x * screenResolution.x, (defaultResolution.y / 2.0) / defaultResolution.y * screenResolution.y };
	createButton(pos_dog, scale,TEXTURE_IDS::BUTTOND);

	cancel_button();
}

void WorldSystem::play_startscreen() {
	createMenu(MENU_TYPES::START, 0.75);

	vec2 pos1 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (400.f / defaultResolution.y) * screenResolution.y };
	vec2 scale = scaleToScreenResolution(vec2(400.f,80.f));
	createButton(pos1, scale,TEXTURE_IDS::BUTTON1);

	vec2 pos2 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (500.f / defaultResolution.y) * screenResolution.y };
	createButton(pos2, scale,TEXTURE_IDS::BUTTON2);

	vec2 pos3 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (600.f / defaultResolution.y) * screenResolution.y };
	createButton(pos3, scale,TEXTURE_IDS::BUTTON3);

	vec2 pos4 = {(defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (700.f / defaultResolution.y) * screenResolution.y };
	createButton(pos4, scale,TEXTURE_IDS::BUTTON4);

}

// change option timer type
void WorldSystem::play_options(float newtimer) {
	createMenu(MENU_TYPES::OPTIONS, -0.5);

	vec2 pos1 = set_pos(980.f, 450.f);
	vec2 scale = set_scale(100.f,100.f);
	createButton(pos1, scale, TEXTURE_IDS::BUTTONLA);

	vec2 pos2 = set_pos(730.f, 450.f);
	createButton(pos2, scale, TEXTURE_IDS::BUTTONRA);

	// vec2 pos3 = set_pos(980.f, 670.f);
	// createButton(pos3, scale, TEXTURE_IDS::BUTTONLB);

	// vec2 pos4 = set_pos(730.f,670.f);
	// createButton(pos4, scale, TEXTURE_IDS::BUTTONRB);

	vec2 pos5 = set_pos(240.f,910.f);
	vec2 scale6 = set_scale(224.f,87.f);
	createButton(pos5, scale6, TEXTURE_IDS::BUTTONGAME);

	cancel_button();

	createTimerCounter(newtimer, textManager);

//	createPlayersCounter(newPlayers, textManager);

}


void WorldSystem::play_levels() {
	createMenu(MENU_TYPES::LEVELS, 0.75);

	vec2 pos1 = set_pos(1400.f, 400.f);
	vec2 scale = set_scale(100.f,100.f);
	createButton(pos1, scale,TEXTURE_IDS::BUTTONL1);

	vec2 pos2 = set_pos(1000.f,400.f);
	createButton(pos2, scale,TEXTURE_IDS::BUTTONL2);

	vec2 pos3 = set_pos(600.f, 400.f);
	createButton(pos3, scale,TEXTURE_IDS::BUTTONL3);

	cancel_button();

}

Game WorldSystem::level_one(float newTimer) {
	Game level_one;
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	if (singlePlayer) {
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	else {
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	level_one.setTimer(newTimer * 1000.f);
	return level_one;
}

Game WorldSystem::level_two(float newTimer) {
	Game level_one;
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	if (singlePlayer) {
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	else {
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	level_one.setTimer(newTimer * 1000.f);
	level_one.setBackGround(MAPS::MIAMI);
	return level_one;
}

Game WorldSystem::level_three(float newTimer) {

	Game level_one;
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	if (singlePlayer) {
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	else {
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	level_one.setTimer(newTimer * 1000.f);
	level_one.setBackGround(MAPS::CYBERPUNK);
	return level_one;

}

Game WorldSystem::multiplayer(float newtimer) {
	Game multiplayer;
	multiplayer.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	multiplayer.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	multiplayer.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	
	// set time to option timer
	multiplayer.setTimer(newtimer * 1000.f);

	return multiplayer;
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
	if (registry.texts.size() > 0) {
		for (Entity e : registry.texts.entities) {
			registry.remove_all_components_of(e);
		}
	}
}

vec2 WorldSystem::set_pos(float posX, float posY){
	return {(defaultResolution.x - posX) / defaultResolution.x * screenResolution.x, (posY / defaultResolution.y) * screenResolution.y };
}

vec2 WorldSystem::set_scale(float scaleX, float scaleY){
	return scaleToScreenResolution(vec2(scaleX,scaleY));
}

Entity WorldSystem::cancel_button(){
	vec2 pos6 = set_pos(1700.f, 920.f);
	vec2 scale6 = set_scale(224.f,87.f);
	return createButton(pos6, scale6, TEXTURE_IDS::BUTTONCANCEL);
}

