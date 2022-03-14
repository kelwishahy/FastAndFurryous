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
				WeaponBase weapon = registry.weapons.get(crosshair_marker.second);
				Motion& crosshair_motion = registry.motions.get(crosshair_marker.first);
				printf("aim angle is: %f\n", weapon.aim_angle);
				crosshair_motion.position = { ShootingSystem::calculate_point(weapon.curr_trajectory_x, 0.35f), ShootingSystem::calculate_point(weapon.curr_trajectory_y, 0.35f) };
				break;
			}
			default:
				break;
		}

	}

}

void UISystem::show_crosshair(Entity e) {

	Motion entity_motion = registry.motions.get(e);
	Entity crosshair = createCrosshair(registry.cats.has(e));

	crosshair_marker.first = crosshair;
	crosshair_marker.second = e;

}

void UISystem::hide_crosshair() {

	registry.remove_all_components_of(crosshair_marker.first);
	crosshair_marker.first = Entity();
	crosshair_marker.second = Entity();

}