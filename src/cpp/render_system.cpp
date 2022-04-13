#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>
#include <hpp/texture_manager.hpp>

#include <hpp/common.hpp>
#include <hpp/components.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "glm/ext.hpp"
#include <iostream>

using namespace glm;
vec2 screenResolution;

void RenderSystem::draw(float elapsed_ms, WorldSystem& world) {
	this->camera = &world.getCamera();
	setTileMap(world.getCurrentGame().getGameMap());
	animation_system.step(elapsed_ms);
	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Draw the map
	if (world.getCurrentGame().inAGame) {
		drawTiles();
		drawParticles(world.getParticleSystem().getNumActive());
	}

	// Sort all the RenderRequests by depth
	registry.renderRequests.sort([](Entity& lhs, Entity& rhs) {
			return lhs.getDepth() < rhs.getDepth();
	});

	for (Entity entity : registry.renderRequests.entities) {

		if (!registry.renderRequests.has(entity))
			continue;

		RenderRequest request = registry.renderRequests.get(entity);

		if (registry.backgrounds.has(entity)) {
			float pos = screenWidth;
			float depth = registry.backgrounds.get(entity).layer;
			if (depth == LAYERS[1]) {
				pos -= camera->getPosition().x / 5.f;
			} else if (depth == LAYERS[2]) {
				pos -= camera->getPosition().x / 25.f;
			}
			drawBackground(request, depth, scaleToScreenResolution({ pos, screenHeight / 2 }), scaleToScreenResolution({ 2 * screenWidth, screenHeight }));
			continue;
		}

		if (registry.menus.has(entity)) {
			MenuItem& menu = registry.menus.get(entity);
			drawBackground(request, menu.layer, {screenWidth / 2, screenHeight / 2}, {screenWidth, screenHeight});
			continue;
		}

		if (registry.texts.has(entity)) {
			drawText(world.getTextManager(), entity);
			continue;
		}

		switch (request.geometry) {
			case GEOMETRY_BUFFER_IDS::QUAD: {
				auto& motion = registry.motions.get(entity);
				std::string shaderInputs[] = { "position" };
				drawQuad(request, shaderInputs, 1);
				renderToScreen(transform(motion.position, motion.scale, 0.f, 0));
				break;
			}

			case GEOMETRY_BUFFER_IDS::TEXTURED_QUAD: {

				// Animate player sprites
				if (registry.animations.has(entity)) {
					animateSprite(request, entity);
					continue;
				}

				// Draw a static textured quad
				auto& motion = registry.motions.get(entity);
				std::string shaderInputs[] = { "position", "texCoord" };
				drawQuad(request, shaderInputs, 2);

				float depth = registry.buttons.has(entity) ? 0.8f : 0.0f;
				renderToScreen(transform(motion.position, motion.scale, depth, motion.angle));
				break;
			}

			default:
				continue;
		}
	}
}

void RenderSystem::renderToScreen(mat4 transformationMatrix) {
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glHasError();
	const GLsizei numIndices = size / sizeof(uint16_t);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transformationMatrix);

	auto proj = camera->getViewProjectionMatrix(); // Passing in the camera's viewProjectionMatrix
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&proj);
	glHasError();

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
	glHasError();
}

