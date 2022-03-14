#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>
#include <hpp/texture_manager.hpp>

#include <hpp/common.hpp>
#include <hpp/components.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "glm/ext.hpp"
#include <iostream>
#include "../project_path.hpp"
#include "hpp/ai_system.hpp"

using namespace glm;

void RenderSystem::draw(float elapsed_ms) {

	animation_system.step(elapsed_ms);

	mat4 projectionMatrix = createProjectionMatrix();

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Draw the map
	drawTiles(projectionMatrix);

	for (Entity entity : registry.renderRequests.entities) {

		if (!registry.renderRequests.has(entity))
			continue;

		RenderRequest request = registry.renderRequests.get(entity);

		if (registry.backgrounds.has(entity)) {
			drawBackground(request, projectionMatrix, -0.5);
			continue;
		}

		if (registry.menus.has(entity)) {
			MenuItem& menu = registry.menus.get(entity);
			drawBackground(request, projectionMatrix, menu.layer);
			continue;
		}

		if (registry.texts.has(entity)) {
			drawText(entity);
			continue;
		}

		mat4 transformationMatrix;

		switch (request.geometry) {
			mat4 transformationMatrix;
			case GEOMETRY_BUFFER_IDS::QUAD: {
				auto& motion = registry.motions.get(entity);
				transformationMatrix = transform(motion.position, motion.scale, 0.f, 0);
				std::string shaderInputs[] = { "position" };
				drawQuad(request, shaderInputs, 1);
				renderToScreen(transformationMatrix, projectionMatrix);
				break;
			}

			case GEOMETRY_BUFFER_IDS::TEXTURED_QUAD: {

				// Animate player sprites
				if (registry.animations.has(entity)) {
					animateSprite(request, entity, elapsed_ms);
					continue;
				}

				// Draw a static textured quad
				auto& motion = registry.motions.get(entity);
				if (registry.buttons.has(entity)) {
					transformationMatrix = transform(motion.position, motion.scale, 0.8f, 0);
				}
				else {
					transformationMatrix = transform(motion.position, motion.scale, 0.f, 0);
				}
				std::string shaderInputs[] = { "position", "texCoord" };
				drawQuad(request, shaderInputs, 2);
				renderToScreen(transformationMatrix, projectionMatrix);
				break;
			}

			default:
				continue;

		}
	}
}

mat4 RenderSystem::transform(vec2 position, vec2 scale, float depth, float angle) {
	Transform transform;
	transform.mat = glm::translate(transform.mat, vec3(position, depth));
	transform.mat = glm::scale(transform.mat, vec3(scale, depth));
	transform.mat = glm::rotate(transform.mat, angle, vec3(0.0f, 0.0f, 1.0f));
	return transform.mat;
}

void RenderSystem::renderToScreen(mat4& transformationMatrix, mat4& projectionMatrix) {
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glHasError();
	const GLsizei numIndices = size / sizeof(uint16_t);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transformationMatrix);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projectionMatrix);
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

void RenderSystem::animateSprite(RenderRequest& request, Entity& entity, float elapsed_ms) {
	mat4 projection = createProjectionMatrix();

	Animation& animation = registry.animations.get(entity);
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
	glUniform1i(frame_uloc, animation.curr_frame);
	glUniform1f(frame_width_uloc, constants.FRAME_TEXTURE_WIDTH);
	glUniform1i(facing_left_uloc, animation.facingLeft);

	// Render to the screen
	auto& motion = registry.motions.get(entity);
	mat4 transformationMatrix = transform(motion.position, motion.scale, 0.5f, 0);
	renderToScreen(transformationMatrix, projection);

	// Resetting the texture coordinates after use
	texturedQuad[0].texCoord = { 1.f, 1.f }; // top right
	texturedQuad[1].texCoord = { 1.f, 0.f }; // bottom right
	texturedQuad[2].texCoord = { 0.f, 0.f }; // bottom left
	texturedQuad[3].texCoord = { 0.f, 1.f }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

}

