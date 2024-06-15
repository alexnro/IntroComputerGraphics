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


	void setWidth(uint32_t width) { _width = width; }
	void setHeight(uint32_t height) { _height = height; }

	[[nodiscard]] uint32_t getWidth() const { return _width; }
	[[nodiscard]] uint32_t getHeight() const { return _height; }

private:
	Window();
	~Window();
	GLFWwindow* _window;
	uint32_t _width = 800;
	uint32_t _height = 600;
};
