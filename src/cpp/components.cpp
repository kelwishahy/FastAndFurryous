#include "../hpp/components.hpp"
#include <hpp/tiny_ecs_registry.hpp>

// stlib
#include <iostream>
#include <sstream>
#include <glm/gtx/extended_min_max.hpp>

#include "hpp/audio_manager.hpp"

Debug debugging;
float death_timer_counter_ms = 3000;

// Very, VERY simple OBJ loader from https://github.com/opengl-tutorials/ogl tutorial 7
// (modified to also read vertex color and omit uv and normals)
// bool Mesh::loadMeshFromObj(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, glm::vec2& out_size)
// {
// 	// disable warnings about fscanf and fopen on Windows
// #ifdef _MSC_VER
// #pragma warning(disable:4996)
// #endif
//
// 	printf("Loading OBJ file %s...\n", obj_path.c_str());
// 	// Note, normal and UV indices are not loaded/used, but code is commented to do so
// 	std::vector<uint16_t> out_uv_indices, out_normal_indices;
// 	std::vector<glm::vec2> out_uvs;
// 	std::vector<glm::vec3> out_normals;
//
// 	FILE* file = fopen(obj_path.c_str(), "r");
// 	if (file == NULL) {
// 		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
// 		getchar();
// 		return false;
// 	}
//
// 	while (1) {
// 		char lineHeader[128];
// 		// read the first word of the line
// 		int res = fscanf(file, "%s", lineHeader);
// 		if (res == EOF)
// 			break; // EOF = End Of File. Quit the loop.
//
// 		if (strcmp(lineHeader, "v") == 0) {
// 			ColoredVertex vertex;
// 			int matches = fscanf(file, "%f %f %f %f %f %f\n", &vertex.position.x, &vertex.position.y, &vertex.position.z,
// 				&vertex.color.x, &vertex.color.y, &vertex.color.z);
// 			if (matches == 3)
// 				vertex.color = { 1,1,1 };
// 			out_vertices.push_back(vertex);
// 		}
// 		else if (strcmp(lineHeader, "vt") == 0) {
// 			glm::vec2 uv;
// 			fscanf(file, "%f %f\n", &uv.x, &uv.y);
// 			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
// 			out_uvs.push_back(uv);
// 		}
// 		else if (strcmp(lineHeader, "vn") == 0) {
// 			glm::vec3 normal;
// 			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
// 			out_normals.push_back(normal);
// 		}
// 		else if (strcmp(lineHeader, "f") == 0) {
// 			std::string vertex1, vertex2, vertex3;
// 			unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];
//
// 			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
// 			if (matches == 1) // try again
// 			{
// 				// Note first vertex index is already consumed by the first fscanf call (match ==1) since it aborts on the first error
// 				matches = fscanf(file, "//%d %d//%d %d//%d\n", &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
// 				if (matches != 5) // try again
// 				{
// 					matches = fscanf(file, "%d/%d %d/%d/%d %d/%d/%d\n", &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
// 					if (matches != 8)
// 					{
// 						printf("File can't be read by our simple parser :-( Try exporting with other options\n");
// 						fclose(file);
// 						return false;
// 					}
// 				}
// 			}
//
// 			// -1 since .obj starts counting at 1 and OpenGL starts at 0
// 			out_vertex_indices.push_back((uint16_t)vertexIndex[0] - 1);
// 			out_vertex_indices.push_back((uint16_t)vertexIndex[1] - 1);
// 			out_vertex_indices.push_back((uint16_t)vertexIndex[2] - 1);
// 			//out_uv_indices.push_back(uvIndex[0] - 1);
// 			//out_uv_indices.push_back(uvIndex[1] - 1);
// 			//out_uv_indices.push_back(uvIndex[2] - 1);
// 			out_normal_indices.push_back((uint16_t)normalIndex[0] - 1);
// 			out_normal_indices.push_back((uint16_t)normalIndex[1] - 1);
// 			out_normal_indices.push_back((uint16_t)normalIndex[2] - 1);
// 		}
// 		else {
// 			// Probably a comment, eat up the rest of the line
// 			char stupidBuffer[1000];
// 			fgets(stupidBuffer, 1000, file);
// 		}
// 	}
// 	fclose(file);
//
// 	// Compute bounds of the mesh
// 	glm::vec3 max_position = { -99999,-99999,-99999 };
// 	glm::vec3 min_position = { 99999,99999,99999 };
// 	for (ColoredVertex& pos : out_vertices)
// 	{
// 		max_position = max(max_position, pos.position);
// 		min_position = min(min_position, pos.position);
// 	}
// 	if(abs(max_position.z - min_position.z)<0.001)
// 		max_position.z = min_position.z+1; // don't scale z direction when everythin is on one plane
//
// 	glm::vec3 size3d = max_position - min_position;
// 	out_size = size3d;
//
// 	// Normalize mesh to range -0.5 ... 0.5
// 	for (ColoredVertex& pos : out_vertices)
// 		pos.position = ((pos.position - min_position) / size3d) - glm::vec3(0.5f, 0.5f, 0.5f);
//
// 	return true;
// }

void remove_children(Entity e) {
	if (registry.parentEntities.has(e)) {
		const ChildEntities children = registry.parentEntities.get(e);

		for (const auto& pair : children.child_data_map) {
			remove_children(pair.second);
			registry.remove_all_components_of(pair.second);
		}
	}
}

