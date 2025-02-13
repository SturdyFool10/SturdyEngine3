//
// Created by sturd on 2/13/2025.
//

#include "Vulkan.h"

#include <expected>
#include <string>

namespace SFT::Renderer::VK {
    Vulkan::Vulkan() {
        this->Vulkan::Initialize();
    }

    Vulkan::~Vulkan() {

    }

    std::expected<void, std::string> Vulkan::Initialize() {
        return {};
    }

    void Vulkan::Shutdown() {

    }

    std::expected<void, std::string> Vulkan::RenderFrame() {

    }

    std::expected<void, std::string> Vulkan::Resize(int width, int height) {

    }
}
