#pragma once
#include "SDL_mixer.h"
#include "hpp/tiny_ecs.hpp"

// Decrease an entity's health to a minimum of 0
void decreaseHealth(Entity entity, int amount, Entity hurtEntity);

// Increase an entity's health to a maximum of 100
void increaseHealth(Entity entity, int amount);