std::vector<Entity> get_all_children(Entity e) {

	std::vector<Entity> allchildren;

	if (registry.parentEntities.has(e)) {
		const ChildEntities children = registry.parentEntities.get(e);

		for (auto pair : children.child_data_map) {
			allchildren.push_back(pair.second);
			for (Entity next_level_child : get_all_children(pair.second)) {
				allchildren.push_back(next_level_child);
			}
		}

	}

	return allchildren;

}
///////////////////CAT MEMBER functions///////////////////////////////
//////////////////////////////////////////////////////////////////////
void Cat::animate_walk() {


	//Animate the cat's main torso
	change_animation(this->character, TEXTURE_IDS::CAT_WALK);

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::CAT_SIDE_BLINK);
			}
		}
	}

}

void Cat::animate_idle() {

	change_animation(this->character, TEXTURE_IDS::CAT_FRONT_IDLE);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::CAT_FRONT_BLINK);
			}
		}

	}
}

void Cat::animate_jump() {
	change_animation(this->character, TEXTURE_IDS::CAT_JUMP);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head" && rig) {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::CAT_FRONT_BLINK);
			}
		}
	}
}

void Cat::animate_hurt() {
	change_animation(this->character, TEXTURE_IDS::CAT_HURT);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::CAT_HURT_FACE);
			}
		}

	}
}

void Cat::animate_dead() {

	registry.renderRequests.remove(this->character);
	registry.renderRequests.insert(this->character, {
		TEXTURE_IDS::CAT_DEAD,
		SHADER_PROGRAM_IDS::TEXTURE,
		GEOMETRY_BUFFER_IDS::TEXTURED_QUAD
	});
}

void Cat::animate_aim() {

	WeaponBase weapon = registry.weapons.get(this->character);
	if (weapon.type == RIFLE) {
		change_animation(this->character, TEXTURE_IDS::CAT_SIDE_IDLE_AK);
	} else if (weapon.type == SHOTGUN) {
		change_animation(this->character, TEXTURE_IDS::CAT_SIDE_IDLE_SG);
	} else if (weapon.type == AWP) {
		change_animation(this->character, TEXTURE_IDS::CAT_SIDE_IDLE_AWP);
	} else if (weapon.type == LAUNCHER) {
		change_animation(this->character, TEXTURE_IDS::CAT_SIDE_IDLE_GL);
	}

	for (Entity rig : registry.animations.entities) {
		Animation& extra = registry.animations.get(rig);
		if (extra.name == "cat_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::CAT_SIDE_BLINK);
			}
		}
	}
}

void Cat::play_hurt_sfx() {
	audio.play_sfx(CAT_SCREAM);
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


///////////////////DOG MEMBER functions///////////////////////////////
//////////////////////////////////////////////////////////////////////
void Dog::animate_walk() {

	///Animate the cat's main torso
	change_animation(this->character, TEXTURE_IDS::DOG_WALK);

	//This is scuffed but we can just look for the extraAnim we need
	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::DOG_SIDE_BLINK);
			}
		}
	}
}

void Dog::animate_idle() {

	change_animation(this->character, TEXTURE_IDS::DOG_FRONT_IDLE);
	std::vector<Entity> children = get_all_children(this->character);
	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::DOG_FRONT_BLINK);
			}
		}

	}
}

void Dog::animate_jump() {

	change_animation(this->character, TEXTURE_IDS::DOG_JUMP);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::DOG_FRONT_BLINK);
			}
		}

	}
}

void Dog::animate_hurt() {

	change_animation(this->character, TEXTURE_IDS::DOG_HURT);

	for (Entity rig : registry.animations.entities) {

		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::DOG_HURT_FACE);
			}
		}

	}
}

void Dog::animate_dead() {
	registry.renderRequests.remove(this->character);
	registry.renderRequests.insert(this->character, {
		TEXTURE_IDS::DOG_DEAD,
		SHADER_PROGRAM_IDS::TEXTURE,
		GEOMETRY_BUFFER_IDS::TEXTURED_QUAD
		});
}

void Dog::animate_aim() {

	WeaponBase weapon = registry.weapons.get(this->character);
	if (weapon.type == RIFLE) {
		change_animation(this->character, TEXTURE_IDS::DOG_SIDE_IDLE_AK);
	}
	else if (weapon.type == SHOTGUN) {
		change_animation(this->character, TEXTURE_IDS::DOG_SIDE_IDLE_SG);
	}
	else if (weapon.type == AWP) {
		change_animation(this->character, TEXTURE_IDS::DOG_SIDE_IDLE_AWP);
	}
	else if (weapon.type == LAUNCHER) {
		change_animation(this->character, TEXTURE_IDS::DOG_SIDE_IDLE_GL);
	}
	for (Entity rig : registry.animations.entities) {
		Animation& extra = registry.animations.get(rig);
		if (extra.name == "dog_head") {
			//Animate the head
			if (check_if_part_of_parent(this->character, rig)) {
				change_animation(rig, TEXTURE_IDS::DOG_SIDE_BLINK);
			}
		}
	}
}

void Dog::play_hurt_sfx() {
	audio.play_sfx(DOG_BARK);
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void change_animation(Entity e, TEXTURE_IDS tex_id) {

	assert(registry.animations.has(e));
	Animation& animation = registry.animations.get(e);
	animation.anim_state = tex_id;

}

bool check_if_part_of_parent(Entity e, Entity child) {
	std::vector<Entity> children = get_all_children(e);
	for (Entity check_child : children) {
		if (child == check_child) {
			return true;
		}
	}
	return false;
}

//------------CHARACTER STATE MACHINE
void CharacterStateMachine::init(CharacterState* starting_state) {
	curr_state = starting_state;
	curr_state->enter();
}

void CharacterStateMachine::changeState(CharacterState* new_state) {
	curr_state->exit();
	curr_state = new_state;
	curr_state->enter();
}
