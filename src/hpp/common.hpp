#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include <ft2build.h>
#include FT_FREETYPE_H 

// Functions
// Simplicity for audio path

#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "assets"; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audios/" + std::string(name); };

// Check for OpenGL errors
bool glHasError();

// Verbose OpenGL error reporting
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

struct Transform {
	glm::mat4 mat = glm::mat4(1.0); // 4x4 Identity matrix
};

// Default screen resolution
constexpr glm::vec2 defaultResolution = glm::vec2(1920.f, 1080.f);

// Global screen resolution
extern glm::vec2 screenResolution;

// scale to screen size
glm::vec2 scaleToScreenResolution(glm::vec2 defaultScale);

float naive_lerp(float a, float b, float t);