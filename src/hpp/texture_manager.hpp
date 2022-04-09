#pragma once

#include <../ext/project_path.hpp>
#include <hpp/components.hpp>

#include <glad/glad.h>
#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <string>

class TextureManager {
public:

	/*
	* Loads the textures defined in texturePaths
	* Stores the returned OpenGL ID in textures, and the texture dimensions in textureDimensions
	*/
	static void loadTextures(
		const std::array<std::string, textureCount>& texturePaths,
		std::array<GLuint, textureCount>& textures,
		std::array<glm::ivec2, textureCount>& textureDimensions);

private:
	static std::string getTexturePath(const std::string& name) { return std::string(PROJECT_SOURCE_DIR) + "assets/textures/" + std::string(name); };
};
