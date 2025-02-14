//
// Created by sturd on 2/13/2025.
//

#include "VulkanRenderer.h"


namespace SFT::Renderer::VK {
    VulkanRenderer::VulkanRenderer() {

    }

    VulkanRenderer::~VulkanRenderer() {

    }

    std::expected<void, std::string> VulkanRenderer::Initialize() {
        
        return {};
    }

    void VulkanRenderer::Shutdown() {

    }

    std::expected<void, std::string> VulkanRenderer::RenderFrame() {
        return {};
    }

    std::expected<void, std::string> VulkanRenderer::Resize(int width, int height) {
        return {};
    }

}
