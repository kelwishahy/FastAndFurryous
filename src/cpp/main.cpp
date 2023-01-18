// Game Systems
#include <hpp/render_system.hpp>
#include <hpp/physics_system.hpp>
#include <hpp/world_system.hpp>

// stlib
#include <chrono>
#include <iostream>

//internal

using Clock = std::chrono::high_resolution_clock;

bool WorldSystem::pause_flag = false;

int main() {

	std::cout << "Starting Fast and Furry-ous" << std::endl;

	// Global systems
	RenderSystem renderer;

	// Initialize game systems
	renderer.init();
	GLFWwindow* window = renderer.getWindow(); // Window is part of the renderer context

START:
	WorldSystem world;
	PhysicsSystem physics;
	physics.init(&renderer);
	world.init(window);

	auto time = Clock::now();

	// Game loop
	glfwSetWindowUserPointer(window, &world);

	while (!glfwWindowShouldClose(window)) { // TO-DO: Make this loop condition depend on the world state, like in assignment template

		if (WorldSystem::pause_flag){
			glfwPollEvents();
			time = Clock::now();
		}
		else {
		// Process system events
		glfwPollEvents();

		// Determine time elapsed
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - time)).count() / 1000;
		time = now;

		world.step(elapsed_ms);
		physics.step(elapsed_ms);

		renderer.draw(elapsed_ms, world);
		glfwSwapBuffers(window);}

		if (world.quit) {
			registry.clear_all_components();
			goto START;
		}
	}

	return EXIT_SUCCESS;
}
