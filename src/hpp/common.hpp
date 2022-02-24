#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"

// Functions

// Check for OpenGL errors
bool glHasError();

// Verbose OpenGL error reporting
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

struct Transform {
	glm::mat4 mat = glm::mat4(1.0); // 4x4 Identity matrix
};