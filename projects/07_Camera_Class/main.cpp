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
#include "engine/camera.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

bool firstMouse = true;
float lastX = (float)800 / 2.0;
float lastY = (float)600 / 2.0;

void handleInput(float delta) {
	Input* input = Input::instance();

	if (input->isKeyPressed(GLFW_KEY_W))
		camera.handleKeyboard(Camera::Movement::Forward, delta);
	if (input->isKeyPressed(GLFW_KEY_S))
		camera.handleKeyboard(Camera::Movement::Backward, delta);
	if (input->isKeyPressed(GLFW_KEY_A))
		camera.handleKeyboard(Camera::Movement::Left, delta);
	if (input->isKeyPressed(GLFW_KEY_D))
		camera.handleKeyboard(Camera::Movement::Right, delta);
}

void render(const Geometry& geometry, const Shader& shader, const Texture& texture) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);

	//shader.set("model", model);
	shader.set("view", camera.getViewMatrix());
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
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMovement(xoffset, yoffset);
}

void onScroll(double xoffset, double yoffset) {
	camera.handleMouseScroll(yoffset);
}

void onKeyPress(int key, int action) {
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		Window::instance()->setCaptureMouse(true);
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		Window::instance()->setCaptureMouse(false);
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);
	input->setKeyPressedCallback(onKeyPress);

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