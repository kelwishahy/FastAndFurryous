#pragma once

#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <glm/vec2.hpp>				// vec2

// Player entities
Entity createCat(RenderSystem* renderer, glm::vec2 pos);

// solid terrain
Entity createWall(RenderSystem* renderer, glm::vec2 pos, int width, int height);

// Tile
Entity createTile();

// AI
Entity createAI(RenderSystem* renderer, glm::vec2 pos);

//Projectile
Entity createProjectile(RenderSystem* renderer, glm::vec2 pos, glm::vec4 coefficientsX, glm::vec4 coefficientsY);


