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
#include <iostream>
#include <map>
#include "../Renderer.h"

using std::expected;
using std::unexpected;
using std::string;
using std::vector;
using std::multimap;
using std::map;
using std::cout;
using std::cerr;

namespace SFT::Renderer::VK {
    class VulkanRenderer : public Renderer {
        private:
#pragma region Private Member Variables
            bool m_isInitialized = false;
            VkInstance m_instance;
            VkDebugUtilsMessengerEXT m_debugMessenger;
            VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
#pragma endregion



#pragma region Internal Functions
            expected<void, string> create_instance();
            static auto checkValidationLayerSupport() -> bool;
            auto setupDebugMessenger() -> expected<void, string>;
            auto pickPhysicalDevice() -> expected<void, string>;
            auto getRequiredExtensions() -> vector<const char*>;
#pragma endregion

        public:
            VulkanRenderer();
            ~VulkanRenderer() override;
            auto Initialize() -> expected<void, string> override;
            void Shutdown() override;
            auto RenderFrame() -> expected<void, string> override;
            auto Resize(int width, int height) -> expected<void, string> override;
            static auto debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData
            ) -> VkBool32;
    };
} // VK

#endif //VULKAN_H
