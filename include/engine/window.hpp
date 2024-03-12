#pragma once

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
	/*int _width = 800;
	int _height = 600;*/
};
