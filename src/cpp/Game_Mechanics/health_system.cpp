#include <hpp/Game_Mechanics/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "hpp/audio_manager.hpp"

void decreaseHealth(Entity entity, int amount, Entity hurtEntity) {
	auto& health = registry.health.get(entity).hp;
	health = (health - amount >= 0) ? health - amount : 0;
	audio.play_sfx(SOUND_EFFECTS::CAT_SCREAM);
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}