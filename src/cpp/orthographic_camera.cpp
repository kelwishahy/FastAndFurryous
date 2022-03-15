#include <hpp/orthographic_camera.hpp>
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
: projectionMatrix(ortho(left, right, bottom, top, -1.f, 1.f)) {
	position = vec3(0.f, 0.f,0.f);
	viewMatrix = mat4(1.0);
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void OrthographicCamera::updateViewMatrix() {
	mat4 transform = 
		translate(mat4(1.0), position) * 
		rotate(mat4(1.0), rotationAngle, vec3(0, 0, 1));

	viewMatrix = inverse(transform);
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

