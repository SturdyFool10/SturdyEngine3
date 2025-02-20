//
// Created by sturd on 2/13/2025.
//

#include "VulkanRenderer.h"

#include <string.h>

const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
    #define debug if(false) std::cout
#else
constexpr bool enableValidationLayers = true;
#define debug std::cout
#endif

namespace SFT::Renderer::VK {

#pragma region additional functions
    auto DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks* pAllocator) -> void {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    auto CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger) -> VkResult {
        if (auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    auto populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) -> void {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VulkanRenderer::debugCallback;
    }

    auto rate_device_suitability(const VkPhysicalDevice& device) -> double {
        map<string, double> type_multipliers = {
                {"discrete", 1},
                {"integrated", 0.75},
                {"virtual", 0.5},
                {"cpu", 0.25},
            };
        map<string, double> feature_multipliers = {
                {"geometryShader", 1.3},
                {"tessellationShader", 1.2},
                {"multiViewport", 1.1},
                {"samplerAnisotropy", 1.2},
                {"textureCompressionBC", 1.1},
                {"fillModeNonSolid", 1.05},
                {"wideLines", 1.05}
            };

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        double score = 30.;

        // Apply feature multipliers
        if (deviceFeatures.geometryShader)
            score *= feature_multipliers["geometryShader"];
        if (deviceFeatures.tessellationShader)
            score *= feature_multipliers["tessellationShader"];
        if (deviceFeatures.multiViewport)
            score *= feature_multipliers["multiViewport"];
        if (deviceFeatures.samplerAnisotropy)
            score *= feature_multipliers["samplerAnisotropy"];
        if (deviceFeatures.textureCompressionBC)
            score *= feature_multipliers["textureCompressionBC"];
        if (deviceFeatures.fillModeNonSolid)
            score *= feature_multipliers["fillModeNonSolid"];
        if (deviceFeatures.wideLines)
            score *= feature_multipliers["wideLines"];

        // Apply device type multiplier
        switch (deviceProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score *= type_multipliers["discrete"];
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score *= type_multipliers["integrated"];
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            score *= type_multipliers["virtual"];
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            score *= type_multipliers["cpu"];
            break;
        default:
            score *= 0; // Unknown device type
        }
        debug << "Device: " << deviceProperties.deviceName << " Score: " << score << "\n";
        return score;
    }
#pragma endregion

#pragma region VulkanRenderer Functions
    auto VulkanRenderer::create_instance() -> expected<void, string> {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            // ReSharper disable once CppDFAUnreachableCode
            return unexpected("validation layers requested, but not available!");
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

        vector<const char*> requiredExtensions;

        requiredExtensions.reserve(extensionCount);
        for (uint32_t i = 0; i < extensionCount; i++) {
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
            return unexpected("We failed to create the Vulkan instance because the driver is incompatible");
        } else if (result != VK_SUCCESS) {
            return unexpected("We failed to create the Vulkan instance");
        }

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> extensionsList(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsList.data());
        // Below code is only included when debugging the extensions, and is not needed otherwise
        /*std::cout << "available extensions:\n";

        for (const auto& [extensionName, specVersion] : extensionsList) {
            std::cout << '\t' << extensionName << '\n';
        }*/
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

    auto VulkanRenderer::setupDebugMessenger() -> expected<void, string> {
        if (!enableValidationLayers)
            return {};

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(this->m_instance, &createInfo, nullptr, &this->m_debugMessenger) != VK_SUCCESS) {
            return std::unexpected("failed to set up debug messenger!");
        }
        return {};
    }

    //physical devices and queue families lesson

    auto VulkanRenderer::is_device_compatible(const VkPhysicalDevice device) -> bool {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        QueueFamilyIndices indices = this->findQueueFamilies(device);


        return deviceFeatures.geometryShader and indices.isComplete();
    }

    //RateDeviceSuitability moved to additional functions

    auto VulkanRenderer::pickPhysicalDevice() -> expected<void, string> {
        //enumerate physical devices to get count
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(this->m_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            return unexpected("failed to find GPUs with Vulkan support!");
            //we obviously can't draw on a system that doesn't support Vulkan
        }
        //we can now pre-alloc a vector to store the devices
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(this->m_instance, &deviceCount, devices.data()); //once more for the data
        //create an ordered map to store the devices and their scores, this also sorts them
        multimap<double, VkPhysicalDevice> candidates;

        for (const auto& device : devices) {
            double score = rate_device_suitability(device);
            if (this->is_device_compatible(device)) {
                candidates.insert(std::make_pair(score, device));
            } else {
                cout << "Device: " << device << " is not compatible, and won't be considered further\n";
            }
            //rate each, then store them in the map as a score to device pair
        }
        if (candidates.rbegin()->first > 0) {
            this->m_physicalDevice = candidates.rbegin()->second;
            auto deviceProperties = VkPhysicalDeviceProperties{};
            vkGetPhysicalDeviceProperties(this->m_physicalDevice, &deviceProperties);
            cout << "We selected a device successfully: " << deviceProperties.deviceName << "\n";
        } else {
            return unexpected("failed to find a suitable GPU!");
        }
        return {};
    }

    auto VulkanRenderer::findQueueFamilies(VkPhysicalDevice device)-> QueueFamilyIndices {
        QueueFamilyIndices indices;
        // Logic to find queue family indices to populate struct with
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (indices.isComplete()) {
                break;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            i++;
        }

        return indices;
    }



    VulkanRenderer::VulkanRenderer() {

    }

    VulkanRenderer::~VulkanRenderer() {

    }

    auto VulkanRenderer::Initialize() -> expected<void, string> {
        auto result = this->create_instance();
        if (!result.has_value()) {
            return unexpected("Failed to create Vulkan instance: " + result.error());
        }
        if (result = this->setupDebugMessenger(); !result.has_value()) {
            return unexpected("Failed to set up debug messenger: " + result.error());
        }
        if (result = this->pickPhysicalDevice(); !result.has_value()) {
            return unexpected("Failed to pick physical device: " + result.error());
        }
        return {};
    }

    auto VulkanRenderer::getRequiredExtensions() -> vector<const char*> {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
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

    auto VulkanRenderer::RenderFrame() -> expected<void, string> {
        return {};
    }

    auto VulkanRenderer::Resize(int width, int height) -> expected<void, string> {
        return {};
    }

    VKAPI_ATTR auto VKAPI_CALL VulkanRenderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            cerr << "validation layer: " << pCallbackData->pMessage << "\n";
        }

        return VK_FALSE;
    }
#pragma endregion

#pragma region QueueFamilyIndices Functions
    auto QueueFamilyIndices::isComplete() -> bool {
        return graphicsFamily.has_value();
    }
#pragma endregion
}
