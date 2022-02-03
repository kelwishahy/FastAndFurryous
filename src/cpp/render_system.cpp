#include <iostream>

#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>
#include <hpp/texture_manager.hpp>

#include <hpp/common.hpp>
#include <hpp/components.hpp>
#include <hpp/tiny_ecs_registry.hpp>

using namespace glm;

RenderSystem::RenderSystem() {}

// Destructor
RenderSystem::~RenderSystem() {
	glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void RenderSystem::draw() {
	mat3 projectionMatrix = createProjectionMatrix();
	for (uint i = 0; i < registry.renderRequests.size(); i++) {

		RenderRequest& renderRequest = registry.renderRequests.get(i);

		const GLuint shader = (GLuint)shaders[(GLuint)renderRequest.shader];

		// Setting shaders
		glUseProgram(shader);
		glHasError();

		const GLuint vbo = vertexBuffers[(GLuint)renderRequest.geometry];
		const GLuint ibo = indexBuffers[(GLuint)renderRequest.geometry];

		// Setting vertex and index buffers
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glHasError();

		GLint in_position_loc = glGetAttribLocation(shader, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(shader, "in_texcoord");
		glHasError();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		glHasError();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glHasError();

		GLuint texture = textures[(GLuint)renderRequest.texture];

		glBindTexture(GL_TEXTURE_2D, texture);
		glHasError();

		// Getting uniform locations for glUniform* calls
		//GLint color_uloc = glGetUniformLocation(program, "fcolor");
		//const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
		//glUniform3fv(color_uloc, 1, (float*)&color);
		//gl_has_errors();

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glHasError();

		GLsizei num_indices = size / sizeof(uint16_t);
		GLsizei num_triangles = num_indices / 3;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		//GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		//glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projectionMatrix);
		glHasError();
		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		glHasError();

		glfwSwapBuffers(window);
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

bool RenderSystem::init() {
	// Create window & context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	// Initialize vertex array object
	vaoId = 1;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// Create and bind frame buffer
	frameBuffer = 0;
	glGenBuffers(1, &frameBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, frameBuffer);
	glHasError();

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Load shaders
	ShaderManager::loadShaders(shaderPaths, shaders);

	// Load textures
	TextureManager::loadTextures(texturePaths, textures, textureDimensions);

	loadMeshes();

	//Load vertex data
	initRenderData();

	return true;
}

void RenderSystem::initRenderData() {

	// Create vertex buffers
	glGenBuffers((GLsizei)vertexBuffers.size(), vertexBuffers.data());

	// Create index buffers
	glGenBuffers((GLsizei)indexBuffers.size(), indexBuffers.data());


	// Load vertex data

	// Cat
	std::vector<TexturedVertex> texturedVertices(4);
	texturedVertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	texturedVertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	texturedVertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	texturedVertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	texturedVertices[0].texCoord= { 0.f, 1.f };
	texturedVertices[1].texCoord = { 1.f, 1.f };
	texturedVertices[2].texCoord = { 1.f, 0.f };
	texturedVertices[3].texCoord = { 0.f, 0.f };

	const std::vector<uint16_t> texturedIndices = { 0, 3, 1, 1, 3, 2 };
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
