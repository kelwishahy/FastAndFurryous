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
			default:
				break;
		}

	}
	for (Entity e : registry.healthboxes.entities) {

		Entity parent = registry.healthboxes.get(e).parent;

		if (!registry.health.has(parent)) {
			registry.remove_all_components_of(registry.healthboxes.get(e).text);
			registry.remove_all_components_of(e);
		}
		else {
			HealthBox healthbox = registry.healthboxes.get(e);
			Motion& box = registry.motions.get(e);
			Motion& text = registry.motions.get(healthbox.text);
			box.position = registry.motions.get(healthbox.parent).position + vec2(10.0f, -80.0f); //hard coded
			text.position = registry.motions.get(healthbox.parent).position + vec2(-9.0f, -98.0f); //hard coded

			Text& text_val = registry.texts.get(healthbox.text);
			text_val.text = std::to_string(registry.health.get(healthbox.parent).hp);
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