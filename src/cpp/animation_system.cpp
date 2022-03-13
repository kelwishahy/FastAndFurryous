#include <array>
#include <vector>
#include <string>
#include <map>

#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3

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

//void AnimationSystem::animate_cat_walk(Entity e) {
//
//}
//
//void AnimationSystem::change_anim_orientation(Entity e) {
//
//}