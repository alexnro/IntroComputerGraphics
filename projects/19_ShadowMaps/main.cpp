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
glm::vec3 lightPos(1.5f, 4.0f, 2.0f);
const glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

const uint32_t k_shadow_width = 1024;
const uint32_t k_shadow_height = 1024;

const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

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

std::pair<uint32_t, uint32_t> createFBO() {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    uint32_t depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, k_shadow_width, k_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error Framebuffer not complete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return std::make_pair(fbo, depthMap);
}

void renderScene(const Shader& shader, const Geometry& quad, const Geometry& cube, const Geometry& sphere, const Texture& t_albedo, const Texture& t_specular) {
    t_albedo.use(shader, "material.diffuse", 0);
    t_specular.use(shader, "material.specular", 1);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    shader.set("model", model);
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    quad.render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.set("model", model);
    normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    cube.render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.5f, 0.0f));
    shader.set("model", model);
    normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    cube.render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.75f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader.set("model", model);
    normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    sphere.render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    shader.set("model", model);
    normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    sphere.render();
}

void render(const Geometry& quad, const Geometry& cube, const Geometry& sphere, const Shader& depth_shader, const Shader& phong_shader, const Shader& light_shader, const Shader& debug_shader, const Texture& t_albedo, const Texture& t_specular,
    const uint32_t fbo, const uint32_t text_fbo) {
    Window* window = Window::instance();

    //FIRST PASS
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, k_shadow_near, k_shadow_far);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    depth_shader.use();
    depth_shader.set("lightSpaceMatrix", lightSpaceMatrix);

    renderScene(depth_shader, quad, cube, sphere, t_albedo, t_specular);

    ////DEBUG PASS
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0, 0, (float)window->getWidth(), (float)window->getHeight());
    //glClear(GL_COLOR_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    //glCullFace(GL_BACK);

    //debug_shader.use();
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, text_fbo);
    //debug_shader.set("depth_map", 0);

    //quad.render();

    //SECOND PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (float)window->getWidth(), (float)window->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    const glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
    const glm::mat4 view = camera.getViewMatrix();

    phong_shader.use();
    phong_shader.set("view", view);
    phong_shader.set("proj", proj);

    phong_shader.set("viewPos", camera.getPosition());
    phong_shader.set("material.shininess", 16);

    phong_shader.set("dirLight.position", lightPos);
    phong_shader.set("dirLight.ambient", lightColor * glm::vec3(0.1f));
    phong_shader.set("dirLight.diffuse", lightColor * glm::vec3(0.5f));
    phong_shader.set("dirLight.specular", lightColor * glm::vec3(1.0f));

    phong_shader.set("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, text_fbo);
    phong_shader.set("depth_map", 2);

    renderScene(phong_shader, quad, cube, sphere, t_albedo, t_specular);
}

int main(int, char* []) {
    Window* window = Window::instance();

    Input* input = Input::instance();
    input->setKeyPressedCallback(onKeyPress);
    input->setMouseMoveCallback(onMouseMove);
    input->setScrollMoveCallback(onScroll);

    const Shader depth_shader(PROJECT_PATH "depth.vert", PROJECT_PATH "depth.frag");
    const Shader debug_shader(PROJECT_PATH "debug.vert", PROJECT_PATH "debug.frag");
    const Shader light_shader(PROJECT_PATH "light.vert", PROJECT_PATH "light.frag");
    const Shader phong_shader(PROJECT_PATH "phong.vert", PROJECT_PATH "phong.frag");
    const Texture t_albedo(ASSETS_PATH "textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular(ASSETS_PATH "textures/bricks_specular.png", Texture::Format::RGB);
    const Quad quad(2.0f);
    const Cube cube(1.0f);
    const Sphere sphere(1.0f, 20, 20);

    auto fbo_res = createFBO();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lastFrame = 0.0f;

    while (window->isAlive()) {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, cube, sphere, depth_shader, phong_shader, light_shader, debug_shader, t_albedo, t_specular, fbo_res.first, fbo_res.second);
        window->handleFrame();
    }

    return 0;
}