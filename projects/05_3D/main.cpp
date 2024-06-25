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

void handleInput() {}

void render(const Geometry& geometry, const Shader& shader, const Texture& texture) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);

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

int main(int, char* []) {
	Window* window = Window::instance();

	const Shader shader(PROJECT_PATH "3d.vert", PROJECT_PATH "3d.frag");

	const Texture texture(ASSETS_PATH "textures/bricks_arrow.jpg", Texture::Format::RGB);

	const Cube cube(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (window->isAlive()) {
		handleInput();
		render(cube, shader, texture);
		window->handleFrame();
	}

	return 0;
}