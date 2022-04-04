#pragma once
#include "hpp/tiny_ecs.hpp"

// change option timer type
int decreaseTimer(Entity entity, float amount);

int increaseTimer(Entity entity, float amount);

int decreasePlayers(Entity entity, int amount);

int increasePlayers(Entity entity, int amount);