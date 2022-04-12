#pragma once

#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "Game_Mechanics/shooting_system.hpp"

enum TURN_CODE {
	PLAYER1,
	PLAYER2,
	AI_TURN,
	NPCAI,
	END
};


struct Blackboard {
	Character* c;
	Entity selected_ai;
	float timer;
	int turn;
	int ai_index;
	bool done_move;
	bool left;
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
	void clear() { children.clear(); }

	void traverse() {
		if (run()) {
			if (conditionIsTrue != nullptr) {
				conditionIsTrue->traverse();
			}
		}
		else {
			if (conditionIsFalse != nullptr) {
				conditionIsFalse->traverse();
			}
		}
	}

	void addFalseConditionNode(Node* node) { conditionIsFalse = node; }
	void addTrueConditionNode(Node* node) { conditionIsTrue = node; }

	Node* conditionIsFalse = nullptr;
	Node* conditionIsTrue = nullptr;

private:
	std::list<Node*> children;
};


// DECISIONS
class IsAITurn : public Node {
	bool run() override {
		return (blackboard->turn == TURN_CODE::NPCAI);
	}
};

class DidTimeEnd : public Node {
	bool run() override {
		return blackboard->done_move;
	}
};

class LeftorRight : public Node {
	bool run() override {
		return blackboard->left;
	}

};

// TASKS


class MoveLeft : public Node {
	bool run() override {
		blackboard->c->state_machine.changeState(blackboard->c->move_left_state);
		return true;
	}
};

class MoveRight : public Node {
	bool run() override {
		blackboard->c->state_machine.changeState(blackboard->c->move_right_state);
		return true;
	}
};


class EndTurn : public Node {
	bool run() override {
		blackboard->c->state_machine.changeState(blackboard->c->idle_state);
		return true;
	}
};

class Shoot : public Node {
	bool run() override {
		//blackboard->c->state_machine.changeState(blackboard->c->shooting_state);
		ShootingSystem::shoot(blackboard->selected_ai);
		blackboard->c->state_machine.getCurrentState()->next_turn = true;
		blackboard->ai_index++;

		return true;
	}
};
