#include <hpp/shader_manager.hpp>

#include <iostream>
#include <fstream>
#include <sstream> 
#include <cassert>

void ShaderManager::loadShaders(
	const std::array<std::string, shaderProgramCount>& shaderPaths,
	std::array<GLuint, shaderProgramCount>& shaders) {
	for (GLuint i = 0; i < shaderProgramCount; i++) {
		std::string path = getShaderPath(shaderPaths[i]);
		const std::string vertexShaderName = path + ".vs.glsl";
		const std::string fragmentShaderName = path + ".fs.glsl";

		bool loaded = loadShadersFromFile(vertexShaderName, fragmentShaderName, shaders[i]);
		assert(loaded && (GLuint)shaders[i] != 0);
	}
}

bool ShaderManager::loadShadersFromFile(const std::string& vsPath, const std::string& fsPath, GLuint& shaderProgram) {
	// Opening files
	std::ifstream vertexShaderFile(vsPath);
	std::ifstream fragmentShaderFile(fsPath);

	if (!vertexShaderFile.good() || !fragmentShaderFile.good()) {
		fprintf(stderr, "Failed to load shader files %s, %s", vsPath.c_str(), fsPath.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vsStream, fsStream;
	vsStream << vertexShaderFile.rdbuf();
	fsStream << fragmentShaderFile.rdbuf();

	std::string vsStr = vsStream.str();
	std::string fsStr = fsStream.str();

	const char* vsSource = vsStr.c_str();
	const char* fsSource = fsStr.c_str();

	GLsizei vsLength = (GLsizei)vsStr.size();
	GLsizei fsLength = (GLsizei)fsStr.size();

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, &vsLength);

	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsSource, &fsLength);

	glHasError();

	// Compile the shaders
	if (!compileShader(vertexShader)) {
		fprintf(stderr, "Vertex shader compilation failed");
		assert(false);
		return false;
	}

	if (!compileShader(fragmentShader)) {
		fprintf(stderr, "Fragment shader compilation failed");
		assert(false);
		return false;
	}

	// Create a program object and attach the shaders to it
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Link the shader program
	glLinkProgram(shaderProgram);
	glHasError();

	// Log output if linking fails
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		GLint log_len;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetProgramInfoLog(shaderProgram, log_len, &log_len, log.data());
		glHasError();

		fprintf(stderr, "Link error: %s", log.data());
		assert(false);
		return false;
	}

	// Detach and delete shaders to free up resources
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glHasError();

	return true;
}

bool ShaderManager::compileShader(GLuint shader) {

	// Compile the shader
	glCompileShader(shader);
	glHasError();

	// Verify that the shader compiled successfully
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	// Log output if shader did not compile
	if (success == GL_FALSE) {
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		glHasError();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}
