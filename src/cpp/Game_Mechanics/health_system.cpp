#include <hpp/Game_Mechanics/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/common.hpp>

void decreaseHealth(Entity entity, int amount, Mix_Chunk* sound) {
	auto& health = registry.health.get(entity).hp;
	health = (health - amount >= 0) ? health - amount : 0;
	Mix_PlayChannel(-1, sound, 0);
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}