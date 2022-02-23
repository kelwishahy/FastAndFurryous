#include <hpp/game_controller.hpp>
#include <hpp/world_init.hpp>

GameController::GameController() {
	inAGame = false;
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
}

void GameController::step(float elapsed_ms)
{
	//While a game is happening make sure the players are controlling from here
	glfwSetWindowUserPointer(window, this);

}

void GameController::build_map() {
	//TEMPORARY UNTIL WE FIGURE OUT A SYSTEM FOR MAP BUILDING
	////Floor
	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();
	createWall(renderer, { width / 2, height }, width, 50);

	////Left Wall
	createWall(renderer, { 0, height / 2 }, 50, height - 10);

	////Right Wall
	createWall(renderer, { width, height / 2 }, 50, height);

	////Ceiling
	createWall(renderer, { width / 2, 0 }, width, 50);

}

void GameController::init_player_teams() {
	//TEMPORARY UNTIL WE FIGURE OUT A SYSTEM FOR MAP BUILDING
	std::vector<int> dummyvector;
	dummyvector.push_back(1);
	const int width = renderer->getScreenWidth();
	const int height = renderer->getScreenHeight();
	//If our game gets more complex I'd probably abstract this out an have an Entity hierarchy -Fred
	if (!dummyvector.empty()) {
		for (int dummyval : dummyvector) {
			//TEMPORARY UNTIL WE HAVE A MAP INIT SYSTEM
			Entity player_cat = createCat(this->renderer, { width / 2 - 200, height - 400 });
			player1_team.push_back(player_cat);
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
	if (game_state.turn_possesion == TURN_CODE::END) {
		game_state.turn_possesion = TURN_CODE::PLAYER1;
	} else if (teams[game_state.turn_possesion].empty()) {
		game_state.turn_number -= 1;
		next_turn();
	}
}

// On key callback
void GameController::on_player_key(int key, int, int action, int mod) {

	//Only allowed to move on specified turn
	if (game_state.turn_possesion == PLAYER1) {
		Motion& catMotion = registry.motions.get(player1_team[0]);

		float current_speed = 150.0f;
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
			if (key == GLFW_KEY_LEFT && catMotion.velocity.x < 0) {
				catMotion.velocity.x = 0.0f;
			}
			if (key == GLFW_KEY_RIGHT && catMotion.velocity.x > 0) {
				catMotion.velocity.x = 0.0f;
			}
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_T) {

		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_N) {
		next_turn();
		printf("Currently it is this players turn: %i", game_state.turn_possesion);
	}
}