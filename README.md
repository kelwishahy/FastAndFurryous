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

  - Player cat initialization
    - File: `src/cpp/world_system.cpp`
    - Function: `World_init::createCat`
    - Relavant Line Numbers: `84`
  
  - Wall initialization
    - File: `src/cpp/world_system.cpp`
    - Function: `World_init::createWall`
    - Relavant Line Numbers: `85 - 86`
  
  - AI initialization

  - Box Collision algorithm, with inspiration from https://github.com/twobitcoder101/FlatPhysics
    - File: `physics_system.cpp`
    - Function: `phsyscs_system::box_collision`
    - Relavant Line Numbers: `96 - 161`
  
  - Movement
    - File: `world_system.cpp`
    - Function: `world_system::on_key`
    - Relavant Line Numbers: `133 - 185`

  - Gravity
    - File: `physics_system.cpp`
    - Function: `PhsysicsSystem::applyMotions`
    - Relavant Line Numbers: `221 - 248`
  
  - Collision resolution
    - File: `world_system.cpp`
    - Function: `world_system::handle_collisions`
    - Relavant Line Numbers: `94 - 123`
    
    
### Creative Component
