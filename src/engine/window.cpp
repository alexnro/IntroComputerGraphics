#include <iostream>
#include <ostream>
#include <engine/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* Window::_window = nullptr;

void onChangeFrameBufferSize(GLFWwindow* window, const int32_t width, const int32_t height) {
	glViewport(0, 0, width, height);
	std::cout << width << " " << height << std::endl;
}

Window::Window() {
	if (!glfwInit()) {
		std::cout << "Error Initializing GLFW" << std::endl;
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(800, 600, "60GIIN", nullptr, nullptr);
	if (!_window) {
		std::cout << "Error Creating Window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(_window);

	if (!gladLoadGL()) {
		std::cout << "Error Initializing GLAD" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	glfwSetFramebufferSizeCallback(_window, onChangeFrameBufferSize);

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
}

Window::~Window() {
	glfwTerminate();
}


Window* Window::instance() {
	static Window w;
	return &w;
}

bool Window::isAlive() {
	return !glfwWindowShouldClose(getNativeWindow());
}

void Window::handleFrame() {
	glfwSwapBuffers(getNativeWindow());
	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT);
}
