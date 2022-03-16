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

	for (int i = 0; i < registry.anchors.components.size(); i++) {
		AnchoredEntities anchor = registry.anchors.components[i];
		Entity e = registry.anchors.entities[i];


		//so everything swaps sides properly 
		if (anchor.tag == "animation") {
			assert(registry.animations.has(e));
			assert(registry.animations.has(anchor.child));
			Animation parent_anim = registry.animations.get(e);
			Animation& anchored_anim = registry.animations.get(anchor.child);
			anchored_anim.facingLeft = parent_anim.facingLeft;
			update_anim_orientation();
		}


	}

	//Animate sprites
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
	change_animation(e, TEXTURE_IDS::CAT_WALK);

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::CAT_SIDE_BLINK);
		} 
	}

}

void AnimationSystem::animate_cat_idle(Entity e) {

	change_animation(e, TEXTURE_IDS::CAT_FRONT_IDLE);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::CAT_FRONT_BLINK);
		}

	}
}

void AnimationSystem::animate_cat_jump(Entity e) {

	change_animation(e, TEXTURE_IDS::CAT_JUMP);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::CAT_FRONT_BLINK);
			// change position of the head if looking left
		}

	}
}

void AnimationSystem::animate_cat_hurt(Entity e) {

	change_animation(e, TEXTURE_IDS::CAT_HURT);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::CAT_HURT_FACE);
			// change position of the head if looking left
		}

	}
}

void AnimationSystem::animate_cat_dead(Entity e) {

}

void AnimationSystem::animate_dog_idle(Entity e) {

	change_animation(e, TEXTURE_IDS::DOG_FRONT_IDLE);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::DOG_HURT_FACE);
			// change position of the head if looking left
		}

	}
}

void AnimationSystem::animate_dog_walk(Entity e) {

	///Animate the cat's main torso
	change_animation(e, TEXTURE_IDS::DOG_WALK);

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::DOG_SIDE_BLINK);
		}
	}
}

void AnimationSystem::animate_dog_jump(Entity e) {

	change_animation(e, TEXTURE_IDS::DOG_JUMP);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::DOG_FRONT_BLINK);
			// change position of the head if looking left
		}

	}
}

void AnimationSystem::animate_dog_hurt(Entity e) {

	change_animation(e, TEXTURE_IDS::DOG_HURT);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::DOG_HURT_FACE);
			// change position of the head if looking left
		}

	}
}

void AnimationSystem::animate_dog_dead(Entity e) {

}
//
void AnimationSystem::update_anim_orientation() {
	//Mirroring the anchors
	for (int i = 0; i < registry.anchors.components.size(); i++) {
		AnchoredEntities& anchor = registry.anchors.components[i];
		if (registry.animations.has(anchor.child)) {
			Animation child = registry.animations.get(anchor.child);
			if (child.name == "cat_head" || child.name == "cat_front_arm" || child.name == "cat_back_arm") {
				if (child.facingLeft == true) {
					anchor.normal_distance.x = -anchor.original_distance.x;
				}
				else {
					anchor.normal_distance.x = anchor.original_distance.x;
				}
			}
		}
	}
}

void AnimationSystem::change_animation(Entity e, TEXTURE_IDS tex_id) {

	assert(registry.animations.has(e));
	Animation& animation = registry.animations.get(e);
	animation.anim_state = tex_id;

}