#pragma once

#include <glm/glm.hpp>

class OrthographicCamera {
public:
	OrthographicCamera() {};
	OrthographicCamera(float left, float right, float bottom, float top);

	glm::mat4 getProjectionMatrix() const { return projectionMatrix; }
	glm::mat4 getViewMatrix() const { return viewMatrix; }
	glm::mat4 getViewProjectionMatrix() const { return viewProjectionMatrix; }

	glm::vec3 getPosition() const { return position; };
	float getRotationAngle() const { return rotationAngle; };

	void setPosition(glm::vec3 position) { this->position = position; updateViewMatrix(); }
	void setRotation(float angle) { this->rotationAngle = angle; updateViewMatrix(); }

	glm::vec3 getCameraRight() const { return glm::vec3(position.x + right, position.y, position.z); }

private:
	void updateViewMatrix();
	float left;
	float right;
	float bottom;
	float top;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 viewProjectionMatrix;

	glm::vec3 position;
	float rotationAngle = 0.f;
};
