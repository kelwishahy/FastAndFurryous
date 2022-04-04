#include <hpp/options_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/common.hpp>

// change option timer type
int decreaseTimer(Entity entity, float amount) {

	auto& timer_count = registry.timer.get(entity).timerC;
	timer_count = (timer_count - amount >= 20.f) ? timer_count - amount : 20.f;
	return timer_count;
}

int increaseTimer(Entity entity, float amount) {
	auto& timer_count = registry.timer.get(entity).timerC;
	timer_count = (timer_count < 50.f) ? timer_count + amount : 50.f;
	return timer_count;
}

int decreasePlayers(Entity entity, int amount) {
	auto& player_count = registry.players.get(entity).playersN;
	player_count = (player_count - amount >= 3) ? player_count - amount : 3;
	return player_count;
}

int increasePlayers(Entity entity, int amount) {
	auto& player_count = registry.players.get(entity).playersN;
	player_count = (player_count < 10) ? player_count + amount : 10;
	return player_count;
}