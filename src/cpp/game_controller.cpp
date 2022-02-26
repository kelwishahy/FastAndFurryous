#include <hpp/game_controller.hpp>
#include <hpp/world_init.hpp>

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
	init_player_teams();

	//Setting player key callback
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GameController*)glfwGetWindowUserPointer(wnd))->on_player_key(_0, _1, _2, _3); };
	glfwSetKeyCallback(this->window, key_redirect);

	inAGame = true;
	player_mode = PLAYER_MODE::MOVING;

	this->shooting_system.init(renderer);
}

void GameController::step(float elapsed_ms)
{
	//While a game is happening make sure the players are controlling from here
	glfwSetWindowUserPointer(window, this);
	shooting_system.step(elapsed_ms);
	handle_collisions();

	// change the animation type depending on the velocity
	Motion& catMotion = registry.motions.get(player1_team.front());
	Player& catPlayer = registry.players.get(player1_team.front());
	if (catMotion.velocity.x == 0) {
		catPlayer.animation_type = IDLE;
	}
	if (catMotion.velocity.x != 0) {
		catPlayer.animation_type = WALKING;
	}
	if (catMotion.velocity.y < 0) {
		catPlayer.animation_type = JUMPING;
	}
	if (catMotion.velocity.x < 0) {
		catPlayer.facingLeft = true;
	}
	if (catMotion.velocity.x > 0) {
		catPlayer.facingLeft = false;
	}

	// // FOR AI Animation
	// Motion& aiMotion = registry.motions.get(ai_cat);
	// Player& aiCat = registry.players.get(ai_cat);
	// if (aiMotion.velocity.x == 0) {
	// 	aiCat.animation_type = IDLE;
	// }
	// if (aiMotion.velocity.x != 0) {
	// 	aiCat.animation_type = WALKING;
	// }
	// if (aiMotion.velocity.y < 0) {
	// 	aiCat.animation_type = JUMPING;
	// }
	// if (aiMotion.velocity.x < 0) {
	// 	aiCat.facingLeft = 1;
	// }
	// if (aiMotion.velocity.x > 0) {
	// 	aiCat.facingLeft = 0;
	// }

}

void GameController::build_map() {
	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();

	// Move the walls off screen and don't render them
	// Floor
	createWall(renderer, { width / 2, height + 10 }, width, 10);
	
	// //Left Wall
	// createWall(renderer, { -10, height / 2 }, 10, height - 10);
	//
	// //Right Wall
	// createWall(renderer, { width + 10, height / 2 }, 10, height);
	//
	// //Ceiling
	// createWall(renderer, { width / 2, -10 }, width, 10);

	this->gameMap = Map();
	renderer->setTileMap(gameMap);
}

void GameController::init_player_teams() {
	std::vector<int> dummyvector;
	dummyvector.push_back(1);
	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();
	//If our game gets more complex I'd probably abstract this out an have an Entity hierarchy -Fred
	if (!dummyvector.empty()) {
		for (int dummyval : dummyvector) {
			//TEMPORARY UNTIL WE HAVE A MAP INIT SYSTEM
			Entity player_cat = createCat(this->renderer, { width / 2 - 200, height - 400 });
			// Entity ai_cat = createAI(this->renderer, { width / 2 - 300, height - 400 });
			player1_team.push_back(player_cat);
			// player1_team.push_back(ai_cat);
			curr_selected_char = player_cat;
		}
	}
	else {
		printf("CRITICAL ERROR: NO PLAYER1 TEAM INITIALIZED");
	}

	/*if (!player2_team.empty()) {
		for (Entity e : player2_team) {

		}
	}

	if (!ai_team.empty()) {
		for (Entity e : ai_team) {

		}
	}

	if (!npcai_team.empty()) {
		for (Entity e : ai_team) {

		}
	}*/

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
	registry.projectiles.clear();
	if (game_state.turn_possesion == TURN_CODE::END) {
		game_state.turn_possesion = TURN_CODE::PLAYER1;
	} else if (teams[game_state.turn_possesion].empty()) {
		game_state.turn_number -= 1;
		next_turn();
	}
}

void GameController::handle_collisions() {
	// Loop over all collisions detected by the physics system

	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		//If a projectile hits any terrain, get rid of the projectile
		if (registry.projectiles.has(entity)) {
			Projectile& pj = registry.projectiles.get(entity);
			if (registry.terrains.has(entity_other) && entity_other != pj.origin) {
				registry.remove_all_components_of(entity);
			}
		}

	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// On key callback
void GameController::on_player_key(int key, int, int action, int mod) {

	//Only allowed to move on specified turn
	if (game_state.turn_possesion == PLAYER1) {
		Motion& catMotion = registry.motions.get(player1_team[0]);

		/*
		 * DEBUGGING FOR MAP SYSTEM -----------------------------------------------------
		 */
		// auto& position = catMotion.position;
		// printf("Cat position is %f, %f\n", position.x, position.y);
		// vec2 closestTile = {position.x / gameMap.getTileScale(), position.y / gameMap.getTileScale()};
		// printf("Closest tile is %f, %f\n", closestTile.x, closestTile.y);

		//-------------------------------------------------------------------------------

		float current_speed = 150.0f;
		if (player_mode == PLAYER_MODE::MOVING) {
			if (action == GLFW_PRESS && key == GLFW_KEY_W) {
				catMotion.velocity.y = -current_speed;
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
		}
		else {
			if (action == GLFW_PRESS && key == GLFW_KEY_W) {
				shooting_system.aimUp(curr_selected_char);
			}

			if (action == GLFW_PRESS && key == GLFW_KEY_S) {
				shooting_system.aimDown(curr_selected_char);
			}

			if (action == GLFW_PRESS && key == GLFW_KEY_T) {
				shooting_system.shoot(curr_selected_char);
				printf("shooting");
				printf("Num of projectiles %u\n", (uint)registry.projectiles.components.size());
			}
		}

		//switch between shooting and firing for curr_selected_char
		if (action == GLFW_PRESS && key == GLFW_KEY_M) {
			if (player_mode == PLAYER_MODE::SHOOTING) {
				player_mode = PLAYER_MODE::MOVING;
			}
			else {
				player_mode = PLAYER_MODE::SHOOTING;
				shooting_system.setAimLoc(curr_selected_char);
			}
			printf("Current mode is: %s", (player_mode == PLAYER_MODE::SHOOTING) ? "SHOOTING" : "MOVING");
		}

	}

	if (action == GLFW_PRESS && key == GLFW_KEY_N) {
		next_turn();
		printf("Currently it is this players turn: %i", game_state.turn_possesion);
	}
}