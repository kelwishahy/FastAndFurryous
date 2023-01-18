#pragma once

#include "components.hpp"
#include "render_system.hpp"

// Player entities
Entity createCat(::WEAPON_TYPES weapon, glm::vec2 pos, int health, GLFWwindow* window);

// Player entities
Entity createDog(WEAPON_TYPES weapon, glm::vec2 pos, int health, GLFWwindow* window);

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

// change option timer type
Entity createTimerCounter(float newtimer, TextManager& textManager);

Entity createGrenade(Entity originE, glm::vec2 force);

Entity createExplosion(float radius, vec2 position, Entity origin);
