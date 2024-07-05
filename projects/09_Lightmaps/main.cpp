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
#include "engine/camera.hpp"
#include "engine/texture.hpp"
#include "engine/geometry/sphere.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(3.0f, 1.0f, 0.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

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
float lastX, lastY;

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

void render(const Geometry& geometry, const Shader& light_shader, const Shader& phong_shader, const Texture& t_albedo, const Texture& t_specular) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::vec3 lightColor(sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.5f), sin(glfwGetTime() * 1.6f));
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	light_shader.use();

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	light_shader.set("model", model);
	light_shader.set("view", camera.getViewMatrix());
	light_shader.set("proj", proj);
	light_shader.set("lightColor", lightColor);
	geometry.render();


	phong_shader.use();

	phong_shader.set("view", camera.getViewMatrix());
	phong_shader.set("proj", proj);

	model = glm::mat4(1.0f);
	phong_shader.set("model", model);

	const glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	phong_shader.set("normalMat", normalMat);

	t_albedo.use(phong_shader, "material.diffuse", 0);
	t_specular.use(phong_shader, "material.specular", 1);
	phong_shader.set("material.shininess", 64);

	phong_shader.set("light.position", lightPos);
	phong_shader.set("light.ambient", lightColor * glm::vec3(0.2f));
	phong_shader.set("light.diffuse", lightColor * glm::vec3(0.5f));
	phong_shader.set("light.specular", glm::vec3(1.0f));

	phong_shader.set("viewPos", camera.getPosition());

	geometry.render();
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

	const Shader light_shader(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
	const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
	const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
	const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
	const Sphere sphere(1.0f, 50, 50);

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
		render(sphere, light_shader, phong_shader, t_albedo, t_specular);
		window->handleFrame();
	}

	return 0;
}