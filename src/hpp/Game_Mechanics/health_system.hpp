#pragma once
#include "hpp/tiny_ecs.hpp"

// Decrease an entity's health to a minimum of 0
void decreaseHealth(Entity entity, int amount);

// Increase an entity's health to a maximum of 100
void increaseHealth(Entity entity, int amount);