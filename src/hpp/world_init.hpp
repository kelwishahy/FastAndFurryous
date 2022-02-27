#pragma once

#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <glm/vec2.hpp>				// vec2

// Player entities
Entity createCat(RenderSystem* renderer, glm::vec2 pos);

// solid terrain
Entity createWall(glm::vec2 pos, int width, int height);

// Tile
Entity createTile(float tileScale, glm::vec2 tilePosition, int numTilesInARow);

// AI
Entity createAI(RenderSystem* renderer, glm::vec2 pos);

//Projectile
Entity createProjectile(RenderSystem* renderer, Entity originE, glm::vec4 coefficientsX, glm::vec4 coefficientsY, glm::vec2 endtangent);


