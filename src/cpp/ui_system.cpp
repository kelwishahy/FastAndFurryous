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
		std::pair<Entity, Entity> pair = { health, e };
		health_map.push_back(pair);
	}

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
		case UI_ELEMENT::HEALTH_DISPLAY: {
			//if ur gonna do something specific to health do it here
			}
			default:
				break;
		}

	}

	//EVERY update loop update all UI health values
	for (int i = 0; i < health_map.size(); i++) {
		//Check if character died
		std::pair<Entity, Entity> pair = health_map[i];
		Entity child = registry.anchors.get(pair.first).child;
		if (!registry.health.has(pair.second)) {
			registry.remove_all_components_of(child);
			registry.remove_all_components_of(pair.first);
			health_map.erase(health_map.begin()+i);
			continue;
		}


		Health health = registry.health.get(pair.second);
		assert(registry.texts.has(registry.anchors.get(pair.first).child));
		Text& text = registry.texts.get(registry.anchors.get(pair.first).child);
		text.text = std::to_string(health.hp);

		Motion& health_bar = registry.motions.get(pair.first);
		assert(registry.motions.has(pair.second));
		health_bar.position = registry.motions.get(pair.second).position + vec2{0.0f, -100.0f};
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