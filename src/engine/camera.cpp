#include "engine/camera.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
	position_ = position;
	worldUp_ = up;
	yaw_ = yaw;
	pitch_ = pitch;
	fov_ = k_FOV;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
	position_ = glm::vec3(posY, posY, posZ);
	worldUp_ = glm::vec3(upX, upY, upZ);
	yaw_ = yaw;
	pitch_ = pitch;
	fov_ = k_FOV;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(position_, position_ + front_, up_);
}

float Camera::getFOV() const {
	return fov_;
}

glm::vec3 Camera::getPosition() const {
	return position_;
}

void Camera::updateCameraVectors() {
	glm::vec3 front;   // calculate the direction vector
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_ = glm::normalize(front);

	right_ = glm::cross(front_, worldUp_);
	up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::handleKeyboard(Movement direction, float dt) {
	const float velocity = k_Speed * dt;

	switch (direction) {
	case Movement::Forward: position_ += velocity * front_; break;
	case Movement::Backward: position_ -= velocity * front_; break;
	case Movement::Left: position_ -= velocity * right_; break;
	case Movement::Right: position_ += velocity * right_; break;
	}
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
	float xoff = xoffset * k_Sensitivity;  // calc offset movement since last frame
	float yoff = yoffset * k_Sensitivity;  // reversed since y-coordinates go from bottom to top

	yaw_ += xoffset;   // add offsets to pitch/yaw
	pitch_ += yoffset;

	if (constrainPitch) {
		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch_ > 89.0f)
			pitch_ = 89.0f;
		if (pitch_ < -89.0f)
			pitch_ = -89.0f;
	}

	updateCameraVectors();
}

void Camera::handleMouseScroll(float yoffset) {
	if (fov_ >= 1.0f && fov_ <= 45.0f)
		fov_ -= yoffset;
	if (fov_ <= 1.0f)
		fov_ = 1.0f;
	if (fov_ >= 45.0f)
		fov_ = 45.0f;
}

