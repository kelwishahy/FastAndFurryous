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
	glm::mat4 projection = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, -1.0f, 1.0f);
	//glm::mat3 projection = createProjectionMatrix();
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
			//Motion& motion = registry.motions.get(entity);
			//Transform transform;
			//transform.translate(motion.position);
			//transform.scale(motion.scale);

			GLint currProgram;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

			// Setting uniform values to the currently bound program
			//GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
			//glUniformMatrix4fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
			GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
			glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection);
			glHasError();

			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
			glHasError();
		}
		
	}
}


//void RenderSystem::draw() {
//	mat3 projectionMatrix = createProjectionMatrix();
//	for (unsigned int i = 0; i < registry.renderRequests.size(); i++) {
//
//		RenderRequest& renderRequest = registry.renderRequests.get(i);
//
//		// Get window size
//		int width, height;
//		glfwGetFramebufferSize(window, &width, &height);
//
//		// Render to the custom frame buffer first
//		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
//		glHasError();
//
//		// Clear the back buffer
//		glViewport(0, 0, width, height); //specifies the affine transformation of x and y from NDCs to window coordinates
//		glDepthRange(0.00001, 10); //specify mapping of depth values from NDCs to window coordinates
//		glClearColor(0.0, 0.0, 0.0, 0.0); //specify clear values for the color buffers (black)
//		glClearDepth(10.f); //specify the clear value for the depth buffer
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear colour and depth buffers to preset values
//		glEnable(GL_BLEND); // enable blending of computed fragment colors with the values in the color buffers
//
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // specify how the colors are blended together
//		glDisable(GL_DEPTH_TEST); // disable depth testing, render order needs to be set manually when alpha blending is enabled
//		glHasError();
//
//		// Draw the textured mesh
//
//		/* Transformations happen here */
//
//		// Set the shader that is being used
//		const GLuint shaderProgram = (GLuint)shaders[(GLuint)renderRequest.shader];
//		glUseProgram(shaderProgram);
//		glHasError();
//
//		// Get the vertex and index buffer objects of this render request
//		const GLuint vbo = vertexBuffers[(GLuint)renderRequest.geometry];
//		const GLuint ibo = indexBuffers[(GLuint)renderRequest.geometry];
//
//		// Bind the vertex and index buffers
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//		glHasError();
//
//		// Pass uniforms to cat.vs.glsl and cat.fs.glsl
//
//		// Find where the uniform values should be stored
//		GLint in_position_loc = glGetAttribLocation(shaderProgram, "in_position");
//		GLint in_texcoord_loc = glGetAttribLocation(shaderProgram, "in_texcoord");
//		glHasError();
//
//		// Enable the per-vertex attributes and pass in values
//		glEnableVertexAttribArray(in_position_loc);
//		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
//
//		glEnableVertexAttribArray(in_texcoord_loc);
//		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // note the stride to skip the preceeding vertex position
//		glHasError();
//
//		// Activate texture slot 0
//		glActiveTexture(GL_TEXTURE0);
//		glHasError();
//
//		// Get the texture to be rendered
//		GLuint texture = textures[(GLuint)renderRequest.texture];
//
//		// Bind the texture to slot 0
//		glBindTexture(GL_TEXTURE_2D, texture);
//		glHasError();
//
//		// Getting the fcolor uniform location
//		GLint color_uloc = glGetUniformLocation(shaderProgram, "fcolor");
//		const vec3 color =  vec3(1);
//
//		// Pass in the fcolor uniform
//		glUniform3fv(color_uloc, 1, (float*)&color);
//		glHasError();
//
//		// Get number of indices from index buffer
//		GLint size = 0;
//		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
//		glHasError();
//		GLsizei num_indices = size / sizeof(uint16_t);
//
//
//		// Get the location of the current shader program being used
//		GLint currProgram;
//		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
//
//		// Set uniform values to the currently bound shader program
//
//		/* Pass in updated transform matrix here */
//
//		// Pass in projection matrix
//		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
//		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projectionMatrix);
//
//		glHasError();
//
//		// Draw the num_indices/3 triangles specified in the index buffer
//		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
//		glHasError();
//
//		//---------------------------------------------------------------------
//		// Render to the screen
//
//		// Get window size
//		glfwGetFramebufferSize(window, &width, &height);
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, width, height);
//		glDepthRange(0, 10);
//		glClearColor(1.f, 0, 0, 1.0);
//		glClearDepth(1.f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glHasError();
//
//		// Enabling alpha channel for textures
//		glDisable(GL_BLEND);
//
//		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glDisable(GL_DEPTH_TEST);
//
//		// Draw the screen texture on the quad geometry
//		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::SCREEN_TRIANGLE]);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[(GLuint)GEOMETRY_BUFFER_IDS::SCREEN_TRIANGLE]);
//
//		// Bind our texture in Texture Unit 0
//		glActiveTexture(GL_TEXTURE0);
//
//		glBindTexture(GL_TEXTURE_2D, renderBufferColour);
//
//		glHasError();
//		// Draw
//		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,nullptr); // one triangle = 3 vertices; nullptr indicates that there is no offset from the bound index buffer
//		glHasError();
//
//		//glfwSwapBuffers(window);
//		//glHasError();
//	}
//}

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

	// Cat
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

mat3 RenderSystem::createProjectionMatrix() {
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	glHasError();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}
