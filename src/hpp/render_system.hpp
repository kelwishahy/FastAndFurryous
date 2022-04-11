#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#include "common.hpp"
#include "world_system.hpp"
#include "hpp/orthographic_camera.hpp"

class RenderSystem {

	// Shaders
	const std::array<std::string, shaderProgramCount> shaderPaths = {
		"animation",
		"texture",
		"wall",
		"ai",
		"font",
		"tile"
	};

	std::array<GLuint, shaderProgramCount> shaders; // OpenGL shader names

	// Textures
	const std::array<std::string, textureCount> texturePaths = {
		//Cat sprites
		"cat_side_idle_AK.png",
		"cat_side_idle_SG.png",
		"cat_side_idle_GL.png",
		"cat_side_idle_AWP.png",
		"cat_front_idle.png",
		"cat_walk.png",
		"cat_jump.png",
		"cat_side_blink.png",
		"cat_front_blink.png",
		"cat_front_arm.png",
		"cat_back_arm.png",
		"cat_hurt_face.png",
		"cat_hurt.png",
		"cat_dead.png",
		// Dog Sprites
		"dog_side_idle_AK.png",
		"dog_side_idle_SG.png",
		"dog_side_idle_GL.png",
		"dog_side_idle_AWP.png",
		"dog_front_blink.png",
		"dog_side_blink.png",
		"dog_front_idle.png",
		"dog_front_arm.png",
		"dog_back_arm.png",
		"dog_walk.png",
		"dog_jump.png",
		"dog_hurt.png",
		"dog_hurt_face.png",
		"dog_dead.png",
		// Gun Sprites
		"rifle.png",
		"ak47.png",
		"awp.png",
		"grenade_launcher.png",
		"stone.png",
		"background.png",
		"start_bg.jpg",
		"select_bg.jpg",
		"options.jpg",
		"levels.jpg",
		"button1.jpg",
		"button2.jpg",
		"button3.jpg",
		"button4.jpg",
		"select_cat.jpg",
		"select_dog.jpg",
		"tutorial_bg.jpg",
		"right_arrowA.png",
		"left_arrowA.png",
		"right_arrowB.png",
		"left_arrowB.png",
		"level1.png",
		"level2.png",
		"level3.png",
		"cat_crosshair.png",
		"dog_crosshair.png",
		"health_square.png",
		"playgame.png",
		"cancel.png",

		// Industrial parallax background
		"industrial/bg.png",
		"industrial/far-buildings.png",
		"industrial/buildings.png",
		"industrial/skill-foreground.png",

		// Night parallax background
		"night/night1.png",
		"night/night2.png",

		// Cyberpunk parallax background
		"cyberpunk/far-buildings.png",
		"cyberpunk/back-buildings.png",
		"cyberpunk/foreground.png",

		// Miami Parallax background
		"miami/back.png",
		"miami/sun.png",
		"miami/buildings.png",
		"miami/palms.png",
		

		"forest/forest.png",
		"space/space.png"
	};

	std::array<GLuint, textureCount> textures; // OpenGL texture names
	std::array<glm::ivec2, textureCount> textureDimensions;

	// Vertex and index buffers
	std::array<GLuint, geometryCount> vertexBuffers;
	std::array<GLuint, geometryCount> indexBuffers;

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
	void draw(float elapsed_ms, WorldSystem& world);

	// Draw a quad with an optional texture
	void drawQuad(RenderRequest& request, std::string shaderInputs[], int numInputs);

	void animateSprite(RenderRequest& request, Entity& entity);

	// Draw a tilemap
	void drawTiles();

	// Draw the background texture image
	void drawBackground(RenderRequest& request, float layer, glm::vec2 position, glm::vec2 scale);

	// Draw any text entities
	void drawText(TextManager& textManager, Entity e);

	// Initialize GLFW window and context
	bool init();

	// Return the GLFW window associated with this renderer
	GLFWwindow* getWindow() { return window; }

	int getScreenWidth() { return this->screenWidth; }
	int getScreenHeight() { return this->screenHeight; }

	void setTileMap(const MapSystem::Map& gameMap) { this->gameMap = gameMap; }

private:
	AnimationSystem animation_system;
	GLFWwindow* window;
	OrthographicCamera* camera;
	int screenWidth;
	int screenHeight;
	GLuint vao;
	GLuint frameBuffer;
	GLuint renderBufferColour;
	GLuint renderBufferDepth;
	MapSystem::Map gameMap;
	std::map<char, Glyph> glyphs;
	std::map<char, Glyph> italic_glyphs;
	std::map<char, Glyph> bold_glyphs;

	// Reusable quad (square) geometry
	std::vector<TexturedVertex> texturedQuad;
	std::vector<glm::vec3> quad;
	const std::vector<uint16_t> quadIndices = { 2, 0, 3, 2, 1, 0 };

	// Load vertex data into the vertex buffers
	void initRenderData();

	// The last step of the draw function
	void renderToScreen(glm::mat4 transformationMatrix);

	// Bind the given vertex and index buffer objects
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices) {

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[(uint)oid]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glHasError();

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[(uint)oid]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glHasError();
	}
};
