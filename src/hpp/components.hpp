#pragma once
#include <map>
#include <hpp/tiny_ecs.hpp>
#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

#include <hpp/States/character_grounded_state.hpp>

#include "particle_system.hpp"
#include "States/character_airborne_states.hpp"

////////////////////////////////////////////////////////////////////////////////
// Component IDs
////////////////////////////////////////////////////////////////////////////////

enum class SHADER_PROGRAM_IDS {
	ANIMATION,
	TEXTURE,
	WALL,
	AI,
	FONT,
	TILE,
	PARTICLE,
	TOTAL
}; constexpr int shaderProgramCount = (int)SHADER_PROGRAM_IDS::TOTAL;

enum class TEXTURE_IDS {
	//Cat textures
	CAT_SIDE_IDLE_AK,
	CAT_SIDE_IDLE_SG,
	CAT_SIDE_IDLE_GL,
	CAT_SIDE_IDLE_AWP,
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
	DOG_SIDE_IDLE_AK,
	DOG_SIDE_IDLE_SG,
	DOG_SIDE_IDLE_GL,
	DOG_SIDE_IDLE_AWP,
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
	BUTTON5,
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
	EXPLOSION,
	TOTAL
}; constexpr int textureCount = (int)TEXTURE_IDS::TOTAL;

enum class GEOMETRY_BUFFER_IDS {
	QUAD,
	TEXTURED_QUAD,
	WALL,
	FONT,
	CIRCLE,
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
	AWP = 2,
	LAUNCHER = 3,
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
	TUTORIAL,
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
// State Machine------------------------------------------------------------------

class CharacterStateMachine {

public:

	CharacterStateMachine() = default;
	~CharacterStateMachine() = default;

	void init(CharacterState* starting_state);


	CharacterState* getCurrentState() {
		return curr_state;
	}

	void changeState(CharacterState* new_state);
	bool isSelected() {
		return selected;
	}
	void selectChar() {
		selected = true;
	}
	void deselectChar() {
		selected = false;
	}

	bool isAI() {
		return isAi;
	}
	void setAI(bool isAI) {
		isAi = isAI;
	}

	bool isJumping() {
		return is_jumping;
	}
	void setJumping(bool isJumping) {
		is_jumping = isJumping;
	}

protected:
	CharacterState* curr_state;
	bool selected = false;
	bool isAi = false;
	bool is_jumping = false;

