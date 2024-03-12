#include <engine/window.hpp>

int main(int, char* []) {
	Window* window = Window::instance();

	while (window->isAlive()) {
		window->handleFrame();
	}

	return 0;
}