enum CharacterType {
	CAT = 1,
	DOG = 2,
};

class AnimationSystem {

public:

	AnimationSystem();
	~AnimationSystem();

	//Cat animations
	static void AnimationSystem::animate_cat_walk(Entity e);
	static void AnimationSystem::animate_cat_idle(Entity e);
	static void AnimationSystem::animate_cat_jump(Entity e);
	static void AnimationSystem::animate_cat_hurt(Entity e);

	static void AnimationSystem::change_anim_orientation(Entity e);

	void init();
	void step(float elapsed_ms);

private:


};
