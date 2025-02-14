//
// Created by sturd on 2/13/2025.
//

#include "VulkanRenderer.h"

#include <iostream>
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

namespace SFT::Renderer::VK {
    auto SFT::Renderer::VK::VulkanRenderer::create_instance() -> std::expected<int, std::string> { //ended on MessageCallback for Error Handling
        if (enableValidationLayers && !this->checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "SFT";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "SturdyEngine3";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &appInfo;

        uint32_t extensionCount = 0;
        const char** extensions;
        extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        std::vector<const char*> requiredExtensions;

        requiredExtensions.reserve(extensionCount);
        for(uint32_t i = 0; i < extensionCount; i++) {
            requiredExtensions.emplace_back(extensions[i]);
        }
        if (enableValidationLayers) {
            create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            create_info.ppEnabledLayerNames = validationLayers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        create_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        create_info.ppEnabledExtensionNames = requiredExtensions.data();

        VkResult result = vkCreateInstance(&create_info, nullptr, &this->m_instance);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            return std::unexpected("We failed to create the Vulkan instance because the driver is incompatible");
        } else if (result != VK_SUCCESS) {
            return std::unexpected("We failed to create the Vulkan instance");
        }

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionsList(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsList.data());

        std::cout << "available extensions:\n";

        for (const auto& extension : extensionsList) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        return {};
    }
    bool VulkanRenderer::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }
    VulkanRenderer::VulkanRenderer() {

    }

    VulkanRenderer::~VulkanRenderer() {

    }

    std::expected<void, std::string> VulkanRenderer::Initialize() {
        auto result = this->create_instance();
        if (!result.has_value()) {
            return std::unexpected("Failed to create Vulkan instance: " + result.error());
        }
        return {};
    }

    void VulkanRenderer::Shutdown() {
        vkDestroyInstance(this->m_instance, nullptr);
    }

    std::expected<void, std::string> VulkanRenderer::RenderFrame() {
        return {};
    }

    std::expected<void, std::string> VulkanRenderer::Resize(int width, int height) {
        return {};
    }

}
