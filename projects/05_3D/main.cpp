#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void handleInput() {}

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		// Position                  // UVs
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, //Front
		0.5f,  -0.5f, 0.5f,     1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
		-0.5f, 0.5f,  0.5f,     0.0f, 1.0f,
		0.5f,  -0.5f, 0.5f,     0.0f, 0.0f, //Right
		0.5f,  -0.5f, -0.5f,    1.0f, 0.0f,
		0.5f,  0.5f,  -0.5f,    1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,     0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f, //Back
		-0.5f, 0.5f,  -0.5f,    1.0f, 1.0f,
		0.5f,  0.5f,  -0.5f,    0.0f, 1.0f,
		0.5f,  -0.5f, -0.5f,    0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     1.0f, 0.0f, //Left
		-0.5f, 0.5f,  0.5f,     1.0f, 1.0f,
		-0.5f, 0.5f,  -0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 1.0f, //Bottom
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		0.5f,  -0.5f, -0.5f,    1.0f, 0.0f,
		0.5f,  -0.5f, 0.5f,     1.0f, 1.0f,
		-0.5f, 0.5f,  0.5f,     0.0f, 0.0f, //Top
		0.5f,  0.5f,  0.5f,     1.0f, 0.0f,
		0.5f,  0.5f,  -0.5f,    1.0f, 1.0f,
		-0.5f, 0.5f,  -0.5f,    0.0f, 1.0f
	};

	uint32_t indices[] = {
		0, 1, 2,      0, 2, 3,    //Front
		4, 5, 6,      4, 6, 7,    //Right
		8, 9, 10,     8, 10, 11,  //Back
		12, 13, 14,   12, 14, 15, //Left
		16, 17, 18,   16, 18, 19, //Bottom
		20, 21, 22,   20, 22, 23  //Top
	};

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// text coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

uint32_t createTexture(const char* path) {
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	} else {
		std::cout << "Failed loading texture " << path << std::endl;
	}

	return texture;
}

void render(uint32_t VAO, const Shader& shader, uint32_t tex) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	Window* window = Window::instance();
	const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);

	shader.set("model", model);
	shader.set("view", view);
	shader.set("proj", proj);

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	shader.set("texture_1", 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);
	const Shader shader(PROJECT_PATH "3d.vert", PROJECT_PATH "3d.frag");

	stbi_set_flip_vertically_on_load(true);
	uint32_t tex = createTexture(ASSETS_PATH "textures/bricks_arrow.jpg");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (window->isAlive()) {
		handleInput();
		render(VAO, shader, tex);
		window->handleFrame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &tex);

	return 0;
}