#include <iostream>
#include <ostream>
#include <engine/window.hpp>
#include <glad/glad.h>

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = { 0.5f, 0.5f, 0.0f,   // top right
					 0.5f, -0.5f, 0.0f,  // bottom right
					-0.5f, -0.5f, 0.0f,  // bottom left
					-0.5f, 0.5f, 0.0f    // top left
	};

	uint32_t indices[] = { 0, 3, 1,  // first triangle
						   1, 3, 2   // second triangle
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

bool checkShader(uint32_t vertexShader) {
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "Error Compiling Shader " << std::endl << infoLog << std::endl;
	}
	return success;
}

bool checkProgram(uint32_t program) {
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(program, 512, nullptr, infoLog);
		std::cout << "Error Compiling Shader " << std::endl << infoLog << std::endl;
	}
	return success;
}

uint32_t createProgram() {
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location=0) in vec3 aPos;\n"
		"void main() {\n"
		"  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	checkShader(vertexShader);

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"  FragColor = vec4(0.7, 0.2, 0.2, 1.0);\n"
		"}\0";

	const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader);

	const uint32_t program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	checkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void render(uint32_t VAO, uint32_t program) {
	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	uint32_t VBO;
	uint32_t EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);
	uint32_t program = createProgram();

	while (window->isAlive()) {
		render(VAO, program);
		window->handleFrame();
	}

	return 0;
}