	void setCurrentState(CharacterState* state) {
		curr_state = state;
	}

};

// Game components ------------------------------------------------------------
struct Particle {
	bool active = false;
	float timer = 1000;
};

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

struct Timer {
	float time;
	float counter;
	void reset_counter() {
		time = counter;
	}
};

struct Explosion
{
	Entity origin;
	float damage = 37.0f;
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
	bool gravity_affected = true;
	glm::vec2 collision_normal;
	glm::vec2 force_accumulator;
	float inertia = 0;
};

struct RayCast {
	int max_depth = 1;
};

struct WeaponBase {
	float MAX_ANGLE;
	float MIN_ANGLE;
	float aim_angle;
	float min_damage;
	float max_damage;
	float max_dist;
	WEAPON_TYPES type;
};

struct Rifle : WeaponBase {
	Rifle() : WeaponBase() { 
		//a little less than pi/2
		MAX_ANGLE = 1.2f;
		//a little more than 0
		MIN_ANGLE = 5.5f;
		// pi/4
		// aim_angle = 0.7854f;
		aim_angle = 0.4f;
		max_damage = 45.f;
		min_damage = 35.f;
		max_dist = 1000.f;
		type = RIFLE;
	}
};

struct Awp : WeaponBase {
	Awp() : WeaponBase() {
		//a little less than pi/2
		MAX_ANGLE = 0.9f;
		//a little more than 0
		MIN_ANGLE = 5.5f;
		// pi/4
		// aim_angle = 0.7854f;
		aim_angle = 0.4f;
		max_damage = 80.f;
		min_damage = 70.f;
		max_dist = 3000.f;
		type = AWP;
	}
};

struct Shotgun : WeaponBase {
	Shotgun() : WeaponBase() {
		//a little less than pi/2
		MAX_ANGLE = 1.2f;
		//a little more than 0
		MIN_ANGLE = 5.5f;
		// pi/4
		// aim_angle = 0.7854f;
		aim_angle = 0.4f;
		max_damage = 6.f;
		min_damage = 2.f;
		max_dist = 400.0f;
		type = SHOTGUN;
	}
};

struct Launcher: WeaponBase {
	Launcher() : WeaponBase() {
		MAX_ANGLE = 1.2f;
		//a little more than 0
		MIN_ANGLE = 5.5f;
		// pi/4
		// aim_angle = 0.7854f;
		aim_angle = 0.4f;
		max_damage = 0.f;
		min_damage = 0.f;
		max_dist = 0.0f;
		type = LAUNCHER;
	}
};

struct Projectile {
	Entity origin;
};

struct Grenade {
	Entity origin;
	int bounces = 3;
	float bounce_cooldown = 100.0f;
	float COOLDOWN = 100.0f;
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
	// static bool loadMeshFromObj(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, glm::vec2& out_size);
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

// change option timer type
struct OptionTimer {
	float timerC = 1.f;
};

void remove_children(Entity e);
std::vector<Entity> get_all_children(Entity e);
void change_animation(Entity e, TEXTURE_IDS tex_id);
bool check_if_part_of_parent(Entity e, Entity child);
bool check_if_cat(Entity e);

struct Character {
	Entity character;
	CharacterStateMachine state_machine;
	CharacterIdleState* idle_state;
	CharacterMoveLeftState* move_left_state;
	CharacterMoveRightState* move_right_state;
	CharacterAimState* aim_state;
	CharacterShootingState* shooting_state;
	CharacterAirborneState* airborne_state;
	CharacterAirborneMoveLeftState* airborne_move_left;
	CharacterAirborneMoveRightState* airborne_move_right;
	CharacterDeadState* dead_state;
	CharacterDamageState* damage_state;
	glm::vec3 team_color;
	ANIMAL animal;
	bool isDead = false;
	ParticleSystem* particleSystem;

	void init(ParticleSystem* particleSystem) {
		idle_state = new CharacterIdleState(character);
		move_left_state = new CharacterMoveLeftState(character);
		move_right_state = new CharacterMoveRightState(character);
		aim_state = new CharacterAimState(character);
		shooting_state = new CharacterShootingState(character);
		airborne_move_left = new CharacterAirborneMoveLeftState(character);
		airborne_move_right = new CharacterAirborneMoveRightState(character);
		airborne_state = new CharacterAirborneState(character);
		dead_state = new CharacterDeadState(character);
		damage_state = new CharacterDamageState(character);

		state_machine = CharacterStateMachine();
		state_machine.init(idle_state);
		this->particleSystem = particleSystem;
	}

	virtual void animate_walk() = 0;
	virtual void animate_idle() = 0;
	virtual void animate_jump() = 0;
	virtual void animate_hurt() = 0;
	virtual void animate_dead() = 0;
	virtual void animate_aim() = 0;

	virtual void play_hurt_sfx() = 0;
};

struct Cat : Character {
	Cat() : Character() {
		team_color = glm::vec3{ 0.862f, 0.525f, 0.517f };
		animal = ANIMAL::CAT;
	}
	void animate_walk() override;
	void animate_idle() override;
	void animate_jump() override;
	void animate_hurt() override;
	void animate_dead() override;
	void animate_aim() override;

	void play_hurt_sfx() override;
};

struct Dog : Character {
	Dog() : Character() {
		team_color = glm::vec3{ 0.039, 0.454, 1 };
		animal = ANIMAL::DOG;
	}
	void animate_walk() override;
	void animate_idle() override;
	void animate_jump() override;
	void animate_hurt() override;
	void animate_dead() override;
	void animate_aim() override;

	void play_hurt_sfx() override;
};