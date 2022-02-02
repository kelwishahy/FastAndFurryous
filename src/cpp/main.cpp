// Game Systems
#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>

// stlib
#include <chrono>
#include <iostream>

//internal
#include "hpp/world_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Triangle NDC
float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

int main() {
	std::cout << "Starting The Fast and the Furryous" << std::endl;
	// TO-DO:
	// Global systems
	WorldSystem world;

	// Initialize game systems
	RenderSystem renderer = RenderSystem::RenderSystem();
	GLFWwindow* window = renderer.getWindow(); // Window is part of the renderer context

	// Copy triangle vertex data into the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	auto time = Clock::now();

	// Game loop
	while (!glfwWindowShouldClose(window)) { // TO-DO: Make this loop condition depend on the world state, like in assignment template
		// Process system events
		glfwPollEvents();

		// TO-DO: Create input system and manage external input here
		// Note: GLFW is used to track keyboard & mouse input

		// Determine time elapsed
		auto now = Clock::now();
		float elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - time)).count() / 1000;
		time = now;

		renderer.draw(SHADER_PROGRAM_IDS::TRIANGLE);

		glfwSwapBuffers(window);
	}



	return EXIT_SUCCESS;
}
