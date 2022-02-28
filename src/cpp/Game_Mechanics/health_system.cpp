#include <hpp/Game_Mechanics/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>

void decreaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health - amount >= 0) ? health - amount : 0;
	printf("New health = %d\n", health);
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}