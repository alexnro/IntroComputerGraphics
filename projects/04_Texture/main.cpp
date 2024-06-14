#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"

void handleInput() {}

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		// positions         // colors             // texture coords
		0.5f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,     1.0f, 1.0f,          // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,     1.0f, 0.0f,          // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,     0.0f, 0.0f,          // bottom left
		-0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,     0.0f, 1.0f           // top left
	};

	uint32_t indices[] = { 0, 3, 1,
						   1, 3, 2 };

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// text coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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

void render(uint32_t VAO, const Shader& shader, uint32_t tex_1, uint32_t tex_2) {
	glClear(GL_COLOR_BUFFER_BIT);

	shader.use();
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_2);

	shader.set("texture_1", 0);
	shader.set("texture_2", 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);
	const Shader shader(PROJECT_PATH "tri.vert", PROJECT_PATH "tri.frag");

	stbi_set_flip_vertically_on_load(true);
	uint32_t tex_1 = createTexture(ASSETS_PATH "textures/bricks_arrow.jpg");
	uint32_t tex_2 = createTexture(ASSETS_PATH "textures/blue_blocks.jpg");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (window->isAlive()) {
		handleInput();
		render(VAO, shader, tex_1, tex_2);
		window->handleFrame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &tex_1);
	glDeleteTextures(1, &tex_2);

	return 0;
}