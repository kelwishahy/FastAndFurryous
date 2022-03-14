#pragma once
#include <hpp/tiny_ecs.hpp>
#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>


////////////////////////////////////////////////////////////////////////////////
// Component IDs
////////////////////////////////////////////////////////////////////////////////

//TODO add AI to the list of IDs
enum class SHADER_PROGRAM_IDS {
	ANIMATION,
	TEXTURE,
	WALL,
	AI,
	FONT,
	TOTAL
}; constexpr int shaderProgramCount = (int)SHADER_PROGRAM_IDS::TOTAL;

enum class TEXTURE_IDS {
	//Cat textures
	CAT_SIDE_IDLE,
	CAT_FRONT_IDLE,
	CAT_WALK,
	CAT_JUMP,
	CAT_SIDE_BLINK,
	CAT_FRONT_BLINK,
	CAT_HURT_FACE,
	CAT_HURT,
	CAT_DEAD,
	//
	STONE,
	BACKGROUND,
	START_MENU,
	//selectscreen
	SELECT_MENU,
	BUTTON1,
	BUTTONC,
	HOWTOMOVE,
	CAT_CROSSHAIR,
	DOG_CROSSHAIR,
	TOTAL
}; constexpr int textureCount = (int)TEXTURE_IDS::TOTAL;

enum class GEOMETRY_BUFFER_IDS {
	QUAD,
	TEXTURED_QUAD,
	WALL,
	AI,
	TOTAL
}; constexpr int geometryCount = (int)GEOMETRY_BUFFER_IDS::TOTAL;

////////////////////////////////////////////////////////////////////////////////
// Component Types
////////////////////////////////////////////////////////////////////////////////

enum RB_TYPES {
	NORMAL = 0,
	KINEMATIC = 1,
	STATIC = 2
};

enum WEAPON_TYPES {
	RIFLE = 0,
	SHOTGUN = 1
};

enum TILE_TYPES {
	NONE,
	STONE
};

enum MENU_TYPES {
	START,
	//selectscreen
	SELECT
};

enum TEAM {
	PLAYER_1_TEAM,
	PLAYER_2_TEAM,
	AI_TEAM,
	NPC_AI_TEAM
};

// Game components ------------------------------------------------------------

struct Player {
	TEAM team;
};

enum UI_ELEMENT {
	CROSSHAIR,
	HEALTH_DISPLAY,
	NAME
};

// Game components ------------------------------------------------------------
struct Background {
	
};

struct Tile {
	TILE_TYPES type;
	glm::vec2 position;
};

struct Health {
	int hp = 100;
};

struct AI {

};

struct SolidTerrain {

};

struct Collider {

};

struct Boxcollider : Collider {
	std::vector<glm::vec2> vertices;
	bool transformed_required = true;
	glm::vec2 deltaPos = glm::vec2{ 0,0 };
};

struct Circlecollider : Collider {
	float radius = 0.0f;
};

struct Motion {
	glm::vec2 position = { 0, 0 };
	float angle = 0;
	glm::vec2 velocity = { 0, 0 };
	glm::vec2 scale = { 10, 10 };
};

struct Rigidbody {
	enum RB_TYPES type = NORMAL;
	float mass = 1;
	float collision_depth;
	glm::vec2 collision_normal;
};

struct RayCast {
	int max_depth = 1;
};

struct WeaponBase {
	float MAX_ANGLE;
	float MIN_ANGLE;
	float aim_angle;
	float distance;
	float area;
	float aim_loc_x;
	float damage;
	WEAPON_TYPES type;
};

struct Rifle : WeaponBase {
	Rifle() { 
		//a little less than pi/2
		MAX_ANGLE = 1.2f;
		//a little more than 0
		MIN_ANGLE = 0.2f;
		// pi/4
		// aim_angle = 0.7854f;
		aim_angle = MIN_ANGLE;
		//distance the gun can shoot
		distance = 500.0f;
		//"radius" around distance
		area = 200.0f;
		damage = 10;
		type = RIFLE;
	}
};

struct Shotgun : WeaponBase {

};

struct Projectile {
	Entity origin;
	glm::vec4 trajectoryAx;
	glm::vec4 trajectoryAy;
	float delta_time = 0;
	float hit_radius;
	glm::vec2 end_tangent;
};

// Stucture to store collision information
struct Collision {
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

struct Clickable {
	std::vector<glm::vec2> vertecies;
	std::vector<std::function<void()>> callbacks;
};

struct AnchoredEntities {
	glm::vec2 normal_distance = {0.0f, 0.0f};
	Entity child;
};

struct UIElement {
	UI_ELEMENT element_type;
};

struct Cat {
	
};

struct Dog {
	
};

// Render components ----------------------------------------------------------

struct TEXTURE_ANIM_CONSTANTS {
	int NUM_FRAMES;
	float FRAME_TEXTURE_WIDTH;
	float TOTAL_FRAME_TIME;
};

// Single vertex vuffer element for textured sprites
struct TexturedVertex {
	glm::vec3 position;
	glm::vec2 texCoord;
};

// Single vertex buffer element for non-textured meshes
struct ColoredVertex {
	glm::vec3 position;
	glm::vec3 color;
};

// Index and vertex buffers
struct Mesh {
	static bool loadMeshFromObj(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, glm::vec2& out_size);
	glm::vec2 originalSize = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertexIndices;
};

// The texture, shader, and geometry to be rendered
// Any entity that will be rendered must have one of these components
struct RenderRequest {
	TEXTURE_IDS texture = TEXTURE_IDS::TOTAL;
	SHADER_PROGRAM_IDS shader = SHADER_PROGRAM_IDS::TOTAL;
	GEOMETRY_BUFFER_IDS geometry = GEOMETRY_BUFFER_IDS::TOTAL;
};

struct MenuItem {
	float layer;
	MENU_TYPES menu_id;
	RenderRequest request;
};

struct Animation {
	int frame = 0;
	float frame_counter_ms = 100;
	bool facingLeft = false;
	int curr_frame = 0;
	TEXTURE_IDS anim_state = TEXTURE_IDS::TOTAL;
	//map of TEXTUREID to animation constants for shaders
	std::unordered_map<TEXTURE_IDS, TEXTURE_ANIM_CONSTANTS> animation_states_constants;
};

struct AnimationExtra {
	std::string tag;   //This is scuffed but we can just name the extra anim for now
	Entity parent;
	glm::vec2 offset_from_parent;
};

struct Text {
	std::string text;
	glm::vec3 color;
};

struct Glyph {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   size;       // Size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

// Debug components ------------------------------------------------------------

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// A struct to refer to debugging graphics in the ECS
struct DebugComponent {
	// Note, an empty struct has size 1
};