void RenderSystem::drawQuad(RenderRequest& request, std::string shaderInputs[], int numInputs) {
	const GLuint vbo = vertexBuffers[(GLuint)request.geometry];
	const GLuint ibo = indexBuffers[(GLuint)request.geometry];

	// Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glHasError();

	// set shaders
	const GLuint shaderProgram = shaders[(GLuint)request.shader];
	glUseProgram(shaderProgram);
	glHasError();

	if (request.geometry == GEOMETRY_BUFFER_IDS::TEXTURED_QUAD) {
		// Set texture
		glActiveTexture(GL_TEXTURE0);
		const GLuint texture = textures[(GLuint)request.texture];
		glBindTexture(GL_TEXTURE_2D, texture);

		// Pass in shader inputs
		for (int i = 0; i < numInputs; i++) {
			const GLint inputPosition = glGetAttribLocation(shaderProgram, shaderInputs[i].c_str());
			glHasError();

			// input variable names need to be standardized to match the names below
			if (shaderInputs[i] == "position") {
				glVertexAttribPointer(inputPosition, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
			}
			else if (shaderInputs[i] == "texCoord") {
				glVertexAttribPointer(inputPosition, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
			}

			glEnableVertexAttribArray(inputPosition);
			glHasError();
		}

	} else {
		const GLint inputPosition = glGetAttribLocation(shaderProgram, "position");
		glVertexAttribPointer(inputPosition, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	}


}

void RenderSystem::animateSprite(RenderRequest& request, Entity& entity) {
	Animation& animation = registry.animations.get(entity);

	// Is the current player selected to move?
	bool selected;
	if (registry.selected.has(entity)) {
		selected = registry.selected.get(entity).isSelected;
	} else {
		selected = true;
	}

	TEXTURE_ANIM_CONSTANTS constants = animation.animation_states_constants.at(animation.anim_state);

	// Updating the texture coordinates for use with the animation sprite sheets
	texturedQuad[0].texCoord = { constants.FRAME_TEXTURE_WIDTH, 1.0f }; // top right
	texturedQuad[1].texCoord = { constants.FRAME_TEXTURE_WIDTH, 0.0f }; // bottom right
	texturedQuad[2].texCoord = { 0.0f, 0.0f }; // bottom left
	texturedQuad[3].texCoord = { 0.0f, 1.0f }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);
	
	std::string shaderInputs[] = { "position", "texCoord" };
	drawQuad(request, shaderInputs, 2);

	// Pass in uniform variables
	const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::ANIMATION];
	glUseProgram(shaderProgram);
	glHasError();

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	GLint frame_uloc = glGetUniformLocation(shaderProgram, "currentFrame");
	GLfloat frame_width_uloc = glGetUniformLocation(shaderProgram, "frameWidth");
	GLint facing_left_uloc = glGetUniformLocation(shaderProgram, "facingLeft");
	GLboolean selectedLoc = glGetUniformLocation(shaderProgram, "selected");
	GLboolean hurtLoc = glGetUniformLocation(shaderProgram, "hurt");
	glUniform1i(frame_uloc, animation.curr_frame);
	glUniform1f(frame_width_uloc, constants.FRAME_TEXTURE_WIDTH);
	glUniform1i(facing_left_uloc, animation.facingLeft);
	glUniform1i(selectedLoc, selected);

	// Render to the screen
	auto& motion = registry.motions.get(entity);
	renderToScreen(transform(motion.position, motion.scale, 0.5f, motion.angle));

	// Resetting the texture coordinates after use
	texturedQuad[0].texCoord = { 1.f, 1.f }; // top right
	texturedQuad[1].texCoord = { 1.f, 0.f }; // bottom right
	texturedQuad[2].texCoord = { 0.f, 0.f }; // bottom left
	texturedQuad[3].texCoord = { 0.f, 1.f }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

}

void RenderSystem::drawTiles() {
	const unsigned int& numTiles = gameMap.getNumTiles();

	// 1. Set texture coordinates
	texturedQuad[0].texCoord = { 0.333, 0.333 }; // top right
	texturedQuad[1].texCoord = { 0.333, 0.667 }; // bottom right
	texturedQuad[2].texCoord = { 0.0, 0.667 }; // bottom left
	texturedQuad[3].texCoord = { 0.0, 0.333 }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

	// 2. Bind buffers and pass in position and texcoord vertex attributes
	const GLuint vbo = vertexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::TEXTURED_QUAD];
	const GLuint ibo = indexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::TEXTURED_QUAD];

	// Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glHasError();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glHasError();

	// 3. Set the shader
	const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::TILE];
	glUseProgram(shaderProgram);
	glHasError();

	// 4. Pass in projection matrix
	auto proj = camera->getViewProjectionMatrix(); // Passing in the camera's viewProjectionMatrix
	GLuint projection_loc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&proj);
	glHasError();

	// 5. Bind the texture
	glActiveTexture(GL_TEXTURE0);
	const GLuint texture = textures[(GLuint)TEXTURE_IDS::STONE];
	glBindTexture(GL_TEXTURE_2D, texture);

	// 6. Draw
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glHasError();
	const GLsizei numIndices = size / sizeof(uint16_t);

	// Draw all the tiles at once
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr, numTiles);

	// 7. Reset the texture coordinates after use
	texturedQuad[0].texCoord = { 1.f, 1.f }; // top right
	texturedQuad[1].texCoord = { 1.f, 0.f }; // bottom right
	texturedQuad[2].texCoord = { 0.f, 0.f }; // bottom left
	texturedQuad[3].texCoord = { 0.f, 1.f }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);
}

