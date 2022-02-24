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
		"texture",
		"wall",
		"ai"
	};

	std::array<GLuint, shaderProgramCount> shaders; // OpenGL shader names

	// Textures
	const std::array<std::string, textureCount> texturePaths = {
		"cat-idle.png",
		"cat-walk.png",
		"cat-jump.png",
		"stone.png"
	};

	std::array<GLuint, textureCount> textures; // OpenGL texture names
	std::array<glm::ivec2, textureCount> textureDimensions;

	// Vertex and index buffers
	std::array<GLuint, geometryCount> vertexBuffers;
	std::array<GLuint, geometryCount> indexBuffers;

	// Meshes
	const std::vector < std::pair<GEOMETRY_BUFFER_IDS, std::string>> meshPaths = {
		// specify meshes of all assets here
	};

	std::array<Mesh, geometryCount> meshes;

	// CAT IDLE
	const int CAT_IDLE_FRAMES = 9;
	const GLfloat CAT_IDLE_FRAME_WIDTH = 0.111;
	float CAT_IDLE_FRAME_TIME = 100; // lowering the value makes the animation faster

	// CAT WALK
	const int CAT_WALK_FRAMES = 9;
	const GLfloat CAT_WALK_FRAME_WIDTH = 0.111;
	float CAT_WALK_FRAME_TIME = 100; 

	// CAT JUMP
	const int CAT_JUMP_FRAMES = 8;
	const GLfloat CAT_JUMP_FRAME_WIDTH = 0.125;
	float CAT_JUMP_FRAME_TIME = 100;


public:
	RenderSystem ();
	~RenderSystem ();

	// Draw to the screen using shaderProgram
	void draw(float elapsed_ms);

	// Draw a quad with an optional texture
	void drawQuad(RenderRequest& request, std::string shaderInputs[], int numInputs);

	void animateSprite(Entity& entity, float elapsed_ms);

	// Draw a tilemap
	void drawTiles();

	// Initialize GLFW window and context
	bool init();

	// Return the GLFW window associated with this renderer
	GLFWwindow* getWindow() { return window; }

	Mesh& getMesh(GEOMETRY_BUFFER_IDS id) { return meshes[(int)id]; }

	int getScreenWidth() { return this->screenWidth; }
	int getScreenHeight() { return this->screenHeight; }

private:
	GLFWwindow* window;
	int screenWidth;
	int screenHeight;
	GLuint vao;
	GLuint frameBuffer;
	GLuint renderBufferColour;
	GLuint renderBufferDepth;

	// Reusable quad (square) geometry
	std::vector<TexturedVertex> texturedQuad;
	std::vector<glm::vec3> quad;
	const std::vector<uint16_t> quadIndices = { 2, 0, 3, 2, 1, 0 };

	// Initialize the off screen render buffer
	// which is used as an intermediate render target
	bool initRenderBuffer();

	// Load vertex data into the vertex buffers
	void initRenderData();

	// Bind the given vertex and index buffer objects
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void loadMeshes();

	// Generate a 4x4 orthographic projection matrix
	// that scales with window size
	glm::mat4 createProjectionMatrix();

};

