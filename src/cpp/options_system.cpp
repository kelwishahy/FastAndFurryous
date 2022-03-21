#include <hpp/options_system.hpp>
#include <hpp/tiny_ecs_registry.hpp>
#include <hpp/common.hpp>
//startscreen16
int decreaseTimer(Entity entity, int amount) {

	auto& timer_count = registry.timer.get(entity).timerC;
	timer_count = (timer_count - amount >= 20) ? timer_count - amount : 20;
	return timer_count;
}
//startscreen16
int increaseTimer(Entity entity, int amount) {
	auto& timer_count = registry.timer.get(entity).timerC;
	timer_count = (timer_count < 50) ? timer_count + amount : 50;
	return timer_count;
}
//startscreen17
int decreasePlayers(Entity entity, int amount) {
	auto& player_count = registry.players.get(entity).playersN;
	player_count = (player_count - amount >= 3) ? player_count - amount : 3;
	return player_count;
}
//startscreen17
int increasePlayers(Entity entity, int amount) {
	auto& player_count = registry.players.get(entity).playersN;
	player_count = (player_count < 10) ? player_count + amount : 10;
	return player_count;
}