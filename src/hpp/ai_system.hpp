#pragma once

#include <vector>
#include <random>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <glm/vec2.hpp>

class BehaviorTree {
public:
	class Node { 
	public:
		virtual bool run() = 0;
	};

	class CompositeNode : public Node {  //  Composite Node in a Behavior Tree
	private:
		std::vector<Node*> children; 
	public:
		const std::vector<Node*>& getChildren() const { return children; }
		void addChild(Node* child) { children.emplace_back(child); }
		void clear() { children.clear(); }
	};

	class Selector : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // Selector Node. One of the children needs to succeed.
				if (child->run())  
					return true;
			}
			return false;  // All children failed --> fails
		}
	};

	class Sequence : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // Sequence Node. Runs children in a sequence. Fails if one of them fails.
				if (!child->run())  
					return false;
			}
			return true;  // All children succeeds
		}
	};

	class Root : public Node { // Root node. So far, our decision tree root node is just a sequence node with 2 actions.
	public:
		Node* child;
		friend class BehaviourTree;
		void setChild(Node* newChild) { child = newChild; }
		virtual bool run() override { return child->run(); }
	};
private:
	Root* root;
public:
	BehaviorTree() : root(new Root) {}
	void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
	bool run() const { return root->run(); }
};


class AISystem
{
public:
	void step(float elapsed_ms);
	void init();

private:
	//float jumpdelay;
	//float timer;
	BehaviorTree::Sequence sequence[2]; // Sequence of 2 actions. 
	//int MIN_DISTANCE;
	//std::default_random_engine rng;
	//std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};

class AIMove : public BehaviorTree::Node
{
private:
	glm::vec2 player_position;
	Entity ai;
public:
	AIMove(glm::vec2 player_position, Entity ai) : player_position(player_position), ai(ai) {
	};
	virtual bool run() override {
		Motion& ai_motion = registry.motions.get(ai);

		ai_motion.angle = atan2(ai_motion.position.x - player_position.x, ai_motion.position.y - player_position.y);
		printf("starting ai movement\n");
		if (abs(ai_motion.position.x - player_position.x) > 200) {
			ai_motion.velocity.x = 100 * -sin(ai_motion.angle);
		}
		else {
			ai_motion.velocity.x = 0;
		}

		return true;
	}
};
