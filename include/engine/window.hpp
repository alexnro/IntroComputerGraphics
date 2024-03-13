#pragma once

#include <cstdint>

class GLFWwindow;

class Window {
public:
	static Window* instance();

	[[nodiscard]] GLFWwindow* getNativeWindow() const {
		return _window;
	}

	[[nodiscard]] bool isAlive() const;
	void handleFrame() const;

private:
	Window();
	~Window();
	GLFWwindow* _window;
	uint32_t _width = 800;
	uint32_t _height = 600;
};
