#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/input.hpp"
#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/geometry/teapot.hpp"
#include "engine/camera.hpp"
#include "engine/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <glm/ext/matrix_clip_space.hpp>

#include "stb_image.h"
#include "engine/model.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

//TODO Move to Camera Controller Class
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

void onKeyPress(int key, int action) {
	if ((key == GLFW_KEY_Q) && (action == GLFW_PRESS))
		Window::instance()->setCaptureMouse(true);
	if ((key == GLFW_KEY_E) && (action == GLFW_PRESS))
		Window::instance()->setCaptureMouse(false);
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

	camera.handleMouseMovement(xoffset, yoffset);
}

void onScroll(double xoffset, double yoffset) {
	camera.handleMouseScroll(yoffset);
}

void render(const Shader& shader, const Model& object) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec3 dirLightcolor(1.0f, 1.0f, 1.0f);
	const glm::vec3 pointLightcolor(1.0f, 0.0f, 0.0f);
	const glm::vec3 spotLightcolor(0.0f, 0.0f, 1.0f);

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);
	const glm::mat4 view = camera.getViewMatrix();

	shader.use();

	shader.set("view", view);
	shader.set("proj", proj);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	shader.set("model", model);

	object.render(shader);
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setKeyPressedCallback(onKeyPress);
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);

	const Shader shader(PROJECT_PATH "vertex.vert", PROJECT_PATH "fragment.frag");
	const Model object(ASSETS_PATH "models/Freighter/Freigther_BI_Export.obj");

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float lastFrame = 0.0f;

	while (window->isAlive()) {
		const auto currentFrame = static_cast<float>(glfwGetTime());
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleInput(deltaTime);
		render(shader, object);
		window->handleFrame();
	}

	return 0;
}