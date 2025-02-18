
#include "SturdyEngine.h"

#include "Renderer/VK/VulkanRenderer.h"
#include "Window/GLFW/GLFWWindowWrapped.h"

namespace SFT {
    SturdyEngine::SturdyEngine() {

    }



    void SturdyEngine::main_loop() {
        while (!this->window->should_close()) {
            this->window->ProcessEvents();
            if (std::expected<void, std::string> result = this->renderer->RenderFrame(); (!result.has_value())) break;
        }
    }

    SturdyEngine::~SturdyEngine() {
        this->renderer->Shutdown();
        delete this->renderer;
        this->window->Destroy();
        delete this->window;
    }

    void SturdyEngine::run() {
        glfwInit();
        this->window = new Window::GLFW::GLFWWindowWrapped();
        auto result = this->window->Create(800, 600, "deez nuts");
        if (!result.has_value()) {
            throw std::runtime_error("Failed to initialize window: " + result.error());
        }
        this->window->setBgBlur(true);

        this->renderer = new Renderer::VK::VulkanRenderer();
        if (result = this->renderer->Initialize(); !result.has_value()) {
            throw std::runtime_error("Failed to initialize renderer: " + result.error());
        }
        this->main_loop();
    }
}
