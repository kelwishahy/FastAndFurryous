#include "hpp/ui_system.hpp"
#include "hpp/world_init.hpp"
#include <hpp/Game_Mechanics/shooting_system.hpp>

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
	Entity crosshair = createCrosshair(e, registry.cats.has(e));

	crosshair_marker.first = crosshair;
	crosshair_marker.second = e;

	crosshair_obj.first = crosshair;
	crosshair_obj.second = registry.anchors.get(crosshair).child;

}

void UISystem::hide_crosshair() {

	registry.remove_all_components_of(crosshair_obj.second);
	registry.remove_all_components_of(crosshair_marker.first);


}