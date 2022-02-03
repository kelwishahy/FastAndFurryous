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

	// Meshes
	const std::vector < std::pair<GEOMETRY_BUFFER_IDS, std::string>> meshPaths = {
		// specify meshes of all assets here
	};

	std::array<Mesh, geometryCount> meshes;


public:
	RenderSystem ();
	~RenderSystem ();

	// Initialize GLFW window and context
	bool init();

	// Return the GLFW window associated with this renderer
	GLFWwindow* getWindow() { return window; };

	// Draw to the screen using shaderProgram
	void draw();

	Mesh& getMesh(GEOMETRY_BUFFER_IDS id) { return meshes[(int)id]; };

private:
	GLFWwindow* window;
	GLuint frameBuffer;
	GLuint vaoId;

	static glm::mat3 createProjectionMatrix();

	// Load vertex data into the vertex buffers
	void initRenderData();

	// Bind the given vertex and index buffer objects
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void loadMeshes();
};

