// internal
#include "../src/hpp/ai_system.hpp"
#include <hpp/behaviour_tree.hpp>

#include "glm/ext.hpp"

using namespace glm;

const uint VELOCITY_CHANGE_DELAY = 3500;

void AISystem::step(float elapsed_ms)
{
	ComponentContainer<AI>& ai_container = registry.ais;
	for (uint i = 0; i < ai_container.components.size(); i++) {
		Entity& entity = ai_container.entities[i];
		Motion& motion = registry.motions.get(entity);
		// if (timer <= 0) {
		// 	motion.velocity.x = direction * uniform_dist(rng) * 100;
		// 	timer = VELOCITY_CHANGE_DELAY;
		// 	direction *= -1;
		// }

		// Don't jump for now
		// if (jumpdelay <= 0) {
		// 	motion.velocity.y = -300.f;
		// 	jumpdelay = uniform_dist(rng) * 100000;
		// }

		blackboard->entity = &entity;
		blackboard->motion = &motion;
		behaviourTree->run();
		timer -= elapsed_ms;
		
		// jumpdelay -= elapsed_ms;

	}
	
}

//initialize stuff here
void AISystem::init() {
	timer = 0;
	direction = 1;
	jumpdelay = 2000;

	// Behaviour tree
	blackboard = new Blackboard;
	behaviourTree = new SequenceNode();
	// auto*sequence1 = new SequenceNode;
	auto* selector1 = new SelectorNode;
	auto* moveLeft = new MoveLeft;
	// auto* jump = new Jump;
	auto* stop = new Stop;
	
	// behaviourTree->addChild(sequence1);
	behaviourTree->addChild(selector1);
	
	// sequence1->addChild(jump);
	// sequence1->addChild(moveLeft);
	// sequence1->addChild(stop);

	// selector1->addChild(jump);
	selector1->addChild(moveLeft);
	selector1->addChild(stop);
}