#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <../project_path.hpp>

#include <array>
#include <vector>
#include <string>
#include <map>

#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3

#include "components.hpp"
#include "map.hpp"
#include "animation_system.hpp"

class RenderSystem {

	// Shaders
	const std::array<std::string, shaderProgramCount> shaderPaths = {
		"animation",
		"texture",
		"wall",
		"ai",
		"font"
	};

	std::array<GLuint, shaderProgramCount> shaders; // OpenGL shader names

	// Textures
	const std::array<std::string, textureCount> texturePaths = {
		//Cat sprites
		"cat_side_idle.png",
		"cat_front_idle.png",
		"cat_walk.png",
		"cat_jump.png",
		"cat_side_blink.png",
		"cat_front_blink.png",
		"cat_hurt_face.png",
		"cat_hurt.png",
		"cat_dead.png",
		//
		"stone.png",
		"background.png",
		"start_bg.jpg",
		//selectscreen
		"select_bg.jpg",
		"button1.jpg",
		"select_cat.jpg",
		"tutorial_bg.jpg"
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
	const GLfloat CAT_IDLE_FRAME_WIDTH = 0.111f;
	float CAT_IDLE_FRAME_TIME = 100; // lowering the value makes the animation faster

	// CAT WALK
	const int CAT_WALK_FRAMES = 9;
	const GLfloat CAT_WALK_FRAME_WIDTH = 0.111f;
	float CAT_WALK_FRAME_TIME = 100; 

	// CAT JUMP
	const int CAT_JUMP_FRAMES = 9;
	const GLfloat CAT_JUMP_FRAME_WIDTH = 0.111f;
	float CAT_JUMP_FRAME_TIME = 100;


public:
	RenderSystem () {};
	~RenderSystem () {};

	// Draw to the screen using shaderProgram
	void draw(float elapsed_ms);

	// Draw a quad with an optional texture
	void drawQuad(RenderRequest& request, std::string shaderInputs[], int numInputs);

	void animateSprite(RenderRequest& request, Entity& entity, float elapsed_ms);

	// Draw a tilemap
	void drawTiles(const glm::mat4& projectionMatrix);

	// Draw the background texture image
	void drawBackground(RenderRequest& request, glm::mat4& projectionMatrix, float layer);

	// Draw any text entities
	void drawText(Entity e);

	// Initialize GLFW window and context
	bool init();

	// Return the GLFW window associated with this renderer
	GLFWwindow* getWindow() { return window; }

	Mesh& getMesh(GEOMETRY_BUFFER_IDS id) { return meshes[(int)id]; }

	int getScreenWidth() { return this->screenWidth; }
	int getScreenHeight() { return this->screenHeight; }

	void setTileMap(const Map& gameMap) { this->gameMap = gameMap; }

private:
	GLFWwindow* window;
	int screenWidth;
	int screenHeight;
	GLuint vao;
	GLuint frameBuffer;
	GLuint renderBufferColour;
	GLuint renderBufferDepth;
	Map gameMap;
	std::map<char, Glyph> glyphs;
	std::map<char, Glyph> italic_glyphs;
	std::map<char, Glyph> bold_glyphs;

	// Reusable quad (square) geometry
	std::vector<TexturedVertex> texturedQuad;
	std::vector<glm::vec3> quad;
	const std::vector<uint16_t> quadIndices = { 2, 0, 3, 2, 1, 0 };

	// Initialize the off screen render buffer
	// which is used as an intermediate render target
	bool initRenderBuffer();

	// Load vertex data into the vertex buffers
	void initRenderData();

	//Load font stuff
	void initFonts();

	// Bind the given vertex and index buffer objects
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void loadMeshes();

	// Apply matrix transformations
	// position is generally motion.position
	// scale is generally motion.scale
	glm::mat4 transform(glm::vec2 position, glm::vec2 scale, float depth);

	// The last step of the draw function
	void renderToScreen(glm::mat4& transformationMatrix, glm::mat4& projectionMatrix);

	// Generate a 4x4 orthographic projection matrix
	// that scales with window size
	glm::mat4 createProjectionMatrix();

	AnimationSystem animation_system;

};

