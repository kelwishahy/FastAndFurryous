#include "hpp/common.hpp"
#include "hpp/components.hpp"

TEXTURE_ANIM_CONSTANTS CAT_WALK_CONSTANTS{
	4,      //Num of frames in sheet
	0.25f,  //Width of each frame in sheet normalized 
	100     //length of animation <---- tweak this one to change animation looks
};

TEXTURE_ANIM_CONSTANTS CAT_IDLE_CONSTANTS{
	2,
	0.5f,
	400.0f
};

TEXTURE_ANIM_CONSTANTS CAT_FRONT_BLINK_CONSTANTS{
	2,
	0.5f,
	400.0f
};

TEXTURE_ANIM_CONSTANTS CAT_SIDE_BLINK_CONSTANTS{
	2,
	0.5f,
	400.0f
};

// IT IS NOT EXACTLY AN ANIMATION, SO WE MIGHT NOT NEED THIS
TEXTURE_ANIM_CONSTANTS CAT_FRONT_ARM_CONSTANTS{
	1,
	1.0f,
	1.0f
};

TEXTURE_ANIM_CONSTANTS CAT_BACK_ARM_CONSTANTS{
	1,
	1.0f,
	1.0f
};

TEXTURE_ANIM_CONSTANTS CAT_JUMP_CONSTANTS{
	2,
	0.5f,
	400.0f
};

