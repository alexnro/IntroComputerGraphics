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

std::pair<uint32_t, uint32_t> createFBO() {
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	uint32_t textureColor;
	glGenTextures(1, &textureColor);
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::instance()->getWidth(), Window::instance()->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);

	uint32_t rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::instance()->getWidth(), Window::instance()->getHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error Framebuffer not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_pair(fbo, textureColor);
}

void render(const Geometry& floor, const Geometry& cube, const Shader& fbo_shader, const Shader& phong_shader, const Texture& t_albedo, const Texture& t_specular, const uint32_t fbo, const uint32_t text_fbo) {
	const glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
	const glm::vec3 dirLightColor(1.0f, 1.0f, 1.0f);

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
	const glm::mat4 view = camera.getViewMatrix();

	{ // first offline render
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		phong_shader.use();
		phong_shader.set("view", camera.getViewMatrix());
		phong_shader.set("proj", proj);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		phong_shader.set("model", model);

		glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
		phong_shader.set("normalMat", normalMat);

		phong_shader.set("viewPos", camera.getPosition());
		t_albedo.use(phong_shader, "material.diffuse", 0);
		t_specular.use(phong_shader, "material.specular", 1);
		phong_shader.set("material.shininess", 16);

		phong_shader.set("dirLight.direction", lightDir);
		phong_shader.set("dirLight.ambient", dirLightColor * glm::vec3(0.02f));
		phong_shader.set("dirLight.diffuse", dirLightColor * glm::vec3(0.1f));
		phong_shader.set("dirLight.specular", dirLightColor * glm::vec3(0.3f));

		floor.render();

		model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		phong_shader.set("model", model);
		normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
		phong_shader.set("normalMat", normalMat);

		cube.render();
	}

	{ // second, to screen
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT);

		fbo_shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, text_fbo);

		fbo_shader.set("screen_texture", 0);

		floor.render();
	}
}

int main(int, char* []) {
	Window* window = Window::instance();

	Input* input = Input::instance();
	input->setKeyPressedCallback(onKeyPress);
	input->setMouseMoveCallback(onMouseMove);
	input->setScrollMoveCallback(onScroll);

	const Shader fbo_shader(PROJECT_PATH "fbo.vert", PROJECT_PATH "fbo.frag");
	const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
	const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
	const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
	const Quad quad(1.0f);
	const Cube cube(5.0f);

	auto fbo_res = createFBO();

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
		render(quad, cube, fbo_shader, phong_shader, t_albedo, t_specular, fbo_res.first, fbo_res.second);
		window->handleFrame();
	}

	return 0;
}