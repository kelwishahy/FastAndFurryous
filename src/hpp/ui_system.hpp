#pragma once

#include "hpp/common.hpp"
#include "hpp/components.hpp"
#include "hpp/tiny_ecs_registry.hpp"
#include <utility>
#include <vector>

class UISystem
{

public:

	UISystem();
	~UISystem();

	void step(float elapsed_ms);

	void init();

	void show_crosshair(Entity e);
	void hide_crosshair();

private:
	//First entity is the actual UIElement, second entity is the character it is attached to

	Entity crosshair_marker;


};