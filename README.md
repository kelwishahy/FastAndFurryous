# Fast and Furryous

## Milestone 2: Minimally Playable Game

### Improved Gameplay
- Game logic to user input
  - File: `src/cpp/ai_system.cpp`
  - Function: `AISystem::init()`
  - Relavant Line Numbers: `34 - 81`
- Sprite sheet animation
  - File: `src/cpp/render_system.cpp`
  - Function: `RenderSystem::draw()`
  - Relavant Line Numbers: `66 - 69`
- New integrated assets (audio)
  - File: `src/cpp/world_system.cpp`
  - Function: `WorldSystem::init()`
  - Relavant Line Numbers: `61 - 64`
- New integrated assets (textures)
  - File: `src/hpp/render_system.hpp`
  - Functions: `RenderSystem::drawTiles()`, `RenderSystem::drawBackground()`
  - Relavant Line Numbers: `31 - 43, 88, 91`
- Basic user tutorial/help
  - File: `src/cpp/world_init.cpp`
  - Functions: `createMenu()`, `createButton()`
  - Relavant Line Numbers: `160 - 221`

### Playability
- Gameplay mechanics
  - File: `src/cpp/game_controller.cpp`
  - Function: `GameController::step()`
  - Relavant Line Numbers: `54 - 117`

### Stability
- Consistent game resolution
  - File: `src/cpp/render_system.cpp`
  - Function: `RenderSystem::init()`
  - Relavant Line Numbers: `397 - 405`
    
### Creative Component
- Background music and sound effects
  - File: `src/cpp/world_system.cpp`
  - Function: `WorldSystem::Init()`
  - Relavant Line Numbers: `50 - 73`
- Complex projectile motion
  - File: `src/cpp/Game_Mechanics/shooting_system.cpp`
  - Function: `ShootingSystem::step()`
  - Relavant Line Numbers: `36 - 39`