#pragma once
#include <hpp/tiny_ecs.hpp>
#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Component IDs
////////////////////////////////////////////////////////////////////////////////

//TODO add AI to the list of IDs
enum class SHADER_PROGRAM_IDS {
	CAT,
	TRIANGLE,
	WALL,
	AI,
	TOTAL
}; constexpr int shaderProgramCount = (int)SHADER_PROGRAM_IDS::TOTAL;

enum class TEXTURE_IDS {
	CAT,
	TRIANGLE,
	TOTAL
}; constexpr int textureCount = (int)TEXTURE_IDS::TOTAL;

enum class GEOMETRY_BUFFER_IDS {
	CAT,
	TRIANGLE,
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

// Game components ------------------------------------------------------------

struct Player {

};

struct AI {

};

struct SolidTerrain {

};

//struct Transform {
//	glm::vec2 position = { 0,0 };
//	float rotation = 0.0f;
//	glm::vec2 scale = { 10, 10 };
//};

struct Collider {
};

struct Boxcollider : Collider {
	std::vector<glm::vec2> vertices;
	std::vector<int> triangles;
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
	std::vector<float> collisionDepths;
	std::vector<glm::vec2> collisionNormals;
};

struct RayCast {
	int max_depth = 1;
};

// Stucture to store collision information
struct Collision {
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Render components ----------------------------------------------------------

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
