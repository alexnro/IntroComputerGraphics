#include <engine/window.hpp>

int main(int, char* []) {
	Window* window = new Window();

	while (window->isAlive()) {
		window->handleFrame();
	}

	return 0;
}