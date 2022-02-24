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

	class CompositeNode : public Node {  //  This type of Node follows the Composite Pattern, containing a list of other Nodes.
	private:
		std::vector<Node*> children;
	public:
		const std::vector<Node*>& getChildren() const { return children; }
		void addChild(Node* child) { children.emplace_back(child); }
		void addChildren(std::initializer_list<Node*>&& newChildren) { for (Node* child : newChildren) addChild(child); }
		template <typename CONTAINER>
		void addChildren(const CONTAINER& newChildren) { for (Node* child : newChildren) addChild(child); }
	protected:
		std::vector<Node*> childrenShuffled() const { std::vector<Node*> temp = children;  std::random_shuffle(temp.begin(), temp.end());  return temp; }
	};

	class Selector : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Selector implementation
				if (child->run())  // If one child succeeds, the entire operation run() succeeds.  Failure only results if all children fail.
					return true;
			}
			return false;  // All children failed so the entire run() operation fails.
		}
	};

	class RandomSelector : public CompositeNode {  // RandomSelector operates as a Selector, but in a random order instead of from first child to last child.
	public:
		virtual bool run() override {
			for (Node* child : childrenShuffled()) {  // The order is shuffled
				if (child->run())
					return true;
			}
			return false;
		}
	};

	class Sequence : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Sequence implementation.
				if (!child->run())  // If one child fails, then enter operation run() fails.  Success only results if all children succeed.
					return false;
			}
			return true;  // All children suceeded, so the entire run() operation succeeds.
		}
	};

	class Root : public Node {
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
	float jumpdelay;
	float timer;
	BehaviorTree::Sequence sequence[2];
	int MIN_DISTANCE;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
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
