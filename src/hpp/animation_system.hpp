enum CharacterType {
	CAT = 1,
	DOG = 2,
};

class AnimationSystem {

public:

	AnimationSystem();
	~AnimationSystem();

	//Cat animations
	static void animate_cat_walk(Entity e);
	static void animate_cat_idle(Entity e);
	static void animate_cat_jump(Entity e);
	static void animate_cat_hurt(Entity e);
	static void animate_cat_dead(Entity e);
	static void animate_cat_aim(Entity e);

	//Dog animations
	static void animate_dog_idle(Entity e);
	static void animate_dog_walk(Entity e);
	static void animate_dog_jump(Entity e);
	static void animate_dog_hurt(Entity e);
	static void animate_dog_dead(Entity e);
	static void animate_dog_aim(Entity e);

	static void update_anim_orientation();
	static void change_animation(Entity e, TEXTURE_IDS tex_id);


	void init();
	void step(float elapsed_ms);

private:

	static void swap_children_orientation(Entity e);
	static bool check_if_part_of_parent(Entity e, Entity child);

};
