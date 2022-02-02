#include <iostream>

#include <hpp/render_system.hpp>
#include <hpp/shader_manager.hpp>
#include <hpp/texture_manager.hpp>

#include <hpp/common.hpp>
#include <hpp/components.hpp>

using namespace glm;

RenderSystem::RenderSystem() {
	assert(init());
}

// Destructor
RenderSystem::~RenderSystem() {
	glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void RenderSystem::draw(const SHADER_PROGRAM_IDS shaderProgram) {
	// Pass in vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(shaders[(GLuint)shaderProgram]);
	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glHasError();
}

bool RenderSystem::init() {
	// Create window & context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->window = glfwCreateWindow(1000, 800, "The Fast and the Furryous", NULL, NULL);

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
	//frameBufferId = 0;
	//glGenBuffers(1, &frameBufferId);
	//glBindBuffer(GL_ARRAY_BUFFER, frameBufferId);
	glHasError();

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Load shaders
	ShaderManager::loadShaders(shaderPaths, shaders);

	// Load textures
	TextureManager::loadTextures(texturePaths, textures, textureDimensions);

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