void RenderSystem::drawBackground(RenderRequest& request, float layer, vec2 position, vec2 scale) {
	mat4 transformationMatrix = transform(position, scale, layer, 0);
	std::string shaderInputs[] = { "position", "texCoord" };

	switch(request.texture) {
	case TEXTURE_IDS::INDUSTRIAL_BG:
	case TEXTURE_IDS::INDUSTRIAL_FAR_BUILDINGS:
	case TEXTURE_IDS::INDUSTRIAL_BUILDINGS:
	case TEXTURE_IDS::INDUSTRIAL_FOREGROUND:
		texturedQuad[0].texCoord = { 2.f, 1.f }; // top right
		texturedQuad[1].texCoord = { 2.f, 0.f }; // bottom right
		bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);
		break;
	case TEXTURE_IDS::NIGHT1:
	case TEXTURE_IDS::NIGHT2:
	case TEXTURE_IDS::CYBERPUNK1:
	case TEXTURE_IDS::CYBERPUNK2:
	case TEXTURE_IDS::CYBERPUNK3:
	case TEXTURE_IDS::MIAMI1:
	case TEXTURE_IDS::MIAMI2:
	case TEXTURE_IDS::MIAMI3:
	case TEXTURE_IDS::MIAMI4:
		texturedQuad[0].texCoord = { 3.f, 1.f }; // top right
		texturedQuad[1].texCoord = { 3.f, 0.f }; // bottom right
		bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);
	
	default:
		break;
	}

	drawQuad(request, shaderInputs, 2);
	renderToScreen(transformationMatrix);
}

void RenderSystem::drawText(TextManager& textManager, Entity e) {
	bool isItalic = false;
	bool isBold = false;

	const GLuint fontvbo = vertexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::FONT];

	Motion& motion = registry.motions.get(e);
	Text& fonttext = registry.texts.get(e);

	glBindBuffer(GL_ARRAY_BUFFER, fontvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// set shaders
	const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::FONT];
	glUseProgram(shaderProgram);
	glHasError();

	glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), fonttext.color.x, fonttext.color.y, fonttext.color.z);
	glActiveTexture(GL_TEXTURE0);

	//this is capital 'A' for reference
	float captialsize = textManager.getCapitalSize();

	// iterate through all characters
	std::string::const_iterator c;
	float acc = motion.position.x;
	for (c = fonttext.text.begin(); c != fonttext.text.end(); c++) {
		//'$' character
		if (*c == 36) {
			isBold = isBold ? false : true;
			continue;
		}
		if (*c == 126) {
			isItalic = isItalic? false : true;
			continue;
		}

		Glyph ch;
		if (isItalic) {
			ch = textManager.getItalicGlyphs()[*c];
		} else if (isBold) {
			ch = textManager.getBoldGlyphs()[*c];
		} else {
			ch = textManager.getGlyphs()[*c];
		}

		float xpos = acc + ch.bearing.x * motion.scale.x;
		float ypos = motion.position.y + (ch.size.y - ch.bearing.y) * motion.scale.y;

		//We need to offset lower case letters because of our coordinate system (since 0,0 is top left)
		if (!isupper((int)*c)) {
			ypos += captialsize - ch.size.y;
		}

		float w = ch.size.x * motion.scale.x;
		float h = ch.size.y * motion.scale.y;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos,     ypos,       0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, fontvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		// the isItalic prevents glitching since italics are slanted but the quads are not
		if (isItalic) {
			acc += (ch.advance >> 6) * motion.scale.x + 3.0f;
		}
		else {
			acc += (ch.advance >> 6) * motion.scale.x; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
	}
	fonttext.scale.x = acc - motion.position.x; // set x scale
	fonttext.scale.y = captialsize * motion.scale.y; // set y scale

	//Custom projection matrix
	auto m = registry.motions.get(e);
	m.position = {m.position.x / defaultResolution.x * screenWidth, m.position.y / defaultResolution.y * screenHeight};
	m.scale = { m.scale.x / defaultResolution.x * screenWidth, m.scale.y / defaultResolution.y * screenHeight };
	renderToScreen(transform(scaleToScreenResolution(m.position), scaleToScreenResolution(m.scale), 1.0f, 0));
}

