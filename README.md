# Fast and Furryous

## Milestone 1: Skeletal Game

### Rendering

  - Initialization of the sprite geometry (square shape):
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::initRenderData()`
    - Relevant Line Number(s): `193 - 216`
    
  - Loading in cat texture (`assets/textures/cat.png`):
    - File: `src/cpp/texture_manager.cpp`
    - Function:  `TextureManager::loadTextures()`
    - Relevant Line Number(s): `12 - 50`

  - Drawing a sprite with the cat texture:
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::draw()`
    - Relevant Line Number(s): `20 - 94`

  - 2D transformations of the rendered geometry:
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::draw()`
    - Relevant Line Number(s): `21, 74 - 88`<br><br>
    ##### Note: Smooth movement is handled in `PhysicsSystem::step()`, lines `35-43`
    
### Gameplay

### Creative Component
