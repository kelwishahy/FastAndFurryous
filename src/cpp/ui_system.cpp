#include "hpp/ui_system.hpp"
#include "hpp/world_init.hpp"
#include <hpp/Game_Mechanics/shooting_system.hpp>

UISystem::UISystem() {
	
}

UISystem::~UISystem() {
	
}

void UISystem::init() {

	for (Entity e : registry.health.entities) {
		Health entity_health = registry.health.get(e);

		Entity health = createHealthCounter(e, entity_health.hp);

	}

}

void UISystem::step(float elapsed_ms) {

	for (Entity e : registry.uiElements.entities) {

		UIElement ui = registry.uiElements.get(e);

		switch (ui.element_type) {
			case UI_ELEMENT::CROSSHAIR: {
			AnchoredEntities& anchor = registry.anchors.get(e);
			assert(registry.weapons.has(anchor.parent));
			Motion& anchor_motion = registry.motions.get(e);
			anchor_motion.angle = registry.weapons.get(anchor.parent).aim_angle;
			break;
			}
		case UI_ELEMENT::HEALTH_DISPLAY: {

			}
			default:
				break;
		}

	}


}

void UISystem::show_crosshair(Entity e) {

	Motion entity_motion = registry.motions.get(e);
	Entity crosshair = createCrosshair(e, registry.cats.has(e));
	crosshair_marker = crosshair;

}

void UISystem::hide_crosshair() {
	if (registry.anchors.has(crosshair_marker)) {
		AnchoredEntities& anchor = registry.anchors.get(crosshair_marker);
		registry.remove_all_components_of(anchor.child);
		registry.remove_all_components_of(crosshair_marker);
	}

}