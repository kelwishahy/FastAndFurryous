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

	for (Entity e : registry.animations.entities) {
		Motion& catMotion = registry.motions.get(e);
		Animation& catAnimation = registry.animations.get(e);

		if (catMotion.velocity.x < 0) {
			catAnimation.facingLeft = true;
		}
		if (catMotion.velocity.x > 0) {
			catAnimation.facingLeft = false;
		}
	}


	for (int i = 0; i < registry.parentEntities.components.size(); i++) {
		ChildEntities& children = registry.parentEntities.components[i];
		Entity e = registry.parentEntities.entities[i];


		//so everything swaps sides properly
		for (int j = 0; j < children.child_data_map.size(); j++) {
			Entity child = children.child_data_map.at(j);
			if (registry.animations.has(e) && registry.animations.has(child)) {
				Animation parent_anim = registry.animations.get(e);
				Animation& anchored_anim = registry.animations.get(child);
				anchored_anim.facingLeft = parent_anim.facingLeft;
			}
		}
	}
	update_anim_orientation();

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
		}

	}
}

void AnimationSystem::animate_cat_dead(Entity e) {
	remove_children(e);
	change_animation(e, TEXTURE_IDS::CAT_DEAD);
	registry.cats.remove(e);
}

void AnimationSystem::animate_dog_idle(Entity e) {

	change_animation(e, TEXTURE_IDS::DOG_FRONT_IDLE);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			change_animation(rig, TEXTURE_IDS::DOG_FRONT_BLINK);
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
		}

	}
}

void AnimationSystem::animate_dog_dead(Entity e) {
	
	remove_children(e);
	change_animation(e, TEXTURE_IDS::DOG_DEAD);
	registry.dogs.remove(e);
}
//
void AnimationSystem::update_anim_orientation() {
	//Mirroring the anchors
	for (int i = 0; i < registry.animations.components.size(); i++) {
		Entity e = registry.animations.entities[i];
		if (registry.parentEntities.has(e)) {
			bool orientation = registry.animations.get(e).facingLeft;
			ChildEntities& children = registry.parentEntities.get(e);
			for (int j = 0; j < children.child_data_map.size(); j++) {
				if (registry.animations.has(children.child_data_map.at(j))) {
					Animation child = registry.animations.get(children.child_data_map.at(j));
					if (child.name == "cat_head" || child.name == "cat_front_arm" || child.name == "cat_back_arm" || child.name == "dog_head" || child.name == "dog_front_arm" || child.name == "dog_back_arm") {
						if (child.facingLeft == true) {
							children.normal_dists[j].x = -children.original_dists[j].x;
						}
						else {
							children.normal_dists[j].x = children.original_dists[j].x;
						}
					} 
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