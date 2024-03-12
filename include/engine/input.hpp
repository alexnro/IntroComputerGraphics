#pragma once

#include <engine/window.hpp>
#include <GLFW/glfw3.h>

class Input {
public:
	static void handleInput() {
		if (glfwGetKey(Window::getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Window::getNativeWindow(), true);
		}
	}
};
