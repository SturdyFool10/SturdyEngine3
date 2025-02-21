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
#include <optional>
#include <set>
#include "../Renderer.h"

using std::expected;
using std::unexpected;
using std::optional;
using std::string;
using std::set;
using std::vector;
using std::multimap;
using std::map;
using std::cout;
using std::cerr;

namespace SFT::Renderer::VK {

    struct QueueFamilyIndices;

    class VulkanRenderer : public Renderer {
        private:
#pragma region Private Member Variables
            bool m_isInitialized = false;
            VkInstance m_instance;
            VkDebugUtilsMessengerEXT m_debugMessenger;
            VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
            VkDevice m_logicalDevice;
            VkQueue m_graphicsQueue;
#pragma endregion



#pragma region Internal Functions
            auto create_instance()-> expected<void, string>;
            static auto checkValidationLayerSupport() -> bool;
            auto setupDebugMessenger() -> expected<void, string>;auto is_device_compatible(VkPhysicalDevice device)->bool;
            auto pickPhysicalDevice() -> expected<void, string>;
            auto findQueueFamilies(VkPhysicalDevice device)->QueueFamilyIndices;
            auto createLogicalDevice()->expected<void, string>;
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
    struct QueueFamilyIndices {
        optional<uint32_t> graphicsFamily;

        auto isComplete() -> bool;
    };
} // VK

#endif //VULKAN_H
