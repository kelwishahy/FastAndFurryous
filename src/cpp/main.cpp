// Game Systems
#include <hpp/render_system.hpp>
#include <hpp/physics_system.hpp>
#include <hpp/world_system.hpp>
#include <hpp/ai_system.hpp>

// stlib
#include <chrono>
#include <iostream>

//internal

using Clock = std::chrono::high_resolution_clock;

int main() {
	std::cout << "Starting The Fast and the Furryous" << std::endl;

	// Global systems
	WorldSystem world;
	PhysicsSystem physics;
	RenderSystem renderer;
	AISystem ai;

	// Initialize game systems
	renderer.init();
	ai.init();
	physics.init(&renderer);
	world.init(&renderer);
	GLFWwindow* window = renderer.getWindow(); // Window is part of the renderer context

	auto time = Clock::now();

	// Game loop
	while (!glfwWindowShouldClose(window)) { // TO-DO: Make this loop condition depend on the world state, like in assignment template
		// Process system events
		glfwPollEvents();

		glfwSetWindowUserPointer(window, &world);
		// TO-DO: Create input system and manage external input here
		auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
		auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
		glfwSetKeyCallback(window, key_redirect);
		glfwSetCursorPosCallback(window, cursor_pos_redirect);
		// Note: GLFW is used to track keyboard & mouse input

		// Determine time elapsed
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - time)).count() / 1000;
		time = now;

		world.step(elapsed_ms);
		ai.step(elapsed_ms);
		physics.step(elapsed_ms);
		world.handle_collisions();

		renderer.draw();
		glfwSwapBuffers(window);
	}

	return EXIT_SUCCESS;
}
