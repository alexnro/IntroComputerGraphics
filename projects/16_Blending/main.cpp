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
#include "engine/geometry/cube.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

glm::vec3 objectPositions[] = {
  glm::vec3(0.0f, 0.0f, 5.0f),
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, 0.0f, -5.0f)
};

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

void render(const Geometry& floor, const Geometry& cube, const Shader& blend_shader, const Shader& phong_shader, const Texture& t_albedo, const Texture& t_specular, const Texture& t_tree) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
	const glm::vec3 dirLightColor(1.0f, 1.0f, 1.0f);

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);
	const glm::mat4 view = camera.getViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

	phong_shader.use();

	phong_shader.set("view", camera.getViewMatrix());
	phong_shader.set("proj", proj);
	phong_shader.set("model", model);
	phong_shader.set("viewPos", camera.getPosition());
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	phong_shader.set("normalMat", normalMat);

	t_albedo.use(phong_shader, "material.diffuse", 0);
	t_specular.use(phong_shader, "material.specular", 1);
	phong_shader.set("shininess", 16);

	phong_shader.set("dirLight.direction", lightDir);
	phong_shader.set("dirLight.ambient", dirLightColor * glm::vec3(0.02f));
	phong_shader.set("dirLight.diffuse", dirLightColor * glm::vec3(0.1f));
	phong_shader.set("dirLight.specular", dirLightColor * glm::vec3(0.3f));

	floor.render();


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	phong_shader.set("model", model);
	normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	phong_shader.set("normalMat", normalMat);

	cube.render();

	blend_shader.use();
	blend_shader.set("view", camera.getViewMatrix());
	blend_shader.set("proj", proj);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
	blend_shader.set("model", model);
	normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	blend_shader.set("normalMat", normalMat);
	t_tree.use(blend_shader, "texture1", 0);

	floor.render();
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setKeyPressedCallback(onKeyPress);
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);

	const Shader blend_shader(PROJECT_PATH "blend.vert", PROJECT_PATH "blend.frag");
	const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
	const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
	const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
	const Texture t_tree(ASSETS_PATH "textures/tree.png", Texture::Format::RGBA);
	const Quad quad(1.0f);
	const Cube cube(5.0f);

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float lastFrame = 0.0f;

	while (window->isAlive()) {
		const auto currentFrame = static_cast<float>(glfwGetTime());
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleInput(deltaTime);
		render(quad, cube, blend_shader, phong_shader, t_albedo, t_specular, t_tree);
		window->handleFrame();
	}

	return 0;
}