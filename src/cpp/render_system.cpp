#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>
#include <hpp/texture_manager.hpp>

#include <hpp/common.hpp>
#include <hpp/components.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "glm/ext.hpp"
#include <iostream>


using namespace glm;

RenderSystem::RenderSystem() {}

// Destructor
RenderSystem::~RenderSystem() {
	glfwTerminate();
}

void RenderSystem::draw(float elapsed_ms) {
	glm::mat4 projection = createProjectionMatrix();
	glViewport(0, 0, this->screenWidth, this->screenHeight);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawTiles();

	for (Entity entity : registry.renderRequests.entities) {

		if (!registry.renderRequests.has(entity))
			continue;

		RenderRequest request = registry.renderRequests.get(entity);

		switch (request.geometry) {

		case GEOMETRY_BUFFER_IDS::QUAD: {
			std::string shaderInputs[] = { "position" };
			drawQuad(request, shaderInputs, 1);
			break;
		}

		case GEOMETRY_BUFFER_IDS::TEXTURED_QUAD: {

			if (registry.players.has(entity)) { // Animate player sprites
				animateSprite(entity, elapsed_ms);
			}
			else { // Draw a static textured quad
				std::string shaderInputs[] = { "position", "texCoord" };
				drawQuad(request, shaderInputs, 2);
			}
			break;
		}

		default:
			continue;
		}

		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glHasError();
		const GLsizei numIndices = size / sizeof(uint16_t);

		/* MATRIX TRANSFORMATIONS */
		Motion& motion = registry.motions.get(entity);
		Transform transform;
		transform.mat = translate(transform.mat, vec3(motion.position, 0.0f));
		transform.mat = scale(transform.mat, vec3(motion.scale, 0.f));

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection);
		glHasError();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
		glHasError();
	}
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
	}

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
}

void animateSprite(Entity& entity, float elapsed_ms) {

	// Frames for animation
	GLint curr_frame = 0;
	GLfloat frame_width = 0;
	int numFrames = 0;
	int timePerFrame = 0;

	// Decrement the frame counter
	float* counter = &registry.players.get(entity).frame_counter_ms;

	*counter -= elapsed_ms;

	// Get the type of animation (IDLE, WALKING)
	int animationType = registry.players.get(entity).animation_type;

	// Get the type of character (CAT, DOG)
	int characterType = registry.players.get(entity).character;

	// Get if character is facing left or not
	int facingLeft = registry.players.get(entity).facingLeft;

	// Get frame
	int* frame = &registry.players.get(entity).frame;

	// Get the current texture to alter
	TEXTURE_IDS& curr_texture = registry.renderRequests.get(entity).texture;
	// Get the current geometry to alter
	GEOMETRY_BUFFER_IDS& curr_geometry = registry.renderRequests.get(entity).geometry;

	switch (characterType) {
	case CAT: {
		switch (animationType) {
		case IDLE: {
			curr_texture = TEXTURE_IDS::CAT_IDLE;
			numFrames = CAT_IDLE_FRAMES;
			frame_width = CAT_IDLE_FRAME_WIDTH;
			timePerFrame = CAT_IDLE_FRAME_TIME;
			break;
		}

		case WALKING: {
			if (curr_geometry != GEOMETRY_BUFFER_IDS::CAT_WALK) {
				curr_geometry = GEOMETRY_BUFFER_IDS::CAT_WALK;
				*frame = 0;
			}
			curr_texture = TEXTURE_IDS::CAT_WALK;
			numFrames = CAT_WALK_FRAMES;
			frame_width = CAT_WALK_FRAME_WIDTH;
			timePerFrame = CAT_WALK_FRAME_TIME;
			break;
		}

		case JUMPING: {
			if (curr_geometry != GEOMETRY_BUFFER_IDS::CAT_JUMP) {
				curr_geometry = GEOMETRY_BUFFER_IDS::CAT_JUMP;
				*frame = 0;
			}
			curr_texture = TEXTURE_IDS::CAT_JUMP;
			numFrames = CAT_JUMP_FRAMES;
			frame_width = CAT_JUMP_FRAME_WIDTH;
			timePerFrame = CAT_JUMP_FRAME_TIME;
			break;
		}

		default: break;
		}

		if (*counter <= 0) {
			if (registry.players.has(entity)) {
				curr_frame = *frame;
				curr_frame += 1;
				*frame = curr_frame % numFrames;
			}
			// Reset frame timer
			*counter = timePerFrame;
		}
		else {
			curr_frame = *frame;
		}

		RenderRequest request = registry.renderRequests.get(entity);

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

		// Set texture
		glActiveTexture(GL_TEXTURE0);
		const GLuint texture = textures[(GLuint)request.texture];
		glBindTexture(GL_TEXTURE_2D, texture);

		// Pass in vertex attributes
		GLint in_position_loc = glGetAttribLocation(shaderProgram, "aPos");
		glHasError();

		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		glEnableVertexAttribArray(in_position_loc);
		glHasError();

		GLint aTexCoordLoc = glGetAttribLocation(shaderProgram, "aTexCoord");
		glHasError();

		glVertexAttribPointer(aTexCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
		glEnableVertexAttribArray(aTexCoordLoc);
		glHasError();

		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glHasError();
		const GLsizei numIndices = size / sizeof(uint16_t);

		/* MATRIX TRANSFORMATIONS */
		Motion& motion = registry.motions.get(entity);
		Transform transform;
		transform.mat = translate(transform.mat, vec3(motion.position, 0.0f));
		transform.mat = scale(transform.mat, vec3(motion.scale, 0.f));

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

		// pass the frame values to the shader as uniform
		GLint frame_uloc = glGetUniformLocation(shaderProgram, "curr_frame");
		GLfloat frame_width_uloc = glGetUniformLocation(shaderProgram, "frame_width");
		GLint facing_left_uloc = glGetUniformLocation(shaderProgram, "facingLeft");
		glUniform1i(frame_uloc, curr_frame);
		glUniform1f(frame_width_uloc, frame_width);
		glUniform1i(facing_left_uloc, facingLeft);

		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection);
		glHasError();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
		glHasError();
	}
	}
}

