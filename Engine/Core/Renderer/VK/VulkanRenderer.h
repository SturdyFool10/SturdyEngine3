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
            VkDebugUtilsMessengerEXT m_debugMessenger;

            std::expected<int, std::string> create_instance();
            static bool checkValidationLayerSupport();
            auto setupDebugMessenger() -> std::expected<void, std::string>;

        public:
            VulkanRenderer();
            ~VulkanRenderer() override;
            std::expected<void, std::string> Initialize() override;
            std::vector<const char*> getRequiredExtensions();
            void Shutdown() override;
            std::expected<void, std::string> RenderFrame() override;
            std::expected<void, std::string> Resize(int width, int height) override;
            static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };
} // VK

#endif //VULKAN_H
