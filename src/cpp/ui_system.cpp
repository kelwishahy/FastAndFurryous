#include "hpp/ui_system.hpp"
#include "hpp/world_init.hpp"

UISystem::UISystem() {
	
}

UISystem::~UISystem() {
	
}

void UISystem::step(float elapsed_ms) {

	for (Entity e : registry.uiElements.entities) {

		UIElement ui = registry.uiElements.get(e);

		switch (ui.element_type) {
			case UI_ELEMENT::CROSSHAIR: {
				assert(registry.weapons.has(crosshair_marker.second));
				Motion& anchor_motion = registry.motions.get(crosshair_marker.first);
				anchor_motion.angle = registry.weapons.get(crosshair_marker.second).aim_angle;
				break;
			}
			default:
				break;
		}

	}

}

void UISystem::show_crosshair(Entity e) {

	Motion entity_motion = registry.motions.get(e);
	Entity crosshair = createCrosshair(entity_motion.position, registry.cats.has(e));

	crosshair_marker.first = crosshair;
	crosshair_marker.second = e;

}

void UISystem::hide_crosshair() {

	registry.remove_all_components_of(crosshair_marker.first);
	crosshair_marker.first = Entity();
	crosshair_marker.second = Entity();

}