#include <hpp/Game_Mechanics/health_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include "hpp/audio_manager.hpp"

void decreaseHealth(Entity entity, int amount, Entity hurtEntity) {
	auto& health = registry.health.get(entity);
	health.hp = (health.hp - amount >= 0) ? health.hp - amount : 0;
	if (health.hp == 0) {
		AnimationSystem::animate_cat_dead(hurtEntity);
		//AnimationSystem::animate_dog_dead(hurtEntity);
	}
	else {
		AnimationSystem::animate_cat_hurt(hurtEntity);
		//AnimationSystem::animate_dog_hurt(hurtEntity);
	}
	audio.play_sfx(SOUND_EFFECTS::CAT_SCREAM);
	health.hurt = true;
}

void increaseHealth(Entity entity, int amount) {
	auto& health = registry.health.get(entity).hp;
	health = (health + amount <= 100) ? health + amount : 100;
}