void RenderSystem::drawTiles() {
	glm::mat4 projection = createProjectionMatrix();
	const int mapWidth = 30;
	const int mapHeight = 20;

	// This map should be stored on disk and passed in
	// instead of being regenerated every time
	std::array<std::array<int, mapWidth>, mapHeight> tilemap;
	for (int x = 0; x < mapHeight; x++) {
		for (int y = 0; y < mapWidth; y++) {
			tilemap[x][y] = 1;
		}
	}

	// Updating the texture coordinates for use with the Stone texture atlas
	texturedQuad[0].texCoord = { 0.333, 0.333 }; // top right
	texturedQuad[1].texCoord = { 0.333, 0.667 }; // bottom right
	texturedQuad[2].texCoord = { 0.0, 0.667 }; // bottom left
	texturedQuad[3].texCoord = { 0.0, 0.333 }; // top left
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::TEXTURED_QUAD, texturedQuad, quadIndices);

	// Only draw the bottom 5 rows of tiles
	for (int i = mapHeight -1 ; i >= 15; i--) {
		for (int j = mapWidth - 1; j >= 0; j--) {
			if (tilemap[i][j] == 1) {

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
				Transform transform;
				float scaleFactor = 64.f;
				transform.mat = translate(transform.mat, vec3((0.5 + j) * scaleFactor, (0.5 + i) * scaleFactor, 0.0f));
				transform.mat = scale(transform.mat, vec3(scaleFactor, scaleFactor, 0.f));

				GLint currProgram;
				glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

				// Setting uniform values to the currently bound program
				GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
				glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
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
	this->window = glfwCreateWindow(1000, 800, "The Fast and the Furryous", NULL, NULL);
	glfwSetWindowAspectRatio(window, 16, 9);
	glfwGetWindowSize(window, &this->screenWidth, &this->screenHeight);

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
