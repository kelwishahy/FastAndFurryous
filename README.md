# Fast and Furryous

## Milestone 4: Final Game

### Playability
- Tutorial level
  - File: `src/cpp/world_system.cpp`
  - Function: `WorldSystem::tutorial_level()`
  - Relavant Line Numbers: `515 - 525`

- Implemented different weapons
  - File: `src/cpp/shooting_system.cpp`
  - Function: `ShootingSystem::shoot()`
  - Relavant Line Numbers: `111 - 149`

- Crosshair angle indicators:
  - File: `src/cpp/world_init.cpp`
  - Function: `createCrosshair()`
  - Relavant Line Numbers: `450 - 494`

### Robustness
The game was profiled using [Tracy](https://github.com/wolfpld/tracy#tracy-profiler), and optimization was done in multiple bottleneck locations.
The following functions were optimized for better performance and memory management:
- `RenderSystem::drawText()` in `src/cpp/render_system.cpp`
- `GameController::step()` in `src/cpp/game_controller.cpp`
- `RenderSystem::transform()` in `src/cpp/render_system.cpp`
- `createText()` in `src/cpp/world_init.cpp`
- `TextManager::getGlyphs()`, `TextManager::getItalicGlyphs()`, and `TextManager::getBoldGlyphs()` in `src/hpp/text_manager.hpp`

### Stability
- Consistent resolution
  - File: `src/cpp/common.cpp`
  - Function: `scaleToScreenResolution()`
  - Relavant Line Numbers: `88 - 92`
    
### Creative Component

- Added bouncing physics
  - File: `src/cpp/character_grounded_state.cpp`
  - Function: `handleGrenadeBounce()`
  - Relavant Line Numbers: `516 - 550`

- Added Explosion with effects
  - File: `src/cpp/world_init.cpp`
  - Function: `createExplosion()`
  - Relavant Line Numbers: `592 - 624`


- Camera that is controlled with mouse input
  - File: `src/cpp/game_controller.cpp`
  - Function: `GameController::moveCamera()`
  - Relavant Line Numbers: `148 - 170`

- Parallax scrolling backgrounds
  - File: `src/cpp/map.cpp`
  - Function: `MapSystem::Map::build()`
  - Relavant Line Numbers: `65 - 181`

- Characters sprites are faded when not selected
  - File: `src/cpp/render_system.cpp`
  - Function: `RenderSystem::animateSprite()`
  - Relavant Line Numbers: `169 - 174, 204`


