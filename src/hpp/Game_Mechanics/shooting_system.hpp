#pragma once
#include "hpp/tiny_ecs.hpp"
#include "hpp/common.hpp"
#include <hpp/render_system.hpp>

using namespace glm;

class ShootingSystem 
{
public:

	ShootingSystem();
	~ShootingSystem();

	void init(RenderSystem* renderer);

	void step(float elapsed_time);

	//Calculate the coefficients for Cubic interpolation
	//x1: origin, x2: endpoint, x1p: tangent at x1, x2p: tangent at x2
	vec4 calculate_A(float c1, float c2, float c1p, float c2p);

	//calculate the interpolated point
	//deltaTime is normalized between 0 and 1, where 0 is the time at and 1 is the time at end
	float calculate_point(vec4 A, float deltaTime);

	void aimUp(Entity e);

	void aimDown(Entity e);

	void setAimLoc(Entity e);

	void shoot(Entity e, Mix_Chunk* sound);

private:

	//Bless 426
	//Matrix should look like this
	//[ 2 -2  1  1]
	//[-3  3 -2 -1]
	//[ 0  0  1  0]
	//[ 1  0  0  0]
	mat4 hermite_matrix = mat4(vec4(2, -3, 0, 1), vec4(-2, 3, 0, 0), vec4(1, -2, 1, 0), vec4(1, -1, 0, 0));

	float pi = 3.14159;
	float pio2 = 1.57079;

	RenderSystem* renderer;

	enum class SHOOT_ORIENTATION {
		RIGHT,
		LEFT
	};
};

