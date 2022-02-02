#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <vector>

using namespace glm;

#include <glm/vec2.hpp>				// vec2

// the player
Entity createCat(RenderSystem* renderer, vec2 pos);

// solid terrain
Entity createWall(RenderSystem* renderer, vec2 pos, int width, int height);


