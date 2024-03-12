#pragma once

class GLFWwindow;

class Window {
public:
	Window();
	~Window();
	static Window* instance();

	[[nodiscard]] static GLFWwindow* getNativeWindow() {
		return _window;
	}

	[[nodiscard]] static bool isAlive();
	static void handleFrame();

private:
	static GLFWwindow* _window;

	/*int _width = 800;
	int _height = 600;*/
};
