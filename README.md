# Fast and Furryous

## Milestone 1: Skeletal Game

### Rendering

  - Initialization of the sprite geometry (square shape):
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::initRenderData()`
    - Relevant Line Number(s): `244 - 282`
    
  - Loading in cat texture (`assets/textures/cat.png`):
    - File: `src/cpp/texture_manager.cpp`
    - Function:  `TextureManager::loadTextures()`
    - Relevant Line Number(s): `12 - 50`

  - Drawing a sprite with the cat texture:
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::draw()`
    - Relevant Line Number(s): `33 - 91`

  - 2D transformations of the rendered geometry:
    - File: `src/cpp/render_system.cpp`
    - Function:  `RenderSystem::draw()`
    - Relevant Line Number(s): `21, 73 - 87`<br><br>
    ##### Note: Smooth movement is handled in `PhysicsSystem::applyMotions()`, lines `227 - 254`
    
### Gameplay

  - Player cat initialization
    - File: `src/cpp/world_system.cpp`
    - Function: `WorldSystem::restart_game()`
    - Relavant Line Numbers: `80`
  
  - Wall initialization
    - File: `src/cpp/world_system.cpp`
    - Function: `WorldSystem::restart_game()`
    - Relavant Line Numbers: `84 - 94`

  - Box Collision algorithm, with inspiration from https://github.com/twobitcoder101/FlatPhysics
    - File: `physics_system.cpp`
    - Function: `phsyscs_system::box_collision()`
    - Relavant Line Numbers: `96 - 161`
  
  - Movement
    - File: `world_system.cpp`
    - Function: `WorldSystem::on_key`
    - Relavant Line Numbers: `127 - 180`
  
  - Collision resolution
    - File: `world_system.cpp`
    - Function: `WorldSystem::handle_collisions()`
    - Relavant Line Numbers: `101 - 118`
    
    
### Creative Component

  - AI initialization
    - File: `src/cpp/world_system.cpp`
    - Function: `WorldSystem::restart_game()`
    - Relavant Line Numbers: `81`

  - Gravity
    - File: `physics_system.cpp`
    - Function: `PhsysicsSystem::applyMotions()`
    - Relavant Line Numbers: `242 - 244`
