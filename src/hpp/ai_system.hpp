#pragma once

#include <vector>
#include <random>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <hpp/Game_Mechanics/shooting_system.hpp>
#include <glm/vec2.hpp>
#include <hpp/game_controller.hpp>

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
		//void addChildren(const vec& new_children) { for (Node* child : newChildren) addChild(child); }
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


struct GameState {
	float timer = 0;
	uint turn_number = 0;
	uint turn_possesion;
};

enum class AI_MODE {
	MOVING,
	SHOOTING,
	IDLE
};

class AISystem
{
public:
	AI_MODE ai_mode;
	uint turn_possesion;

	void step(float elapsed_time);
	void init();
	void updateGameState(uint new_turn);
private:
	//float jumpdelay;
	float timer = 4000.f;
	//float timer_shoot = 0.f;


	BehaviorTree::Selector root; // Sequence of 2 actions. 
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
	AIMove(glm::vec2 player_position, Entity ai) : player_position(player_position), ai(ai) {};

	virtual bool run() override {
		Motion& ai_motion = registry.motions.get(ai);

		ai_motion.angle = atan2(ai_motion.position.x - player_position.x, ai_motion.position.y - player_position.y);
		if (abs(ai_motion.position.x - player_position.x) > 200) {
			ai_motion.velocity.x = 100 * -sin(ai_motion.angle);
		}
		else {
			ai_motion.velocity.x = 0;
		}

		return true;
	}
};

class AIRight : public BehaviorTree::Node
{
private:
	glm::vec2 player_position;
	Entity ai;
	AI_MODE ai_mode;
public:
	AIRight(glm::vec2 player_position, Entity ai, AI_MODE ai_mode) : player_position(player_position), ai(ai), ai_mode(ai_mode) {};


	virtual bool run() override {
		Motion& ai_motion = registry.motions.get(ai);

		if (ai_mode == AI_MODE::MOVING) {
			if (ai_motion.position.x - player_position.x < 0) {
				ai_motion.velocity.x = 100;
				return true;
			}
		}
		
		return false;
	}
};

class AILeft : public BehaviorTree::Node
{
private:
	glm::vec2 player_position;
	Entity ai;
	AI_MODE ai_mode;

public:
	AILeft(glm::vec2 player_position, Entity ai, AI_MODE ai_mode) : player_position(player_position), ai(ai), ai_mode(ai_mode) {};

	virtual bool run() override {
		Motion& ai_motion = registry.motions.get(ai);

		if (ai_mode == AI_MODE::MOVING) {
			if (ai_motion.position.x - player_position.x < 0) {
				ai_motion.velocity.x = -100;
				return true;
			}
		}

		return false;
	}
};

class AIShoot : public BehaviorTree::Node 
{
private: 
	ShootingSystem shootingSystem;
	Entity ai;
	AISystem ai_system;

public:
	AIShoot(ShootingSystem shootingSystem, Entity ai) : shootingSystem(shootingSystem), ai(ai) {};

	virtual bool run() override {
		if (ai_system.ai_mode == AI_MODE::SHOOTING) {
			shootingSystem.shoot(ai);

			return true;
		}
		return false;
	}
};

//class AITurn : public BehaviorTree::Node
//{
//private:
//	uint game_turn;
//public:
//	AITurn(uint turn) : game_turn(turn) {};
//
//	virtual bool run() override {
//		if (game_turn == GameController::getTurnCode()) {
//			return true;
//		}
//		return false;
//	}
//};