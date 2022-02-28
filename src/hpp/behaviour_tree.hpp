#pragma once

#include <chrono>
#include <list>
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
using Clock = std::chrono::high_resolution_clock;

/*
 * Adapted from http://www.cplusplus.com/forum/general/141582/
 */

struct Blackboard {
	Entity* entity;
	Motion* motion;
	bool walking;
};

static Blackboard* blackboard;

 /*
  * NODE TYPES
  */

  // Generic node
class Node {
public:
	Node() {};
	~Node() {
		for (const Node* child : children) {
			delete(child);
		}
	}
	virtual bool run() = 0; // Run this node
	const std::list<Node*>& getChildren() const { return children; }
	void addChild(Node* child) { children.emplace_back(child); }

private:
	std::list<Node*> children;
};

// Selector node - only one child needs to succeed
class SelectorNode : public Node {
public:
	SelectorNode() {};
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
public:
	SequenceNode() {};
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
class MoveLeft : public Node {
public:
	MoveLeft() {};

	bool run() override {
		if (!blackboard->walking) {
			blackboard->motion->velocity.x = -velocity;
			blackboard->walking = true;
			return true;
		}
		return false;
	}
private:
	float velocity = 150.f;
};

// Stop the entity
class Stop : public Node {
public:
	Stop() {};

	bool run() override {
		if (blackboard->walking) {
			blackboard->motion->velocity.x = 0.f;
			blackboard->motion->velocity.y = -2.5f;
			blackboard->walking = false;
			return true;
		}
		return false;
	}
};

// Jump
class Jump : public Node {
public:
	Jump() {};

	bool run() override {
		blackboard->motion->velocity.y = -2.5 * 150.0;
		return true;
	}
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