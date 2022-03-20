#include <hpp/Game_Mechanics/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/common.hpp>
#include "hpp/audio_manager.hpp"
#include "hpp/animation_system.hpp"

void decreaseHealth(Entity entity, int amount, Entity hurtEntity) {
	auto& health = registry.health.get(entity).hp;
	health = (health - amount >= 0) ? health - amount : 0;
	if (health == 0) {
		AnimationSystem::animate_cat_dead(hurtEntity);
		AnimationSystem::animate_dog_dead(hurtEntity);
	}
	else {
		AnimationSystem::animate_cat_hurt(hurtEntity);
		AnimationSystem::animate_dog_hurt(hurtEntity);
	}
	audio.play_sfx(SOUND_EFFECTS::CAT_SCREAM);
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}