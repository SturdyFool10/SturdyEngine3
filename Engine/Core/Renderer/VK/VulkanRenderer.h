//
// Created by sturd on 2/13/2025.
//

#ifndef VULKAN_H
#define VULKAN_H
#include <complex>
#include <expected>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "../Renderer.h"

namespace SFT::Renderer::VK {
    class VulkanRenderer : public Renderer {
        private:
            bool m_isInitialized = false;
            VkInstance m_instance;



            std::expected<int, std::string> create_instance();
            bool checkValidationLayerSupport();

        public:
            VulkanRenderer();
            ~VulkanRenderer() override;
            std::expected<void, std::string> Initialize() override;
            void Shutdown() override;
            std::expected<void, std::string> RenderFrame() override;
            std::expected<void, std::string> Resize(int width, int height) override;
    };
} // VK

#endif //VULKAN_H
