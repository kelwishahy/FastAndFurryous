#pragma once
#include <map>
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
	CAT_FRONT_ARM,
	CAT_BACK_ARM,
	CAT_HURT_FACE,
	CAT_HURT,
	CAT_DEAD,
	// Dog Textures
	DOG_SIDE_IDLE,
	DOG_FRONT_BLINK,
	DOG_SIDE_BLINK,
	DOG_FRONT_IDLE,
	DOG_FRONT_ARM,
	DOG_BACK_ARM,
	DOG_WALK,
	DOG_JUMP,
	DOG_HURT,
	DOG_HURT_FACE,
	DOG_DEAD,
	//
	//Gun Sprites
	RIFLE,
	AK_47,
	AWP,
	GRENADE_LAUNCHER,
	STONE,
	// All Screens
	BACKGROUND,
	START_MENU,
	SELECT_MENU,
	OPTIONS_MENU,
	LEVELS_MENU,
	// Start Menu Buttons
	BUTTON1,
	BUTTON2,
	BUTTON3,
	BUTTON4,
	// Select Menu Buttons
	BUTTONC,
	BUTTOND,
	// Tutorial Menu 
	HOWTOMOVE,
	// Option Menu Buttons
	BUTTONRA,
	BUTTONLA,
	BUTTONRB,
	BUTTONLB,
	// Level Menu Buttons
	BUTTONL1,
	BUTTONL2,
	BUTTONL3,
	CAT_CROSSHAIR,
	DOG_CROSSHAIR,
	HEALTH_SQUARE,
	BUTTONGAME,
	BUTTONCANCEL,
	/* Maps */

	// Industrial
	INDUSTRIAL_BG,
	INDUSTRIAL_FAR_BUILDINGS,
	INDUSTRIAL_BUILDINGS,
	INDUSTRIAL_FOREGROUND,

	// Night
	NIGHT1,
	NIGHT2,

	// Cyberpunk
	CYBERPUNK1,
	CYBERPUNK2,
	CYBERPUNK3,

	// MIAMI
	MIAMI1,
	MIAMI2,
	MIAMI3,
	MIAMI4,

	FOREST,
	SPACE,
	TOTAL
}; constexpr int textureCount = (int)TEXTURE_IDS::TOTAL;

enum class GEOMETRY_BUFFER_IDS {
	QUAD,
	TEXTURED_QUAD,
	WALL,
	FONT,
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
	SHOTGUN = 1,
	TOTAL //For no weaponed 
}; constexpr int weaponCount = (int)WEAPON_TYPES::TOTAL;

enum TILE_TYPES {
	NONE,
	STONE
};

enum MENU_TYPES {
	START,
	SELECT,
	OPTIONS,
	LEVELS
};

enum TEAM {
	PLAYER_1_TEAM,
	PLAYER_2_TEAM,
	AI_TEAM,
	NPC_AI_TEAM
};

enum UI_ELEMENT {
	CROSSHAIR,
	HEALTH_DISPLAY,
	NAME
};

enum class MAPS {
	INDUSTRIAL,
	FOREST,
	SPACE,
	NIGHT,
	CITY,
	CYBERPUNK,
	MIAMI,
	TOTAL
}; constexpr int mapCount = (int)MAPS::TOTAL;

constexpr float LAYERS[4] = { -0.9f, -0.8f, -0.7f, -0.6f };

enum class IndustrialBackground {
	INDUSTRIAL_BG,
	INDUSTRIAL_FAR_BUILDINGS,
	INDUSTRIAL_BUILDINGS,
	INDUSTRIAL_FOREGROUND,
	TOTAL
}; constexpr int IndustrialBackgroundLayers = (int)IndustrialBackground::TOTAL;

enum class NightBackground {
	NIGHT1,
	NIGHT2,
	TOTAL
}; constexpr int NightBackgroundLayers = (int)NightBackground::TOTAL;

enum class CyberpunkBackground {
	CYBERPUNK1,
	CYBERPUNK2,
	CYBERPUNK3,
	TOTAL
}; constexpr int CyberpunkBackgroundLayers = (int)CyberpunkBackground::TOTAL;

enum class MiamiBackground {
	MIAMI1,
	MIAMI2,
	MIAMI3,
	MIAMI4,
	TOTAL
}; constexpr int MiamiBackgroundLayers = (int)MiamiBackground::TOTAL;

enum class ANIMAL {
	CAT,
	DOG
};
// Game components ------------------------------------------------------------
struct Background {
	float layer = -0.5;
};

struct Tile {
	TILE_TYPES type;
	glm::vec2 position;
};

struct Selected {
	bool isSelected = true;
};

struct Health {
	int hp = 100;
	float damageTimer = 800;
	bool hurt = false;
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
	glm::vec2 force_accumulator;
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
	//float aim_loc_x;
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
	/*glm::vec4 trajectoryAx;
	glm::vec4 trajectoryAy;
	float delta_time = 0;*/
	//float hit_radius;
	//glm::vec2 end_tangent;
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

struct ChildEntities {
	std::map<int, Entity> child_data_map; //unfortunately putting Entity first causes an error
	std::unordered_map<int, glm::vec2> normal_dists;
	std::unordered_map<int, std::string> tags; //optional
	std::unordered_map<int, glm::vec2>  original_dists; //opational, this me hackig stuff in
	void remove_child(Entity e) {
		for (int i = 0; i < child_data_map.size(); i++) {
			if (child_data_map.at(i) == e) {
				normal_dists.erase(i);
				tags.erase(i);
				original_dists.erase(i);
				child_data_map.erase(i);
				i--;
			}
		}
	}
};
void remove_children(Entity e);

struct UIElement {
	UI_ELEMENT element_type;
};

struct Cat {

};

struct Dog {

};

struct HealthBox {
	Entity parent;
	Entity text;
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
	std::string name;
	int frame = 0;
	float frame_counter_ms = 100;
	bool facingLeft = false;
	int curr_frame = 0;
	TEXTURE_IDS anim_state = TEXTURE_IDS::TOTAL;
	//map of TEXTUREID to animation constants for shaders
	std::unordered_map<TEXTURE_IDS, TEXTURE_ANIM_CONSTANTS> animation_states_constants;
};


struct Text {
	std::string text;
	glm::vec3 color;
	glm::vec2 scale;
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

struct OptionTimer {
	int timerC;
};

struct OptionPlayers {
	int playersN;
};

void remove_children(Entity e);
std::vector<Entity> get_all_children(Entity e);

