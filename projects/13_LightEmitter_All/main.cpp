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

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

glm::vec3 dirLightDirection(-0.2f, -1.0f, -0.3f);

glm::vec3 pointLightPositions[] = {
  glm::vec3(0.0f,2.0f, -4.0f),
  glm::vec3(0.0f,2.0f, 4.0f),
};

glm::vec3 spotLightPositions[] = {
  glm::vec3(4.0f,2.0f, 0.0f),
  glm::vec3(-4.0f,2.0f, 0.0f)
};

glm::vec3 spotLightDirections[] = {
  glm::vec3(0.0f,-1.0f, 0.0f),
  glm::vec3(0.0f,-1.0f, 0.0f)
};

glm::vec3 objectPositions[] = {
  glm::vec3(0.0f,0.0f, -4.0f),
  glm::vec3(0.0f,0.0f, 4.0f),
  glm::vec3(4.0f,0.0f, 0.0f),
  glm::vec3(-4.0f,0.0f, 0.0f),
  glm::vec3(-4.0f,0.0f, -4.0f),
  glm::vec3(-4.0f,0.0f, 4.0f),
  glm::vec3(4.0f,0.0f, -4.0f),
  glm::vec3(4.0f,0.0f, 4.0f),
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

void render(const Geometry& floor, const Geometry& sphere, const Geometry& teapot, const Shader& light_shader, const Shader& phong_shader, const Texture& t_albedo, const Texture& t_specular) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec3 dirLightcolor(1.0f, 1.0f, 1.0f);
	const glm::vec3 pointLightcolor(1.0f, 0.0f, 0.0f);
	const glm::vec3 spotLightcolor(0.0f, 0.0f, 1.0f);

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);
	const glm::mat4 view = camera.getViewMatrix();

	light_shader.use();

	light_shader.set("view", camera.getViewMatrix());
	light_shader.set("proj", proj);

	for (const auto& pointPos : pointLightPositions) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pointPos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_shader.set("model", model);

		light_shader.set("lightColor", pointLightcolor);

		sphere.render();
	}

	for (const auto& spotPos : spotLightPositions) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, spotPos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_shader.set("model", model);

		light_shader.set("lightColor", spotLightcolor);

		sphere.render();
	}

	phong_shader.use();
	phong_shader.set("view", view);
	phong_shader.set("proj", proj);

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	phong_shader.set("model", model);

	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	phong_shader.set("normalMat", normalMat);

	phong_shader.set("viewPos", camera.getPosition());

	t_albedo.use(phong_shader, "material.diffuse", 0);
	t_specular.use(phong_shader, "material.specular", 1);
	phong_shader.set("material.shininess", 64);

	phong_shader.set("dirLight.direction", dirLightDirection);
	phong_shader.set("dirLight.ambient", dirLightcolor * glm::vec3(0.02f));
	phong_shader.set("dirLight.diffuse", dirLightcolor * glm::vec3(0.1f));
	phong_shader.set("dirLight.specular", dirLightcolor * glm::vec3(0.3f));

	const std::string prefixPoints = "pointLights[";
	for (uint32_t i = 0; i < sizeof(pointLightPositions); ++i) {
		const std::string lightName = prefixPoints + std::to_string(i) + "].";
		phong_shader.set((lightName + "position").c_str(), pointLightPositions[i]);
		phong_shader.set((lightName + "ambient").c_str(), pointLightcolor * glm::vec3(0.02f));
		phong_shader.set((lightName + "diffuse").c_str(), pointLightcolor * glm::vec3(0.2f));
		phong_shader.set((lightName + "specular").c_str(), pointLightcolor * glm::vec3(0.5f));
		phong_shader.set((lightName + "constant").c_str(), 1.0f);
		phong_shader.set((lightName + "linear").c_str(), 0.22f);
		phong_shader.set((lightName + "quadratic").c_str(), 0.20f);
	}

	const std::string prefixSpots = "spotLights[";
	for (uint32_t i = 0; i < sizeof(spotLightPositions); ++i) {
		const std::string lightName = prefixSpots + std::to_string(i) + "].";
		phong_shader.set((lightName + "position").c_str(), spotLightPositions[i]);
		phong_shader.set((lightName + "direction").c_str(), spotLightDirections[i]);
		phong_shader.set((lightName + "ambient").c_str(), spotLightcolor * glm::vec3(0.02f));
		phong_shader.set((lightName + "diffuse").c_str(), spotLightcolor * glm::vec3(0.2f));
		phong_shader.set((lightName + "specular").c_str(), spotLightcolor * glm::vec3(1.0f));
		phong_shader.set((lightName + "constant").c_str(), 1.0f);
		phong_shader.set((lightName + "linear").c_str(), 0.009f);
		phong_shader.set((lightName + "quadratic").c_str(), 0.0032f);
		phong_shader.set((lightName + "cutOff").c_str(), glm::cos(glm::radians(25.0f)));
		phong_shader.set((lightName + "outerCutOff").c_str(), glm::cos(glm::radians(30.0f)));
	}

	floor.render();

	for (auto& pos : objectPositions) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		phong_shader.set("model", model);

		normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
		phong_shader.set("normalMat", normalMat);

		teapot.render();
	}
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setKeyPressedCallback(onKeyPress);
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);

	const Shader light_shader(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
	const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
	const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
	const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
	const Quad quad(1.0f);
	const Sphere sphere(0.5f, 50.0f, 50.0f);
	const Teapot teapot(10);

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
		render(quad, sphere, teapot, light_shader, phong_shader, t_albedo, t_specular);
		window->handleFrame();
	}

	return 0;
}