#pragma once

#include <engine/window.hpp>
#include <GLFW/glfw3.h>

class Input {
public:
	/*static Input* instance() {
		static Input i;
		return &i;
	}*/

	static void handleInput(const Window* window) {
		if (glfwGetKey(window->getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window->getNativeWindow(), true);
		}
	}
};