void RenderSystem::drawTiles(const mat4& projectionMatrix) {
	const mat4& projection = projectionMatrix;
	const auto& tileMap = gameMap.getTileMap();
	const int& mapWidth = gameMap.getMapWidth();
	const int& mapHeight = gameMap.getMapHeight();
	const float& scaleFactor = gameMap.getTileScale();

	// Updating the texture coordinates for use with the Stone texture atlas
	texturedQuad[0].texCoord = { 0.333, 0.333 }; // top right
	texturedQuad[1].texCoord = { 0.333, 0.667 }; // bottom right
	texturedQuad[2].texCoord = { 0.0, 0.667 }; // bottom left
	texturedQuad[3].texCoord = { 0.0, 0.333 }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

	// Only draw the bottom 5 rows of tiles
	for (int i = mapHeight - 1 ; i >= 0; i--) {
		for (int j = mapWidth - 1; j >= 0; j--) {
			if (tileMap[i][j] == 1) {

				const GLuint vbo = vertexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::TEXTURED_QUAD];
				const GLuint ibo = indexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::TEXTURED_QUAD];

				// Bind buffers
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				glHasError();

				// set shaders
				const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::TEXTURE];
				glUseProgram(shaderProgram);
				glHasError();

				// Set texture
				glActiveTexture(GL_TEXTURE0);
				const GLuint texture = textures[(GLuint)TEXTURE_IDS::STONE];
				glBindTexture(GL_TEXTURE_2D, texture);

				// Pass in shader inputs
				std::string shaderInputs[] = { "position", "texCoord" };
				for (int i = 0; i < 2; i++) {
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

				GLint size = 0;
				glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
				glHasError();
				const GLsizei numIndices = size / sizeof(uint16_t);

				/* MATRIX TRANSFORMATIONS */
				mat4 transformationMatrix = transform(
					{ (0.5 + j) * scaleFactor, (0.5 + i) * scaleFactor }, 
					{ scaleFactor, scaleFactor }, 
					0.f, 0);

				GLint currProgram;
				glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

				// Setting uniform values to the currently bound program
				GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
				glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transformationMatrix);
				GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
				glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection);
				glHasError();

				glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
				glHasError();
			}
		}
	}

	// Resetting the texture coordinates after use
	texturedQuad[0].texCoord = { 1.f, 1.f }; // top right
	texturedQuad[1].texCoord = { 1.f, 0.f }; // bottom right
	texturedQuad[2].texCoord = { 0.f, 0.f }; // bottom left
	texturedQuad[3].texCoord = { 0.f, 1.f }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);
}

void RenderSystem::drawBackground(RenderRequest& request, mat4& projectionMatrix, float layer) {
	mat4 transformationMatrix = transform({ screenWidth / 2, screenHeight / 2 }, { screenWidth, screenHeight }, layer, 0);
	std::string shaderInputs[] = { "position", "texCoord" };
	drawQuad(request, shaderInputs, 2);
	renderToScreen(transformationMatrix, projectionMatrix);
}

void RenderSystem::drawText(Entity e) {
	bool isItalic = false;
	bool isBold = false;

	GLuint fontvbo;
	RenderRequest rr = registry.renderRequests.get(e);

	Motion motion = registry.motions.get(e);
	Text fonttext = registry.texts.get(e);

	//we need to do this we're dynamically creating geometry in this function
	glGenBuffers(1, &fontvbo);
	glBindBuffer(GL_ARRAY_BUFFER, fontvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set shaders
	const GLuint shaderProgram = shaders[(GLuint)SHADER_PROGRAM_IDS::FONT];
	glUseProgram(shaderProgram);
	glHasError();

	glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), fonttext.color.x, fonttext.color.y, fonttext.color.z);
	glActiveTexture(GL_TEXTURE0);

	//this is capital 'A' for reference
	float captialsize = glyphs[65].size.y;

	// iterate through all characters
	std::string::const_iterator c;
	float acc = motion.position.x;
	for (c = fonttext.text.begin(); c != fonttext.text.end(); c++)
	{
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
			assert(!bold_glyphs.empty());
			ch = italic_glyphs[*c];
		}
		else if (isBold) {
			assert(!bold_glyphs.empty());
			ch = bold_glyphs[*c];
		}
		else {
			assert(!glyphs.empty());
			ch = glyphs[*c];
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
	//glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Custom projection matrix
	auto m = registry.motions.get(e);
	m.position = {m.position.x / defaultResolution.x * screenWidth, m.position.y / defaultResolution.y * screenHeight};
	m.scale = { m.scale.x / defaultResolution.x * screenWidth, m.scale.y / defaultResolution.y * screenHeight };
	mat4 transformationMatrix = transform(m.position, m.scale, 1.0f, 0);
	mat4 projection = createProjectionMatrix();
	renderToScreen(transformationMatrix, projection);
}

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

	// Create and bind custom frame buffer
	//frameBuffer = 0;
	//glGenFramebuffers(1, &frameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	//glHasError();

	// Create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glHasError();

	// Initialize the render buffer
	//initRenderBuffer();

	// Load shaders
	ShaderManager::loadShaders(shaderPaths, shaders);

	// Load textures
	TextureManager::loadTextures(texturePaths, textures, textureDimensions);

	// Load meshes
	//loadMeshes();

	//Load vertex data
	initRenderData();

	initFonts();


	return true;
}

