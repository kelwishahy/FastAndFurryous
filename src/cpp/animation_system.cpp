#include <vector>

#include <glm/vec2.hpp>				// vec2

#include "hpp/components.hpp"
#include "hpp/tiny_ecs_registry.hpp"

#include "hpp/animation_system.hpp"

AnimationSystem::AnimationSystem() {

}

AnimationSystem::~AnimationSystem() {

}

void AnimationSystem::init() {

}

void AnimationSystem::step(float elapsed_ms) {

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra& extra = registry.animExtras.get(e);
		Motion& motion_extra = registry.motions.get(e);
		Motion parent_motion = registry.motions.get(extra.parent);
		Animation& extra_anim = registry.animations.get(e);
		Animation parent_anim = registry.animations.get(extra.parent);

		extra_anim.facingLeft = parent_anim.facingLeft;
		motion_extra.position = parent_motion.position + extra.offset_from_parent;

	}

	for (Entity e : registry.animations.entities) {

		RenderRequest& rr = registry.renderRequests.get(e);
		Animation& anim = registry.animations.get(e);

		rr.texture = anim.anim_state;
		TEXTURE_ANIM_CONSTANTS anim_const = anim.animation_states_constants.at(anim.anim_state);


		anim.frame_counter_ms -= elapsed_ms;

		if (anim.frame_counter_ms <= 0) {
			anim.curr_frame = anim.frame;
			anim.curr_frame += 1;
			anim.frame = anim.curr_frame % anim_const.NUM_FRAMES;

			// Reset frame timer
			anim.frame_counter_ms = anim_const.TOTAL_FRAME_TIME;
		}
		else {
			anim.curr_frame = anim.frame;
		}

	}

}

//PLEASE ONLY PASS IN CATS THAT HAVE THE CORRECT ANIM_CONSTATNST LOADED
void AnimationSystem::animate_cat_walk(Entity e) {

	//Animate the cat's main torso
	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::CAT_WALK;

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "cat_head") {
			//Animate the head
			Animation& headAnim = registry.animations.get(e);
			headAnim.anim_state = TEXTURE_IDS::CAT_SIDE_BLINK;
			// change position of the head if looking left
			// not working yet
			if (anim.facingLeft == true) {
				Motion& headmotion = registry.motions.get(e);
				printf("Position old X %d\n", headmotion.position);
				headmotion.position.x += 100.f;
				printf("Position new X %d\n", headmotion.position);
			}
		}
	}
}

void AnimationSystem::animate_cat_idle(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::CAT_FRONT_IDLE;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "cat_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::CAT_FRONT_BLINK;
		}
	}
}

void AnimationSystem::animate_cat_jump(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::CAT_JUMP;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "cat_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::CAT_FRONT_BLINK;
		}

	}
}

void AnimationSystem::animate_cat_hurt(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::CAT_HURT;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "cat_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::CAT_HURT_FACE;
		}

	}
}

void AnimationSystem::animate_cat_dead(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::CAT_DEAD;

	for (Entity e : registry.animExtras.entities) {

		// remove head and arms
		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "cat_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
		if (extra.tag == "cat_frontArm") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
		if (extra.tag == "cat_backArm") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
	}
}

void AnimationSystem::animate_dog_idle(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::DOG_FRONT_IDLE;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "dog_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::DOG_FRONT_BLINK;
		}
	}
}

void AnimationSystem::animate_dog_walk(Entity e) {

	//Animate the cat's main torso
	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::DOG_WALK;

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "dog_head") {
			//Animate the head
			Animation& headAnim = registry.animations.get(e);
			headAnim.anim_state = TEXTURE_IDS::DOG_SIDE_BLINK;
			// change position of the head if looking left
			// not working yet
			if (anim.facingLeft == true) {
				Motion& headmotion = registry.motions.get(e);
				printf("Position old X %d\n", headmotion.position);
				headmotion.position.x += 100.f;
				printf("Position new X %d\n", headmotion.position);
			}
		}
	}
}

void AnimationSystem::animate_dog_jump(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::DOG_JUMP;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "dog_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::DOG_FRONT_BLINK;
		}

	}
}

void AnimationSystem::animate_dog_hurt(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::DOG_HURT;

	for (Entity e : registry.animExtras.entities) {

		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "dog_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::DOG_HURT_FACE;
		}

	}
}

void AnimationSystem::animate_dog_dead(Entity e) {

	Animation& anim = registry.animations.get(e);
	anim.anim_state = TEXTURE_IDS::DOG_DEAD;

	for (Entity e : registry.animExtras.entities) {

		// remove head and arms
		AnimationExtra extra = registry.animExtras.get(e);
		if (extra.tag == "dog_head") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
		if (extra.tag == "dog_frontArm") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
		if (extra.tag == "dog_backArm") {
			Animation& headanim = registry.animations.get(e);
			headanim.anim_state = TEXTURE_IDS::EMPTY;
		}
	}
}
//
//void AnimationSystem::change_anim_orientation(Entity e) {
//
//}