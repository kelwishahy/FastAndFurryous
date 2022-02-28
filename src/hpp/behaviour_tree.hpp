#pragma once
#include <iostream>
#include <list>
#include <vector>

#include "components.hpp"
#include "tiny_ecs_registry.hpp"

/*
 * Adapted from http://www.cplusplus.com/forum/general/141582/
 */

 /*
  * NODE TYPES
  */

  // Generic node
class Node {
public:
	Entity entity;
	Node();
	Node(Entity& e) { this->entity = e; };
	virtual bool run() = 0; // Run this node
	const std::list<Node*>& getChildren() const { return children; }
	void addChild(Node* child) { children.emplace_back(child); }

private:
	std::list<Node*> children;
};

// Selector node - only one child needs to succeed
class SelectorNode : public Node {
	bool run() override {
		for (Node* child : getChildren()) {
			if (child->run())
				return true;
		}
		return false;
	}
};

// Sequence node - all children need to succceed
class SequenceNode : public Node {
	bool run() override {
		for (Node* child : getChildren()) {
			if (!child->run())
				return false;
		}
		return true;
	}
};

/*
 * TASKS
 */

// Move the entity
class Move : public Node {
public:
	Move(Entity& e) : Node(e) {
		this->motion = registry.motions.get(e);
	}

	bool run() override {
		motion.velocity.x = -velocity;
		return true;
	}
private:
	Motion motion;
	float velocity = 150.f;
};

// Stop the entity
class Stop : public Node {
public:
	Stop(Entity& e) : Node(e) {
		this->motion = registry.motions.get(e);
	}

	bool run() override {
		motion.velocity.x = 0;
		motion.velocity.y = 0;
		return true;
	}
private:
	Motion motion;
};

// Jump
// Stop the entity
class Jump : public Node {
public:
	Jump(Entity& e) : Node(e) {
		this->motion = registry.motions.get(e);
	}

	bool run() override {
		motion.velocity.y = -2.5 * 150.0;
		return true;
	}
private:
	Motion motion;
};

/*
 * Behaviour tree
 */
// class BehaviorTree {
// public:
// 	Node* BehaviourTree();
// 	~BehaviorTree();
//
// 	void addSequence();
// 	void addSelector();
// 	void run();
// private:
// 	Node* root;
// };