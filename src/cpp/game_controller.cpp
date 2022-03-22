#include <hpp/game_controller.hpp>
#include <hpp/world_init.hpp>

#include "hpp/ai_system.hpp"
#include "hpp/Game_Mechanics/health_system.hpp"

#include <glm/vec2.hpp>	
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/components.hpp>



GameController::GameController() {
	inAGame = false;
}

GameController::~GameController() {
	//do something when this object is destroyed
}

//initialize stuff here
void GameController::init(GLFWwindow* window, MapSystem::Map& map, OrthographicCamera& camera, TextManager& textManager, Game game) {
	this->window = window;
	this->gameMap = map;
	this->camera = &camera;
	this->textManager = textManager;
	this->game_data = game;
	this->mousePosition = scaleToScreenResolution(defaultResolution/2.f);
	this->mouseDeadzone = scaleToScreenResolution({ 250.f, 250.f }).x;
	this->mouseTriggerArea = scaleToScreenResolution({ 300.f, 150.f });


	//Init game metadata
	game_state.turn_number += 1;
	game_state.turn_possesion = TURN_CODE::PLAYER1;

	//Building the stage via json
	build_map();

	//Building the teams via json
	init_player_teams();

	//Setting player key callback
	//glfwSetWindowUserPointer(window, this);
	set_user_input_callbacks();

	timePerTurnMs = game_data.getTimer();

	inAGame = true;
	player_mode = PLAYER_MODE::MOVING;

	ai.init(shooting_system);

	ui.init(textManager);

	turnIndicatorScale = scaleToScreenResolution({ 2.0f, 2.f }).x;
	turnIndicator = createText(textManager, "", turnPosition, turnIndicatorScale, redColor);

	timerScale = scaleToScreenResolution({ 1.5f, 1.5f }).x;
	timeIndicator = createText(textManager, "", scaleToScreenResolution({ 2 * defaultResolution.x / 4.f + this->camera->getPosition().x, 110.0f }), timerScale, { 0.172f, 0.929f, 0.286f });
}

void GameController::step(float elapsed_ms) {
	//While a game is happening make sure the players are controlling from here
	glfwSetWindowUserPointer(window, this);

	//Pan camera based on mouse position
	moveCamera();

	shooting_system.step(elapsed_ms);
	ui.step(elapsed_ms);

	handle_collisions();

	turnPosition = scaleToScreenResolution({ 2 * defaultResolution.x / 4.f + camera->getPosition().x,  30.0f });
	auto& turnIndicatorText = registry.texts.get(turnIndicator);
	auto& turnIndicatorPosition = registry.motions.get(turnIndicator).position;

	if (game_state.turn_possesion == PLAYER1) {
		turnIndicatorText.text = "PLAYER 1'S TURN";
		turnIndicatorPosition = turnPosition;
		turnIndicatorPosition.x = turnIndicatorPosition.x - turnIndicatorText.scale.x / 2.f;
		turnIndicatorText.color = redColor;
	} else if (game_state.turn_possesion == PLAYER2) {
		turnIndicatorText.text = "PLAYER 2'S TURN";
		turnIndicatorPosition = turnPosition;
		turnIndicatorPosition.x = turnIndicatorPosition.x - turnIndicatorText.scale.x / 2.f;
	} else if (game_state.turn_possesion == AI) {
		turnIndicatorText.text = "COMPUTER'S TURN";
		turnIndicatorPosition = turnPosition;
		turnIndicatorPosition.x = turnIndicatorPosition.x - turnIndicatorText.scale.x / 2.f;
	} else if (game_state.turn_possesion == NPCAI_TURN) {
		turnIndicatorText.text = "COMPUTER'S TURN";
		turnIndicatorPosition = turnPosition;
		turnIndicatorPosition.x = turnIndicatorPosition.x - turnIndicatorText.scale.x / 2.f;
		turnIndicatorText.color = darkGreenColor;


	}

	for (Entity e : teams[game_state.turn_possesion]) {
		shooting_system.setAimLoc(e);
	}

	for (int i = 0; i < player1_team.size(); i++) {
		auto e = player1_team[i];
		if (registry.health.get(e).hp == 0) {
			player1_team.erase(player1_team.begin() + i);

			registry.remove_all_components_of(e);
			inAGame = false;
		}

		// auto rightDist = abs(registry.motions.get(e).position.x - camera->getCameraRight().x);
		// auto leftDist = abs(registry.motions.get(e).position.x - camera->getPosition().x);
		// if (rightDist < 400.f && camera->getCameraRight().x < 2 * screenResolution.x) {
		// 	camera->setPosition(camera->getPosition() + vec3(1.5f, 0.f, 0.f));
		// } else if (leftDist < 400.f && camera->getPosition().x > 0.f) {
		// 	camera->setPosition(camera->getPosition() + vec3(-1.5f, 0.f, 0.f));
		// }

		// if (player1_team.size() == 0) {
		// 	restart_current_match();
		// }
	}

	for (int i = 0; i < npcai_team.size(); i++) {
		auto e = npcai_team[i];
		if (registry.health.get(e).hp == 0) {
			npcai_team.erase(npcai_team.begin() + i);
			remove_children(e);
			registry.remove_all_components_of(e);
		}

	}


	ai.step(elapsed_ms, game_state.turn_possesion);
	// if (game_state.turn_possesion == TURN_CODE::NPCAI) next_turn();
	decrementTurnTime(elapsed_ms);

}

