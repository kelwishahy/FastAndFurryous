#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"

// Variables 
const int window_width_px = 1000;
const int window_height_px = 800;

// Functions

// Check for OpenGL errors
bool glHasError();

// More informative OpenGL error reporting
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

struct Transform {
	glm::mat3 mat = glm::mat3(1.0); // 3x3 Identity matrix
	void scale(glm::vec2 scale);
	void rotate(float radians);
	void translate(glm::vec2 offset);
};