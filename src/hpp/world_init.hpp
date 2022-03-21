#pragma once

#include "tiny_ecs.hpp"
#include "render_system.hpp"

// Player entities
Entity createCat(glm::vec2 pos);

// Player entities
Entity createDog(glm::vec2 pos);

// solid terrain
Entity createWall(glm::vec2 pos, float width, float height);

// Tile
Entity createTile(float tileScale, glm::vec2 tilePosition, int numTilesInARow);

// AI
Entity createAI(glm::vec2 pos);

//Projectile
Entity createProjectile(Entity originE, glm::vec2 force);

//Specify a menu
Entity createMenu(MENU_TYPES menu, float layer);

Entity createButton(glm::vec2 pos, glm::vec2 scale, TEXTURE_IDS tex_id);

Entity createText(TextManager& textManager, std::string text, glm::vec2 pos, float scale, glm::vec3 color);

Entity createCrosshair(Entity origin, bool iscat);

Entity createHealthCounter(Entity origin, int health, TextManager& textManager);

Entity createTimerCounter(int newtimer, TextManager& textManager);

Entity createPlayersCounter(int newplayers, TextManager& textManager);

