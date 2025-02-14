//
// Created by sturd on 2/13/2025.
//

#include "VulkanRenderer.h"

#include <iostream>
const std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif


namespace SFT::Renderer::VK {

#pragma region additional functions
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        if (auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VulkanRenderer::debugCallback;
    }
#pragma endregion



    auto VulkanRenderer::create_instance() -> std::expected<int, std::string> {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            // ReSharper disable once CppDFAUnreachableCode
            return std::unexpected("validation layers requested, but not available!");
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

        auto extensions = getRequiredExtensions();
        uint32_t extensionCount = static_cast<uint32_t>(extensions.size());

        std::vector<const char*> requiredExtensions;

        requiredExtensions.reserve(extensionCount);
        for(uint32_t i = 0; i < extensionCount; i++) {
            requiredExtensions.emplace_back(extensions[i]);
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        // ReSharper disable once CppDFAUnreachableCode
        if (enableValidationLayers) {
            create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            create_info.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            create_info.pNext = &debugCreateInfo;
        } else {
            create_info.enabledLayerCount = 0;

            create_info.pNext = nullptr;
        }
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        create_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        create_info.ppEnabledExtensionNames = requiredExtensions.data();

        if (VkResult result = vkCreateInstance(&create_info, nullptr, &this->m_instance); result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            return std::unexpected("We failed to create the Vulkan instance because the driver is incompatible");
        } else if (result != VK_SUCCESS) {
            return std::unexpected("We failed to create the Vulkan instance");
        }

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionsList(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsList.data());

        std::cout << "available extensions:\n";

        for (const auto& [extensionName, specVersion] : extensionsList) {
            std::cout << '\t' << extensionName << '\n';
        }
        return {};
    }

    auto VulkanRenderer::checkValidationLayerSupport() -> bool {
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

    auto VulkanRenderer::setupDebugMessenger() -> std::expected<void, std::string> {
        if (!enableValidationLayers) return {};

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(this->m_instance, &createInfo, nullptr, &this->m_debugMessenger) != VK_SUCCESS) {
            return std::unexpected("failed to set up debug messenger!");
        }
        return {};
    }

    VulkanRenderer::VulkanRenderer() {

    }

    VulkanRenderer::~VulkanRenderer() {

    }

    auto VulkanRenderer::Initialize() -> std::expected<void, std::string> {
        auto result = this->create_instance();
        if (!result.has_value()) {
            return std::unexpected("Failed to create Vulkan instance: " + result.error());
        }
        auto result2 = this->setupDebugMessenger();
        if (!result2.has_value()) {
            return std::unexpected("Failed to set up debug messenger: " + result2.error());
        }
        return {};
    }

    auto VulkanRenderer::getRequiredExtensions() -> std::vector<const char*> {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    void VulkanRenderer::Shutdown() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(this->m_instance, this->m_debugMessenger, nullptr);
        }
        vkDestroyInstance(this->m_instance, nullptr);
    }

    auto VulkanRenderer::RenderFrame() -> std::expected<void, std::string> {
        return {};
    }

    auto VulkanRenderer::Resize(int width, int height) -> std::expected<void, std::string> {
        return {};
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";
        }

        return VK_FALSE;
    }
}