void GameController::moveCamera() {
	auto mousePos = mousePosition.x + camera->getPosition().x;
	auto rightDist = abs(mousePos - camera->getCameraRight().x);
	auto leftDist = abs(mousePos - camera->getPosition().x);

	if (mousePosition.y > mouseDeadzone) {
		if (camera->getCameraRight().x < 2 * screenResolution.x) {
			if (rightDist < mouseTriggerArea.x && rightDist > mouseTriggerArea.y) {
				camera->setPosition(camera->getPosition() + vec3(3.f, 0.f, 0.f));
			} else if (rightDist < mouseTriggerArea.y) {
				camera->setPosition(camera->getPosition() + vec3(6.f, 0.f, 0.f));
			}
		}

		if (camera->getPosition().x > 0.f) {
			if (leftDist < mouseTriggerArea.x && leftDist > mouseTriggerArea.y) {
				camera->setPosition(camera->getPosition() + vec3(-3.f, 0.f, 0.f));
			} else if (leftDist < mouseTriggerArea.y) {
				camera->setPosition(camera->getPosition() + vec3(-6.f, 0.f, 0.f));
			}
		}
	}
}


void GameController::decrementTurnTime(float elapsed_ms) {
	// registry.remove_all_components_of(timeIndicator);
	uint timePerTurnSec = uint(timePerTurnMs / 1000);
	if (timePerTurnMs <= 0) {
		next_turn();
		timePerTurnMs = 20000.0;
	} else {
		timePerTurnMs -= elapsed_ms;
	}
	auto& timerPosition = registry.motions.get(timeIndicator);
	auto& timerText = registry.texts.get(timeIndicator);
	timerText.text = std::to_string(timePerTurnSec) + " seconds left!";
	timerPosition.position = scaleToScreenResolution({ 2 * defaultResolution.x / 4.f + camera->getPosition().x, 110.0f });
	timerPosition.position.x = timerPosition.position.x - timerText.scale.x / 2.f;
}


void GameController::build_map() {
	const int width = screenResolution.x;
	const int height = screenResolution.y;
	
	//Left Wall
	createWall({ -10, height / 2 }, 10, height - 10);

	//Right Wall
	createWall({ 2 * width + 10, height / 2 }, 10, height);

	gameMap.build();
}

