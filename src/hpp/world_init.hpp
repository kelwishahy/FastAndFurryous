#pragma once

#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <glm/vec2.hpp>				// vec2

// the player
Entity createCat(RenderSystem* renderer, glm::vec2 pos);

// solid terrain
Entity createWall(RenderSystem* renderer, glm::vec2 pos, int width, int height);

// AI
Entity createAI(RenderSystem* renderer, glm::vec2 pos);