bool RenderSystem::initRenderBuffer() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);  

	glGenTextures(1, &renderBufferColour);
	glBindTexture(GL_TEXTURE_2D, renderBufferColour);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHasError();

	glGenRenderbuffers(1, &renderBufferDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferDepth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderBufferColour, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferDepth);
	glHasError();

	// Make sure framebuffer is complete
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

void RenderSystem::initRenderData() {

	// Create vertex buffers
	glGenBuffers((GLsizei)vertexBuffers.size(), vertexBuffers.data());

	// Create index buffers
	glGenBuffers((GLsizei)indexBuffers.size(), indexBuffers.data());

	// set vertex data

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
}

void RenderSystem::initFonts() {

	std::string path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System.ttf";
	std::string italic_path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System_Italic.ttf";
	std::string bold_path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System_Bold.ttf";
	std::vector<std::string> fontpaths = {path, italic_path, bold_path};


	//The fonts NEED to be initialized properly otherwise the anything subsequent in the program starts breaking
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	FT_Face italic_face;
	FT_Face bold_face;
	std::vector<FT_Face*> faces = { &face, &italic_face, &bold_face };


	
	//Not character size, this size is just for initial processing


	//Storing texture id of the first 128 ascii characters
	int acc = 0;
	for (FT_Face *face_element: faces) {
		// load character glyph
		if (FT_New_Face(ft, fontpaths[acc].c_str(), 0, face_element)) {
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}
		FT_Set_Pixel_Sizes(*face_element, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(*face_element, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// generate texture
			FT_Face fe = *face_element;
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				fe->glyph->bitmap.width,
				fe->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				fe->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Glyph glyph = {
				texture,
				glm::ivec2(fe->glyph->bitmap.width, fe->glyph->bitmap.rows),
				glm::ivec2(fe->glyph->bitmap_left, fe->glyph->bitmap_top),
				fe->glyph->advance.x
			};

			if (acc == 0) {
				glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
			else if (acc == 1) {
				italic_glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
			else {
				bold_glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
		}
		FT_Done_Face(*face_element);
		acc++;
	}

	//Enabling blending for EVERYTHING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	FT_Done_FreeType(ft);
}

template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_IDS oid, std::vector<T> vertices, std::vector<uint16_t> indices) {

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[(uint)oid]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glHasError();

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[(uint)oid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glHasError();
}

void RenderSystem::loadMeshes() {
	for (uint i = 0; i < meshPaths.size(); i++) {
		// Initialize meshes
		GEOMETRY_BUFFER_IDS geoIndex = meshPaths[i].first;
		std::string name = meshPaths[i].second;

		// Load mesh from .obj file
		Mesh::loadMeshFromObj(name,
			meshes[(int)geoIndex].vertices,
			meshes[(int)geoIndex].vertexIndices,
			meshes[(int)geoIndex].originalSize);

		// Bind the vertex and index buffer objects
		bindVBOandIBO(geoIndex,
			meshes[(int)geoIndex].vertices,
			meshes[(int)geoIndex].vertexIndices);
	}
}

mat4 RenderSystem::createProjectionMatrix() {
	constexpr float left = 0.f;
	constexpr float top = 0.f;
	float right = (float)this->screenWidth;
	float bottom = (float)this->screenHeight;
	constexpr float far = 1.f;
	constexpr float near = -1.f;

	const float sx = 2.f / (right - left);
	const float sy = 2.f / (top - bottom);
	const float tx = -(right + left) / (right - left);
	const float ty = -(top + bottom) / (top - bottom);
	const float zNear = -2.f / (far - near);
	const float zFar = -(far + near) / (far - near);

	mat4 projectionMatrix = {
		{sx, 0.f, 0.f, tx},
		{0.f, sy, 0.f, ty},
		{0.f, 0.f, zNear, zFar},
		{0.f, 0.f, 0.f, 1.f}};

	return transpose(projectionMatrix);
}
