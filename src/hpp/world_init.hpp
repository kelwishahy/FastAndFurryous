#pragma once

#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <glm/vec2.hpp>				// vec2

// Player entities
Entity createCat(RenderSystem* renderer, glm::vec2 pos);

// Player entities
Entity createDog(RenderSystem* renderer, glm::vec2 pos);

// solid terrain
Entity createWall(glm::vec2 pos, float width, float height);

// Tile
Entity createTile(float tileScale, glm::vec2 tilePosition, int numTilesInARow);

// AI
Entity createAI(RenderSystem* renderer, glm::vec2 pos);

//Projectile
Entity createProjectile(RenderSystem* renderer, Entity originE, glm::vec2 force);

//Specify a menu
Entity createMenu(MENU_TYPES menu, float layer);

Entity createButton(glm::vec2 pos, glm::vec2 scale, TEXTURE_IDS tex_id, std::vector<std::function<void()>> callbacks);

Entity createText(glm::vec2 pos, float scale, glm::vec3 color, std::string text);

Entity createCrosshair(Entity origin, bool iscat);

Entity createHealthCounter(Entity origin, int health);

void remove_children(Entity e);