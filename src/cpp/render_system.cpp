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

void RenderSystem::draw() {
	glm::mat4 projection = createProjectionMatrix();
	for (Entity entity : registry.renderRequests.entities) {
		if (!registry.renderRequests.has(entity))
			continue;

		RenderRequest request = registry.renderRequests.get(entity);

		// Draw a basic triangle to the screen
		if (request.geometry == GEOMETRY_BUFFER_IDS::CAT) {

			const GLuint vbo = vertexBuffers[(GLuint)request.geometry];
			const GLuint ibo = indexBuffers[(GLuint)request.geometry];

			// Bind buffers
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glHasError();

			glViewport(0, 0, window_width_px, window_height_px);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

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

bool RenderSystem::init() {
	// Create window & context
	glfwInit();
	// Using OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // Request GLFW debug context

	this->window = glfwCreateWindow(window_width_px, window_height_px, "The Fast and the Furryous", NULL, NULL);

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

	// Cat sprite
	std::vector<TexturedVertex> texturedVertices(4);
	texturedVertices[0].position = { 0.5f,  0.5f, 0.0f }; // top right
	texturedVertices[1].position = { 0.5f, -0.5f, 0.0f }; // bottom right
	texturedVertices[2].position = { -0.5f, -0.5f, 0.0f }; // bottom left
	texturedVertices[3].position = { -0.5f,  0.5f, 0.0f }; // top left
	texturedVertices[0].texCoord= { 1.0f, 1.0f };
	texturedVertices[1].texCoord = { 1.0f, 0.0f };
	texturedVertices[2].texCoord = { 0.0f, 0.0f };
	texturedVertices[3].texCoord = { 0.0f, 1.0f };

	const std::vector<uint16_t> texturedIndices = { 2, 0, 3, 2, 1, 0 };
	bindVBOandIBO(GEOMETRY_BUFFER_IDS::CAT, texturedVertices, texturedIndices);
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
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;
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
