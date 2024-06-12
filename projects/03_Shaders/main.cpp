#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

void handleInput() {}

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		// positions            // colors
		0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,  // bottom left
		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f   // top
	};

	uint32_t indices[] = { 0, 2, 1 };

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

void render(uint32_t VAO, const Shader& shader) {
	glClear(GL_COLOR_BUFFER_BIT);

	shader.use();
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);
	const Shader shader(PROJECT_PATH "tri.vert", PROJECT_PATH "tri.frag");

	//glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (window->isAlive()) {
		handleInput();
		render(VAO, shader);
		window->handleFrame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return 0;
}