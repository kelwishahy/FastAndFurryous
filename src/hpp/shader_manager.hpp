#pragma once

#include <hpp/common.hpp>

#include <string>
#include <array>


class ShaderManager {
public:

	/*
	* Loads the shaders defined in shaderPaths 
	* Store the returned OpenGL ID in shaders
	*/
	static void loadShaders(
		const std::array<std::string, shaderProgramCount>& shaderPaths, 
		std::array<GLuint, shaderProgramCount>& shaders);

private:

	// Retrieve shader path for one object
	static std::string getShaderPath(const std::string& name) { return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name; };

	// Load the fragment and vertex shaders for an object
	static bool loadShadersFromFile(const std::string& vsPath, const std::string& fsPath, GLuint& programId);

	// Compile a shader file
	static bool compileShader(GLuint programId);

};