void RenderSystem::drawParticles(int numParticles) {
	vec3 colour = { 1.f, 0.f, 0.f };

	// Bind buffers and pass in position
	const GLuint vbo = vertexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::CIRCLE];
	const GLuint ibo = indexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::CIRCLE];

	// Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glHasError();

	// set shaders
	const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::PARTICLE];
	glUseProgram(shaderProgram);
	glHasError();

	// Pass in shader inputs (position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glHasError();

	// Pass in uniforms
	auto proj = camera->getViewProjectionMatrix(); // Passing in the camera's viewProjectionMatrix
	GLuint projection_loc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&proj);
	glHasError();

	GLuint colour_loc = glGetUniformLocation(shaderProgram, "colour");
	glUniform3fv(colour_loc, 1, (float*)&colour);
	glHasError();

	// Draw
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glHasError();
	const GLsizei numIndices = size / sizeof(uint16_t);

	// Draw all the particles at once
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr, numParticles);
}


/*
 * INITIALIZATION FUNCTIONS
 */
bool RenderSystem::init() {
	// Create window & context
	glfwInit();
	// Using OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // Request GLFW debug context
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Window is not resizable
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE); // Make the window full screen

	// NOTE: The width & height here are unimportant as the window will be maximized on creation
	this->window = glfwCreateWindow(1600, 900, "Fast and the Furry-ous", NULL, NULL);
	glfwSetWindowAspectRatio(window, 16, 9);
	glfwGetFramebufferSize(window, &this->screenWidth, &this->screenHeight);
	printf("Screen size: %d, %d\n", this->screenWidth, this->screenHeight);
	screenResolution = {screenWidth, screenHeight};

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}


	//----------------------------------------------------------------------------------
	/* FOR DEBUGGING PURPOSES ONLY */

	//Check if the debug context was succesfully initialized
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		// Initialize debug output
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	//----------------------------------------------------------------------------------

	// Create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glHasError();

	// Load shaders
	ShaderManager::loadShaders(shaderPaths, shaders);

	// Load textures
	TextureManager::loadTextures(texturePaths, textures, textureDimensions);

	//Load vertex data
	initRenderData();

	// initFonts();

	return true;
}

void RenderSystem::initRenderData() {

	// Create vertex buffers
	glGenBuffers((GLsizei)vertexBuffers.size(), vertexBuffers.data());

	// Create index buffers
	glGenBuffers((GLsizei)indexBuffers.size(), indexBuffers.data());

	// Every object in this game is essentially a textured quad (square),
	// so we can just define one quad geometry that all objects can use

	/*
	 * Initialize the reuasable untextured quad geometry
	 */
	quad.push_back({ 0.5f,  0.5f, 0.0f });
	quad.push_back({ 0.5f, -0.5f, 0.0f });
	quad.push_back({ -0.5f, -0.5f, 0.0f });
	quad.push_back({ -0.5f,  0.5f, 0.0f });

	/*
	 * Initialize the reusable textured quad geometry
	 * The texture coordinates may need to be updated if using a sprite sheet
	 */
	texturedQuad.push_back({ { 0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } }); // top right
	texturedQuad.push_back({ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } }); // bottom right
	texturedQuad.push_back({ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }); // bottom left
	texturedQuad.push_back({ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } }); // top left

	bindVBOandIBO(GEOMETRY_BUFFER_IDS::QUAD, quad, quadIndices);
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

	/*
	 * Initialize the circle mesh
	 */
	std::vector<ColoredVertex> circleVertices;
	std::vector<uint16_t> circleIndices;
	constexpr float z = -0.1f;
	constexpr int NUM_TRIANGLES = 62;

	for (int i = 0; i < NUM_TRIANGLES; i++) {
		const float t = float(i) * M_PI * 2.f / float(NUM_TRIANGLES - 1);
		circleVertices.push_back({});
		circleVertices.back().position = { 0.5 * cos(t), 0.5 * sin(t), z };
		circleVertices.back().color = { 0.8, 0.8, 0.8 };
	}

	circleVertices.push_back({});
	circleVertices.back().position = { 0, 0, 0 };
	circleVertices.back().color = { 1, 1, 1 };

	for (int i = 0; i < NUM_TRIANGLES; i++) {
		circleIndices.push_back((uint16_t)i);
		circleIndices.push_back((uint16_t)((i + 1) % NUM_TRIANGLES));
		circleIndices.push_back((uint16_t)NUM_TRIANGLES);
	}

	int geom_index = (int)GEOMETRY_BUFFER_IDS::CIRCLE;
	meshes[geom_index].vertices = circleVertices;
	meshes[geom_index].vertexIndices = circleIndices;
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::CIRCLE, meshes[geom_index].vertices, meshes[geom_index].vertexIndices);
}
