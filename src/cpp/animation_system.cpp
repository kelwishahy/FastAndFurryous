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
		if (registry.characters.has(e)) {
			Motion& motion = registry.motions.get(e);
			Animation& anim = registry.animations.get(e);

			if (motion.velocity.x < 0) {
				anim.facingLeft = true;
				swap_children_orientation(e);
			}
			if (motion.velocity.x > 0) {
				anim.facingLeft = false;
				swap_children_orientation(e);
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

void AnimationSystem::swap_children_orientation(Entity e) {

	if (registry.animations.has(e)) {
		bool parent_orientation = registry.animations.get(e).facingLeft;

		if (registry.parentEntities.has(e)) {
			ChildEntities& children = registry.parentEntities.get(e);
			for (auto pair : children.child_data_map) {
				if (registry.animations.has(pair.second)) {
					Animation& child_anim = registry.animations.get(pair.second);
					child_anim.facingLeft = parent_orientation;
				}
				swap_children_orientation(pair.second);
			}
		}
	}

}

