#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/input.hpp"
#include "engine/texture.hpp"
#include "engine/geometry/cube.hpp"

std::vector<glm::vec3> cubePositions{
  glm::vec3(0.0f, 0.0f,0.0f),
  glm::vec3(1.0f, 6.0f, -20.0f),
  glm::vec3(-2.0f, -2.0f,-2.0f),
  glm::vec3(-4.0f, -6.0f,-10.0f),
  glm::vec3(3.0f, -1.0f,-4.0f),
  glm::vec3(-2.0f, 3.0f,-8.0f),
  glm::vec3(2.0f, -2.0f,-3.0f),
  glm::vec3(1.0f, 2.0f,-2.0f),
  glm::vec3(1.0f, 0.0f,-2.0f),
  glm::vec3(-1.0f, 1.0f,-1.0f)
};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
const float cameraSpeed = 2.0f;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = (float)800 / 2.0;
float lastY = (float)600 / 2.0;
float fov = 45.0f;

void handleInput(float deltaTime) {
	const float speed = cameraSpeed * deltaTime;
	if (Input::isKeyPressed(GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += speed * cameraFront;
	if (Input::isKeyPressed(GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= speed * cameraFront;
	if (Input::isKeyPressed(GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= speed * cameraRight;
	if (Input::isKeyPressed(GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += speed * cameraRight;
}

void render(const Geometry& geometry, const Shader& shader, const Texture& texture) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(fov), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);

	//shader.set("model", model);
	shader.set("view", view);
	shader.set("proj", proj);

	texture.use(shader, "texture_1", 0);

	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	for (size_t i = 0; i < cubePositions.size(); ++i) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		const float angle = 10.0f + (20.0f * static_cast<float>(i));
		model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

		shader.set("model", model);

		geometry.render();

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

void onMouseMove(double xpos, double ypos) {
	if (firstMouse) {     // initial values the first frame
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;  // calc offset movement since last frame
	float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;    // constraint movement speed
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;   // add offsets to pitch/yaw
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;   // calculate the direction vector
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void onScroll(double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);

	const Shader shader(PROJECT_PATH "3d.vert", PROJECT_PATH "3d.frag");

	const Texture texture(ASSETS_PATH "textures/bricks_arrow.jpg", Texture::Format::RGB);

	const Cube cube(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float lastFrame = 0.0f;

	while (window->isAlive()) {
		const float currentFrame = static_cast<float>(glfwGetTime());
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleInput(deltaTime);
		render(cube, shader, texture);
		window->handleFrame();
	}

	return 0;
}