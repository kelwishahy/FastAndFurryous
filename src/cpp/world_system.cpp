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
	quit = false;
	this->window = window;
	this->camera = OrthographicCamera(0.f, screenResolution.x, screenResolution.y, 0.f);

	this->mapSystem = MapSystem();
	this->mapSystem.init();

	this->textManager = TextManager();
	textManager.initFonts();

	this->particleSystem = ParticleSystem();
	printf("2. Particle entities is %d\n", (int)registry.particles.entities.size());

	init_main_menu();
	audio.play_music(MUSIC_LIST::IN_GAME_BACKGROUND);

	set_user_input_callbacks();

	printf("window height is: %f px, window width is: %f px\n", screenResolution.x, screenResolution.y);
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	if (current_game.inAGame) {
		current_game.step(elapsed_ms_since_last_update);
	} else if (current_game.quit == true) {
		this->quit = true;
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game(Game level) {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	//Initialize current game
	current_game.init(window, mapSystem.getMap(level.getBackGround()), camera, textManager, level, particleSystem);
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

	vec2 pos5 = { (defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (800.f / defaultResolution.y) * screenResolution.y };
	createButton(pos5, scale, TEXTURE_IDS::BUTTON5);
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
			TEXTURE_IDS tex = registry.renderRequests.get(e).texture;

			if (tex == TEXTURE_IDS::BUTTON1) {
				remove_components();
				singlePlayer = true;
				play_levels();
				break;
			}

			if (tex == TEXTURE_IDS::BUTTON3) {
				remove_components();
				play_tutorial();
				break;
			}
			
			if (tex == TEXTURE_IDS::BUTTON4) {

				remove_components();
				glfwSetWindowShouldClose(this->window, true);
				break;
			}

			
			if (tex == TEXTURE_IDS::BUTTON2) {
				
				remove_components();
				singlePlayer = false;
				play_select();
				break;
			}

			if (tex == TEXTURE_IDS::BUTTON5) {

				remove_components();
				audio.stop_music();
				restart_game(tutorial_level());// audio.stop_music();
				break;
			}
		
			if (tex == TEXTURE_IDS::HOWTOMOVE) {

				remove_components();
				play_startscreen();

				break;
			}
			
			if (tex == TEXTURE_IDS::BUTTONC) {
				remove_components();
				// change option timer type
				play_options(20.0f);
				break;
			}
			
			if (tex == TEXTURE_IDS::BUTTONLA) {

				float newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = decreaseTimer(e, 5.f);
				}

				remove_components();
				play_options(newtimer);

				break;
			}
			if (tex == TEXTURE_IDS::BUTTONRA) {

				float newtimer;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = increaseTimer(e, 5.f);
				}

				remove_components();
				play_options(newtimer);

				break;
			}

			else if (registry.renderRequests.get(e).texture == TEXTURE_IDS::BUTTONGAME) {
				float newtimer = 12.f;
				for (Entity e : registry.timer.entities) {
					OptionTimer timer = registry.timer.get(e);
					newtimer = timer.timerC;
				}
				remove_components();
				multiplayer_play_levels(newtimer);
				break;
				
			}

			if (tex == TEXTURE_IDS::BUTTONCANCEL) {
				remove_components();
				// change option timer type
				play_startscreen();

				break;
			}
			if (tex == TEXTURE_IDS::BUTTONL1) {
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

			if (tex == TEXTURE_IDS::BUTTONL2) {
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

			if (tex == TEXTURE_IDS::BUTTONL3) {
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

	vec2 pos_cat = set_pos(240.f,920.f);
	vec2 scale = set_scale(224.f,87.f);
	createButton(pos_cat, scale,TEXTURE_IDS::BUTTONC);
	
	// vec2 pos_dog = { (3.1f * defaultResolution.x / 4.f) / defaultResolution.x * screenResolution.x, (defaultResolution.y / 2.0) / defaultResolution.y * screenResolution.y };
	// createButton(pos_dog, scale,TEXTURE_IDS::BUTTOND);

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

	vec2 pos5 = { (defaultResolution.x - 480.f) / defaultResolution.x * screenResolution.x, (800.f / defaultResolution.y) * screenResolution.y };
	createButton(pos5, scale, TEXTURE_IDS::BUTTON5);

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
	vec2 scale = set_scale(100.f, 100.f);
	createButton(pos1, scale, TEXTURE_IDS::BUTTONL1);

	vec2 pos2 = set_pos(1000.f, 400.f);
	createButton(pos2, scale, TEXTURE_IDS::BUTTONL2);

	vec2 pos3 = set_pos(600.f, 400.f);
	createButton(pos3, scale, TEXTURE_IDS::BUTTONL3);

	cancel_button();

}


void WorldSystem::multiplayer_play_levels(float newtimer) {
	createMenu(MENU_TYPES::LEVELS, 0.75);

	vec2 pos1 = set_pos(1400.f, 400.f);
	vec2 scale = set_scale(100.f,100.f);
	createButton(pos1, scale,TEXTURE_IDS::BUTTONL1);

	vec2 pos2 = set_pos(1000.f,400.f);
	createButton(pos2, scale,TEXTURE_IDS::BUTTONL2);

	vec2 pos3 = set_pos(600.f, 400.f);
	createButton(pos3, scale,TEXTURE_IDS::BUTTONL3);

	cancel_button();

	auto entity = Entity();
	OptionTimer& timer0 = registry.timer.emplace(entity);
	timer0.timerC = newtimer;
}

Game WorldSystem::level_one(float newTimer) {
	Game level_one;
	level_one.addCat(SHOTGUN, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
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
	level_one.addCat(LAUNCHER, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	level_one.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	if (singlePlayer) {
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, NPC_AI_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	else {
		level_one.addDog(LAUNCHER, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
		level_one.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	}
	level_one.setTimer(newTimer * 1000.f);
	level_one.setBackGround(MAPS::CYBERPUNK);
	return level_one;

}

Game WorldSystem::multiplayer(float newtimer) {
	Game multiplayer;
	multiplayer.addCat(LAUNCHER, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	multiplayer.addCat(SHOTGUN, PLAYER_1_TEAM, { screenResolution.x / 2 - 100, screenResolution.y - 400 }, 100);
	multiplayer.addCat(AWP, PLAYER_1_TEAM, { screenResolution.x / 2 + 100, screenResolution.y - 800 }, 100);

	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x - 200, screenResolution.y - 800 }, 100);
	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 400, screenResolution.y - 1000 }, 100);
	multiplayer.addDog(RIFLE, PLAYER_2_TEAM, { screenResolution.x + 600, screenResolution.y - 400 }, 100);
	
	// set time to option timer
	multiplayer.setTimer(newtimer * 1000.f);

	return multiplayer;
}

Game WorldSystem::tutorial_level() {
	Game tutorial;
	tutorial.addCat(RIFLE, PLAYER_1_TEAM, { screenResolution.x / 2 - 400, screenResolution.y - 800 }, 100);
	tutorial.addDog(RIFLE, PLAYER_2_TEAM, scaleToScreenResolution({ 3400.0f, 400.0f }), 999);

	// set time to option timer
	tutorial.setBackGround(MAPS::TUTORIAL);
	tutorial.setTimer(600000.f); //10 mins
	tutorial.tutorial = true;

	return tutorial;
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
	vec2 pos6 = set_pos(1700.f, 930.f);
	vec2 scale6 = set_scale(224.f,87.f);
	return createButton(pos6, scale6, TEXTURE_IDS::BUTTONCANCEL);
}

WEAPON_TYPES WorldSystem::generate_random_weapon() {
	std::srand(std::time(NULL));
	WEAPON_TYPES weapon =(WEAPON_TYPES)(std::rand() % 4);
	return weapon;
}

