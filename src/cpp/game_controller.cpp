#include <hpp/game_controller.hpp>
#include <hpp/world_init.hpp>

#include "hpp/ai_system.hpp"
#include "hpp/Game_Mechanics/health_system.hpp"

#include <glm/vec2.hpp>	
#include <hpp/tiny_ecs_registry.hpp>

GameController::GameController() {
	inAGame = false;
	//ShootingSystem shooting_system(renderer);
}

GameController::~GameController() {
	//do something when this object is destroyed
}

//initialize stuff here
void GameController::init(RenderSystem* renderer, GLFWwindow* window) {
	//Set renderer
	this->renderer = renderer;
	this->window = window;

	//Init game metadata
	game_state.turn_number += 1;
	game_state.turn_possesion = TURN_CODE::PLAYER1;

	//Building the stage via json
	build_map();

	//Building the teams via json
	numPlayersInTeam = 1; // Default number of players for each team
	init_player_teams();

	//Setting player key callback
	//glfwSetWindowUserPointer(window, this);
	set_user_input_callbacks();

	inAGame = true;
	player_mode = PLAYER_MODE::MOVING;

	this->shooting_system.init(renderer);
	this->timePerTurnMs = 20000.0;

	ai.init(shooting_system);

	//TEST TEXT
	createText({ 10.0f, 10.0f }, 1.0f, { 0.172f, 0.929f, 0.286f }, "~The quick brown fox~ $jumped over the lazy$ dog !., 1234567890");
	ai.init(shooting_system);
}

void GameController::step(float elapsed_ms) {
	//While a game is happening make sure the players are controlling from here
	glfwSetWindowUserPointer(window, this);
	//While a game is happening make sure the players are controlling from here
	shooting_system.step(elapsed_ms);
	ui.step(elapsed_ms);

	handle_collisions();

	if (game_state.turn_possesion == PLAYER1) {
		//DO STUFF
	} else if (game_state.turn_possesion == PLAYER2) {
		//DO STUFF
	} else if (game_state.turn_possesion == AI) {
		//DO STUFF
	} else if (game_state.turn_possesion == NPCAI_TURN) {
		//DO STUFF
	}

	// change the animation type depending on the velocity
	for (Entity e : registry.animations.entities) {
		Motion& catMotion = registry.motions.get(e);
		Animation& catAnimation = registry.animations.get(e);

		if (catMotion.velocity.x < 0) {
				catAnimation.facingLeft = true;
				shooting_system.setAimLoc(e);
		}
		if (catMotion.velocity.x > 0) {
			catAnimation.facingLeft = false;
			shooting_system.setAimLoc(e);
		}
	}

	for (int i = 0; i < player1_team.size(); i++) {
		auto& e = player1_team[i];
		if (registry.health.get(e).hp == 0) {
			player1_team.erase(player1_team.begin() + i);
			registry.remove_all_components_of(e);
			inAGame = false;
		}

		// if (player1_team.size() == 0) {
		// 	restart_current_match();
		// }
	}

	for (int i = 0; i < npcai_team.size(); i++) {
		auto e = npcai_team[i];
		if (registry.health.get(e).hp == 0) {
			npcai_team.erase(npcai_team.begin() + i);
			registry.remove_all_components_of(e);
		}

		// if (npcai_team.size() == 0) {
		// 	restart_current_match();
		// }
	}


	ai.step(elapsed_ms, game_state.turn_possesion);
	// if (game_state.turn_possesion == TURN_CODE::NPCAI) next_turn();
	decrementTurnTime(elapsed_ms);
}

void GameController::decrementTurnTime(float elapsed_ms) {
	if (timePerTurnMs <= 0) {
		next_turn();
		timePerTurnMs = 20000.0;
	} else {
		timePerTurnMs -= elapsed_ms;
	}
}


void GameController::build_map() {
	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();

	// Move the walls off screen
	// Floor
	// createWall({ width / 2, height + 10 }, width, 10);
	
	// //Left Wall
	createWall({ -10, height / 2 }, 10, height - 10);
	//
	// //Right Wall
	createWall({ width + 10, height / 2 }, 10, height);
	//
	// //Ceiling
	createWall({ width / 2, -10 }, width, 10);

	this->gameMap = Map();
	gameMap.init(renderer->getScreenWidth());
	renderer->setTileMap(gameMap);
}

void GameController::init_player_teams() {

	if (numPlayersInTeam < 1) {
		printf("CRITICAL ERROR: NO TEAMS CAN BE INITIALIZED");
		assert(false);
	}

	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();

	// Init the player team
	// If our game gets more complex I'd probably abstract this out an have an Entity hierarchy -Fred
	for (int i = 0; i < numPlayersInTeam; i++) {
		Entity player_cat = createCat(renderer, { width / 2 - 200, height - 400 });
		player1_team.push_back(player_cat);
		curr_selected_char = player_cat;
	}

	// Init npcai team
	// NOTE: We should add some kind of bool to check if we should init a specific team,
	// and then add the contents of this loop to the loop above
	Entity ai_cat0 = createAI(renderer, { width - 400,300 });
	Entity ai_cat1 = createAI(renderer, { width - 200,300 });
	npcai_team.push_back(ai_cat0);
	npcai_team.push_back(ai_cat1);

	//This needs to be in order
	teams.push_back(player1_team);
	teams.push_back(player2_team);
	teams.push_back(ai_team);
	teams.push_back(npcai_team);
}

