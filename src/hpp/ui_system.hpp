#pragma once
#include "text_manager.hpp"

class UISystem
{

public:

	UISystem();
	~UISystem();

	void step(float elapsed_ms);

	void init(TextManager& textManager);

	void show_crosshair(Entity e);
	void hide_crosshair();

private:
	//First entity is the actual UIElement, second entity is the character it is attached to

	Entity crosshair_marker;


};