void GameController::init_player_teams() {

	for (Game::Character character : game_data.getCharacters()) {
		Entity e = character.animal == ANIMAL::CAT ? createCat(character.weapon, character.starting_pos, character.health) : createDog(character.weapon, character.starting_pos, character.health);

		if (character.alignment == TEAM::PLAYER_1_TEAM) {
			player1_team.push_back(e);
		} else if (character.alignment == TEAM::PLAYER_2_TEAM) {
			player2_team.push_back(e);
		} else if (character.alignment == TEAM::AI_TEAM) {
			registry.ais.emplace(e);
			ai_team.push_back(e);
		} else {
			registry.ais.emplace(e);
			npcai_team.push_back(e);
		}
	}

	//This needs to be in order
	teams.push_back(player1_team);
	teams.push_back(player2_team);
	teams.push_back(ai_team);
	teams.push_back(npcai_team);
	change_curr_selected_char(player1_team[0]);
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

		// Player stops moving
		for (Entity e : player1_team) {
			auto& motion = registry.motions.get(e);
			motion.velocity.x = 0;
		}
		next_turn();
	}
	change_curr_selected_char(teams[game_state.turn_possesion][0]); //supposed to be the first player on each team
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
					if (origin_isonteam && !entity_other_isonteam) {
						decreaseHealth(entity_other, registry.weapons.get(pj.origin).damage, curr_selected_char);
					}
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

void GameController::change_selected_state(Entity e, bool state) {
	Selected& selparent = registry.selected.get(e); //unselect parent
	selparent.isSelected = state;
	std::vector<Entity> children = get_all_children(e);
	for (Entity child : children) {
		if (registry.selected.has(child)) {
			Selected& sel = registry.selected.get(child); //unselect all children attached
			sel.isSelected = state;
		}
	}
}

void GameController::change_curr_selected_char(Entity e) {

	curr_selected_char = e;

	for (std::vector<Entity> team : teams) {
		for (Entity player : team) {
			if (curr_selected_char != player && registry.renderRequests.has(player)) {
				change_selected_state(player, false);
			} else if (curr_selected_char == player) {
				change_selected_state(player, true);
			}
		}
	}

}
void GameController::change_to_next_char_on_team() {

	//so scuffed lmao
	bool next_char = false;

	for (Entity e : teams[game_state.turn_possesion]) {
		if (e == curr_selected_char) {
			next_char = true;
		}
		if (next_char) {
			change_curr_selected_char(e);
		}
	}

}

// On key callback
void GameController::on_player_key(int key, int, int action, int mod) {
	//Only allowed to move on specified turn
	if (game_state.turn_possesion == PLAYER1 && inAGame) {
		if (registry.cats.has(curr_selected_char)) {
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
						AnimationSystem::animate_cat_jump(curr_selected_char);
						//AnimationSystem::animate_dog_jump(curr_selected_char);
						ui.hide_crosshair();
					}
				}
				if (key == GLFW_KEY_D) {
					catMotion.velocity.x = current_speed;
					AnimationSystem::animate_cat_walk(curr_selected_char);
					//AnimationSystem::animate_dog_walk(curr_selected_char);
					player_mode = PLAYER_MODE::MOVING;
					ui.hide_crosshair();
				}
				if (key == GLFW_KEY_A) {
					catMotion.velocity.x = -current_speed;
					AnimationSystem::animate_cat_walk(curr_selected_char);
					//AnimationSystem::animate_dog_walk(curr_selected_char);
					player_mode = PLAYER_MODE::MOVING;
					ui.hide_crosshair();
				}
				if (key == GLFW_KEY_N) {
					ui.hide_crosshair();
					remove_children(curr_selected_char);
					registry.remove_all_components_of(curr_selected_char);
				}
			}



			if (action == GLFW_RELEASE) {
				if (key == GLFW_KEY_A && catMotion.velocity.x < 0) {
					catMotion.velocity.x = 0.0f;
					AnimationSystem::animate_cat_aim(curr_selected_char);
					//AnimationSystem::animate_dog_idle(curr_selected_char);
					player_mode = PLAYER_MODE::SHOOTING;
					ui.show_crosshair(curr_selected_char);
				}
				if (key == GLFW_KEY_D && catMotion.velocity.x > 0) {
					catMotion.velocity.x = 0.0f;
					AnimationSystem::animate_cat_aim(curr_selected_char);
					//AnimationSystem::animate_dog_idle(curr_selected_char);
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
}

void GameController::on_mouse_move(vec2 mouse_pos) {
	this->mousePosition = mouse_pos;
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