void GameController::next_turn() {
	//Turn order will ALWAYS be PLAYER1 -> PLAYER2 -> ENEMY_AI -> (not implemented) AI
	//IF THERE IS NO ONE ON A TEAM, THE GAME CONTROLLER WILL MOVE ON TO THE NEXT TURN

	game_state.turn_possesion += 1;
	game_state.turn_number += 1;
	
	for (Entity e : registry.projectiles.entities) {
		registry.remove_all_components_of(e);
	}

	if (game_state.turn_possesion == TURN_CODE::END) {
		game_state.turn_possesion = TURN_CODE::PLAYER1;
	} else if (teams[game_state.turn_possesion].empty()) {
		game_state.turn_number -= 1;
		next_turn();
	}
}

void GameController::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
		
		if (registry.projectiles.has(entity)) {// Projectile hit terrain
			Projectile& pj = registry.projectiles.get(entity);
			if (registry.terrains.has(entity_other) && entity_other != pj.origin) {
				registry.remove_all_components_of(entity);
			} else if (entity_other != pj.origin) { // Projectile hit another player
				for (std::vector<Entity> vec : teams) {
					bool origin_isonteam = false;
					bool entity_other_isonteam = false;
					for (Entity e : vec) { //check for friendly fire, since std::find dosen't work
						if (e == pj.origin) 
							origin_isonteam = true;
						if (e == entity_other) 
							entity_other_isonteam = true;
					}
					if (origin_isonteam && !entity_other_isonteam)
						decreaseHealth(entity_other, registry.weapons.get(pj.origin).damage);
				}
				registry.remove_all_components_of(entity);
			}
		}

		if (registry.motions.has(entity) && registry.rigidBodies.has(entity) && registry.terrains.has(entity_other)) {
			Rigidbody& rb = registry.rigidBodies.get(entity);
			Motion& motion = registry.motions.get(entity);

			if (rb.collision_normal.y == -1) {
				motion.velocity.y = 0;
			}
		
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// On key callback
void GameController::on_player_key(int key, int, int action, int mod) {

	//Only allowed to move on specified turn
	if (game_state.turn_possesion == PLAYER1 && inAGame) {
		Motion& catMotion = registry.motions.get(curr_selected_char);
		Rigidbody& rb = registry.rigidBodies.get(curr_selected_char);

		float current_speed = 150.0f;
		float gravity_force = 2.5;

		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_W) {
				if (catMotion.velocity.y == gravity_force) {
					catMotion.velocity.y = -gravity_force * current_speed;
					rb.collision_normal.y = 0;
					player_mode = PLAYER_MODE::MOVING;
					ui.hide_crosshair();
				}
			}

			if (key == GLFW_KEY_D) {
				catMotion.velocity.x = current_speed;
				AnimationSystem::animate_cat_walk(curr_selected_char);
				player_mode = PLAYER_MODE::MOVING;
				ui.hide_crosshair();
			}

			if (key == GLFW_KEY_A) {
				catMotion.velocity.x = -current_speed;
				AnimationSystem::animate_cat_walk(curr_selected_char);
				player_mode = PLAYER_MODE::MOVING;
				ui.hide_crosshair();
			}
		}


		if (action == GLFW_RELEASE) {
			if (key == GLFW_KEY_A && catMotion.velocity.x < 0) {
				catMotion.velocity.x = 0.0f;
				AnimationSystem::animate_cat_idle(curr_selected_char);
				player_mode = PLAYER_MODE::SHOOTING;
				ui.show_crosshair(curr_selected_char);
			}
			if (key == GLFW_KEY_D && catMotion.velocity.x > 0) {
				catMotion.velocity.x = 0.0f;
				AnimationSystem::animate_cat_idle(curr_selected_char);
				player_mode = PLAYER_MODE::SHOOTING;
				ui.show_crosshair(curr_selected_char);
			}
		}


		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			shooting_system.aimUp(curr_selected_char, 0.05f);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			shooting_system.aimDown(curr_selected_char, 0.05f);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
			// should only shoot when standing
			if (catMotion.velocity.y == gravity_force && catMotion.velocity.x == 0.0) {
				shooting_system.shoot(curr_selected_char);
			}
		}
	}
}

void GameController::on_mouse_move(vec2 mouse_pos) {
	(void)mouse_pos;
	// printf("now in game_controller");
}

void GameController::on_mouse_click(int button, int action, int mods) {

	if (action == GLFW_PRESS) {
		printf("In A Game");
	}
}

void GameController::set_user_input_callbacks() {
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GameController*)glfwGetWindowUserPointer(wnd))->on_player_key(_0, _1, _2, _3); };
	glfwSetKeyCallback(this->window, key_redirect);
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((GameController*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(this->window, cursor_pos_redirect);
	auto mouse_input = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((GameController*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(this->window, mouse_input);
}
