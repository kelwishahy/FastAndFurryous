#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// All game components in alphabetical order
	ComponentContainer<AI> ais;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<Boxcollider> boxColliders;
	ComponentContainer<Circlecollider> circleColliders;
	ComponentContainer<Collision> collisions;
	ComponentContainer<glm::vec3> colors;
	ComponentContainer<Health> health;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<Motion> motions;
	ComponentContainer<RayCast> rayCasts;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<Rigidbody> rigidBodies;
	ComponentContainer<SolidTerrain> terrains;
	ComponentContainer<Tile> tiles;
	ComponentContainer<WeaponBase> weapons;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<Animation> animations;
	ComponentContainer<MenuItem> menus;
	ComponentContainer<Clickable> buttons;
	ComponentContainer<Text> texts;
	ComponentContainer<ChildEntities> parentEntities;
	ComponentContainer<UIElement> uiElements;
	ComponentContainer<Character*> characters;
	ComponentContainer<HealthBox> healthboxes;
	ComponentContainer<Selected> selected;
	ComponentContainer<OptionTimer> timer;
	ComponentContainer<Grenade> grenades;
	ComponentContainer<Timer> entityTimers;
	ComponentContainer<Explosion> explosions;
	ComponentContainer<Particle> particles;

	ECSRegistry() {
		registry_list.push_back(&ais);
		registry_list.push_back(&animations);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&boxColliders);
		registry_list.push_back(&buttons);
		registry_list.push_back(&characters);
		registry_list.push_back(&circleColliders);
		registry_list.push_back(&collisions);
		registry_list.push_back(&colors);
		registry_list.push_back(&health);
		registry_list.push_back(&menus);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&motions);
		registry_list.push_back(&parentEntities);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&rayCasts);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&rigidBodies);
		registry_list.push_back(&selected);
		registry_list.push_back(&terrains);
		registry_list.push_back(&texts);
		registry_list.push_back(&uiElements);
		registry_list.push_back(&healthboxes);
		registry_list.push_back(&tiles);
		registry_list.push_back(&uiElements);
		registry_list.push_back(&weapons);
		registry_list.push_back(&timer);
		registry_list.push_back(&entityTimers);
		registry_list.push_back(&explosions);
		registry_list.push_back(&particles);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;
