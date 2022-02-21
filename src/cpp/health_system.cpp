#include <hpp/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>

void decreaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health - amount >= 0) ? health - amount : 0;
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}