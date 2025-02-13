
#include "SturdyEngine.h"

#include "Renderer/VK/Vulkan.h"

namespace SFT {
    SturdyEngine::SturdyEngine() {
        this->renderer = new Renderer::VK::Vulkan();
        if (auto result = this->renderer->Initialize(); !result) {
            throw std::runtime_error("Failed to initialize renderer: " + result.error());
        }
    }
}
