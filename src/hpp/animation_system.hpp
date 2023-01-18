enum CharacterType {
	CAT = 1,
	DOG = 2,
};

class AnimationSystem {

public:

	AnimationSystem();
	~AnimationSystem();


	static void update_anim_orientation();


	void init();
	void step(float elapsed_ms);

private:

	static void swap_children_orientation(Entity e);
};
