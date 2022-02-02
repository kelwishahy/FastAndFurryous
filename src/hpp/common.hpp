#pragma once

#include <glad/glad.h>
#include "../ext/project_path.hpp"
#include <cassert>
#include <iostream>
#include <array>
#include <vector>

// glm
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

//Functions

// Check for OpenGL errors
bool glHasError();

const int window_width_px = 600;
const int window_height_px = 900;

////////////////////////////////////////////////////////////////////////////////
// Asset IDs
////////////////////////////////////////////////////////////////////////////////

enum class SHADER_PROGRAM_IDS {
	CAT,
	TRIANGLE,
	TOTAL
}; const int shaderProgramCount = (int)SHADER_PROGRAM_IDS::TOTAL;

enum class TEXTURE_IDS {
	CAT,
	TRIANGLE,
	TOTAL
}; const int textureCount = (int)TEXTURE_IDS::TOTAL;

enum class OBJECT_BUFFER_IDS {
	CAT,
	TRIANGLE,
	TOTAL
}; const int objectCount = (int)OBJECT_BUFFER_IDS::TOTAL;

////////////////////////////////////////////////////////////////////////////////
// Data Types
////////////////////////////////////////////////////////////////////////////////

struct TexturedVertex {
	glm::vec3 position;
	glm::vec2 texcoord;
};