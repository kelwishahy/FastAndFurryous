#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <vector>
#include <string>

#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3

#include "components.hpp"

class RenderSystem {

	// Shaders
	const std::array<std::string, shaderProgramCount> shaderPaths = {
		"cat",
		"triangle"
	};

	std::array<GLuint, shaderProgramCount> shaders;

	// Textures
	const std::array<std::string, textureCount> texturePaths = {
		"cat.png",
	};

	std::array<GLuint, textureCount> textures;
	std::array<glm::ivec2, textureCount> textureDimensions;

	// Vertex and index buffers
	std::array<GLuint, geometryCount> vertexBuffers;
	std::array<GLuint, geometryCount> indexBuffers;


public:
	RenderSystem ();
	~RenderSystem ();

	// Return the GLFW window associated with this renderer
	GLFWwindow* getWindow() { return window; };

	// Draw to the screen using shaderProgram
	void draw(const SHADER_PROGRAM_IDS shaderProgram);

private:
	GLFWwindow* window;
	GLuint frameBufferId;
	GLuint vaoId;

	// Initialize GLFW window and context
	bool init();

	// Load vertex data into the vertex buffers
	void initRenderData();

	// Bind the given vertex and index buffer objects
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices);
};

