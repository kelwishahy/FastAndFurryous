#pragma once
#include "text_manager.hpp"

class UISystem
{

public:

	UISystem();
	~UISystem();

	void step(float elapsed_ms);

	void init(TextManager& textManager);

	static void show_crosshair(Entity e);
	static void hide_crosshair(Entity e);

private:
	//First entity is the actual UIElement, second entity is the character it is attached to

	Entity crosshair_marker;


};
