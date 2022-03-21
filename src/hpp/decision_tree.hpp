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
	bool walking;
	int turn;
	float timer;
	float lowerhp;
	ShootingSystem* shootingSystem;
	Entity* player;
	float move_delay;
	bool shot;
	vec2 prev_pos;
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

class IsHurt : public Node {
	bool run() override {
		return (blackboard->timer > 0 && blackboard->timer < 5.f);
	}
};

class HpLower : public Node {
	bool run() override {
		return blackboard->lowerhp;
	}
};

class Shot : public Node {
	bool run() override {
		return blackboard->shot;
	}

};

// TASKS
class Charge : public Node {
	bool run() override {
		Motion& player_motion = registry.motions.get(*blackboard->player);
		if (blackboard->motion->position.x < player_motion.position.x) {
			blackboard->motion->velocity.x = blackboard->velocity;
		}
		else {
			blackboard->motion->velocity.x = -blackboard->velocity;
		}
		
		return (blackboard->motion->velocity.x < 0 || blackboard->motion->velocity.x > 0);
	}
};

class RunAway : public Node {
	bool run() override {
		Motion& player_motion = registry.motions.get(*blackboard->player);
		printf("move away \n");

		if (blackboard->motion->position.x < player_motion.position.x) {
			blackboard->motion->velocity.x = -blackboard->velocity;
		}
		else {
			blackboard->motion->velocity.x = blackboard->velocity;
		}

		return (blackboard->motion->velocity.x < 0 || blackboard->motion->velocity.x > 0);
	}
};
class Move : public Node {
	bool run() override {
		Motion& player_motion = registry.motions.get(*blackboard->player);
		if (blackboard->shot) {
			blackboard->motion->velocity.x = 0;
			return true;
		}
		if (blackboard->lowerhp) {
			//Motion& player_motion = registry.motions.get(*blackboard->player);
			printf("move away \n");

			if (blackboard->motion->position.x < player_motion.position.x) {
				blackboard->motion->velocity.x = -blackboard->velocity;
			}
			else {
				blackboard->motion->velocity.x = blackboard->velocity;
			}

			return (blackboard->motion->velocity.x < 0 || blackboard->motion->velocity.x > 0);
		}
		else {
			printf("charge \n");

			if (blackboard->motion->position.x < player_motion.position.x) {
				blackboard->motion->velocity.x = blackboard->velocity;
			}
			else {
				blackboard->motion->velocity.x = -blackboard->velocity;
			}

			return (blackboard->motion->velocity.x < 0 || blackboard->motion->velocity.x > 0);
		}
	}

};

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
		if (blackboard->shot) {
			blackboard->motion->velocity.x = 0;
			blackboard->motion->velocity.y = -2.5;
			// for (int i = 0; i < 7; i++) {
			// 	blackboard->shootingSystem->aimDown(*blackboard->entity);
			// }
			blackboard->shootingSystem->shoot(*blackboard->entity);
			blackboard->shot = true;
			return true;
		}
		return false;
	}
};

class EndTurn : public Node {
	bool run() override {
		blackboard->motion->velocity.x = 0.f;
		blackboard->motion->velocity.y = 350.f;
		blackboard->turn = TURN_CODE::PLAYER1_TURN;
		blackboard->shot = false;
		return true;
	}
};