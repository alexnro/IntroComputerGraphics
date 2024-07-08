#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/input.hpp"
#include "engine/window.hpp"
#include "engine/shader.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/camera.hpp"
#include "engine/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <glm/ext/matrix_clip_space.hpp>

#include "stb_image.h"
#include "engine/geometry/cube.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

//TODO Move to Camera Controller Class
bool firstMouse = true;
float lastX, lastY;

void handleInput(float delta) {
    Input* input = Input::instance();

    if (input->isKeyPressed(GLFW_KEY_W))
        camera.handleKeyboard(Camera::Movement::Forward, delta);
    if (input->isKeyPressed(GLFW_KEY_S))
        camera.handleKeyboard(Camera::Movement::Backward, delta);
    if (input->isKeyPressed(GLFW_KEY_A))
        camera.handleKeyboard(Camera::Movement::Left, delta);
    if (input->isKeyPressed(GLFW_KEY_D))
        camera.handleKeyboard(Camera::Movement::Right, delta);
}

void onKeyPress(int key, int action) {
    if ((key == GLFW_KEY_Q) && (action == GLFW_PRESS))
        Window::instance()->setCaptureMouse(true);
    if ((key == GLFW_KEY_E) && (action == GLFW_PRESS))
        Window::instance()->setCaptureMouse(false);
}

void onMouseMove(double xpos, double ypos) {
    if (firstMouse) {     // initial values the first frame
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;  // calc offset movement since last frame
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.handleMouseMovement(xoffset, yoffset);
}

void onScroll(double xoffset, double yoffset) {
    camera.handleMouseScroll(yoffset);
}

void render(const Geometry& quad, const Shader& normal_shader, const Shader& phong_shader, const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 lightDirColor(1.0f, 1.0f, 1.0f);

    Window* window = Window::instance();
    const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getWidth(), 0.1f, 100.0f);
    const glm::mat4 view = camera.getViewMatrix();

    float l_pos[] = { std::sin((float)glfwGetTime() / 2.0f), 0.0f, std::abs(std::cos((float)glfwGetTime() / 2.0f)) };
    glm::vec3 lightPos = glm::vec3(l_pos[0], l_pos[1], l_pos[2]);

    //lightPos = glm::vec3(0.0f, 1.0f, 2.0f);

    {
        phong_shader.use();
        phong_shader.set("view", view);
        phong_shader.set("proj", proj);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        phong_shader.set("model", model);

        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
        phong_shader.set("normalMat", normalMat);

        phong_shader.set("viewPos", camera.getPosition());
        t_albedo.use(phong_shader, "material.diffuse", 0);
        t_specular.use(phong_shader, "material.specular", 1);
        t_normal.use(phong_shader, "material.normal", 2);
        phong_shader.set("material.shininess", 16);

        phong_shader.set("light.position", lightPos);
        phong_shader.set("light.ambient", lightDirColor * glm::vec3(0.02f));
        phong_shader.set("light.diffuse", lightDirColor * glm::vec3(0.1f));
        phong_shader.set("light.specular", lightDirColor * glm::vec3(0.3f));

        quad.render();
    }

    {
        normal_shader.use();
        normal_shader.set("view", view);
        normal_shader.set("proj", proj);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        normal_shader.set("model", model);

        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
        normal_shader.set("normalMat", normalMat);

        normal_shader.set("viewPos", camera.getPosition());
        t_albedo.use(normal_shader, "material.diffuse", 0);
        t_specular.use(normal_shader, "material.specular", 1);
        t_normal.use(normal_shader, "material.normal", 2);
        normal_shader.set("material.shininess", 16);

        normal_shader.set("light.position", lightPos);
        normal_shader.set("light.ambient", lightDirColor * glm::vec3(0.02f));
        normal_shader.set("light.diffuse", lightDirColor * glm::vec3(0.1f));
        normal_shader.set("light.specular", lightDirColor * glm::vec3(0.3f));

        quad.render();
    }
}

int main(int, char* []) {
    Window* window = Window::instance();

    Input* input = Input::instance();
    input->setKeyPressedCallback(onKeyPress);
    input->setMouseMoveCallback(onMouseMove);
    input->setScrollMoveCallback(onScroll);

    const Shader normal_shader(PROJECT_PATH "normal.vert", PROJECT_PATH "normal.frag");
    const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
    const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
    const Texture t_normal(ASSETS_PATH "textures/bricks_normal.png", Texture::Format::RGB);
    const Quad quad(2.0f);

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lastFrame = 0.0f;

    while (window->isAlive()) {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, normal_shader, phong_shader, t_albedo, t_specular, t_normal);
        window->handleFrame();
    }

    return 0;
}