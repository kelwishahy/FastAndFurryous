#pragma once

#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "Game_Mechanics/shooting_system.hpp"

enum TURN_CODE {
	PLAYER1_TURN,
	PLAYER2_TURN,
	AI_TURN,
	NPCAI_TURN,
	END
};

struct Blackboard {
	Entity* entity;
	Motion* motion;
	float velocity;
	int turn;
	float timer;
	ShootingSystem* shootingSystem;
};

static Blackboard* blackboard;

/*
 * NODE TYPES
 */

// Generic node
class Node {
public:
	Node() {};

	~Node() {};

	virtual bool run() = 0;

	// Traverse the decision tree
	void traverse() {
		if (run()) {
			if (conditionIsTrue != nullptr) {
				conditionIsTrue->traverse();
			}
		} else {
			if (conditionIsFalse != nullptr) {
				conditionIsFalse->traverse();
			}
		}
	}

	void addFalseConditionNode(Node* node) { conditionIsFalse = node; }
	void addTrueConditionNode(Node* node) { conditionIsTrue = node; }

	Node* conditionIsFalse = nullptr;
	Node* conditionIsTrue = nullptr;
};

// DECISIONS
class IsAITurn : public Node {
	bool run() override {
		return (blackboard->turn == TURN_CODE::NPCAI_TURN);
	}
};

class DidTimeEnd : public Node {
	bool run() override {
		return (blackboard->timer <= 0);
	}
};

class IsMoving : public Node {
	bool run() override {
		return (blackboard->motion->velocity.x != 0);
	}
};

class IsMovingLeft : public Node {
	bool run() override {
		return (blackboard->motion->velocity.x < 0);
	}
};

class IsMovingRight : public Node {
	bool run() override {
		return (blackboard->motion->velocity.x > 0);
	}
};

class IsNearEnemy : public Node {
	bool run() override {
		return false;
	}
};

class IsHurt : public Node {
	bool run() override {
		return (registry.health.get(*blackboard->entity).hp < 100.f && blackboard->timer > 0 && blackboard->timer < 5.f);
	}
};

// TASKS
class MoveLeft : public Node {
	bool run() override {
		blackboard->motion->velocity.x = -blackboard->velocity;
		return (blackboard->motion->velocity.x < 0);
	}
};

class MoveRight : public Node {
	bool run() override {
		blackboard->motion->velocity.x = blackboard->velocity;
		return (blackboard->motion->velocity.x > 0);
	}
};

class SwitchDirection : public Node {
	bool run() override {
		blackboard->motion->velocity.x *= -1.f;
		return (blackboard->motion->velocity.x != 0);
	}
};

class Shoot : public Node {
	bool run() override {
		blackboard->motion->velocity.x = 0;
		blackboard->motion->velocity.y = -2.5;
		for (int i = 0; i < 7; i++) {
			blackboard->shootingSystem->aimDown(*blackboard->entity);
		}
		blackboard->shootingSystem->shoot(*blackboard->entity);
		return true;
	}
};

class EndTurn : public Node {
	bool run() override {
		blackboard->motion->velocity.x = 0.f;
		blackboard->motion->velocity.y = 350.f;
		return true;
	}
};
