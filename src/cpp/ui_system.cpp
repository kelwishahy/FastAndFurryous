#include "hpp/ui_system.hpp"

#include "hpp/tiny_ecs_registry.hpp"
#include "hpp/world_init.hpp"

UISystem::UISystem() {
	
}

UISystem::~UISystem() {
	
}

void UISystem::init(TextManager& textManager) {

	for (Entity e : registry.health.entities) {
		Health entity_health = registry.health.get(e);

		Entity health = createHealthCounter(e, entity_health.hp, textManager);

	}

}

void UISystem::step(float elapsed_ms) {

	for (Entity e : registry.uiElements.entities) {

		UIElement ui = registry.uiElements.get(e);

		switch (ui.element_type) {
			case UI_ELEMENT::CROSSHAIR: {
				for (int i = 0; i < registry.parentEntities.components.size(); i++) {
					Entity parent = registry.parentEntities.entities[i];
					ChildEntities& children = registry.parentEntities.components[i];
					for (int j = 0; j < children.child_data_map.size(); j++) {
						if (children.child_data_map.at(j) == e) {
							assert(registry.weapons.has(parent));
							Motion& child_motion = registry.motions.get(e);
							child_motion.angle = registry.weapons.get(parent).aim_angle;
						}
					}
				}
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
	Entity crosshair = createCrosshair(e, registry.characters.get(e)->animal == ANIMAL::CAT);

}

void UISystem::hide_crosshair(Entity e) {
	if (registry.parentEntities.has(e)) {
		ChildEntities& children = registry.parentEntities.get(e); //shoudl be a character entity i.e cat/dog
		for (int i = 0; i < children.child_data_map.size(); i++) {
			if (children.tags.at(i) == "crosshair") { //there should be a tag in the character for crosshair
				for (Entity e_ : registry.parentEntities.entities) {
					if (e_ == children.child_data_map.at(i)) {
						ChildEntities& crosshair_children = registry.parentEntities.get(e_); //the child of the child
						registry.remove_all_components_of(crosshair_children.child_data_map[0]); //erase the actual physical crosshair
						registry.remove_all_components_of(children.child_data_map.at(i)); //erase the crosshair child
						children.remove_child(children.child_data_map.at(i)); //update the ChildEntities
					}
				}
			}
		}
		//registry.remove_all_components_of(crosshair_marker);
	}

}