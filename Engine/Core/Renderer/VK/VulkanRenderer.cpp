//
// Created by sturd on 2/13/2025.
//

#include "VulkanRenderer.h"
#include "spdlog/spdlog.h"
#include "Core/Window/GLFW/GLFWWindowWrapped.h"

const std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

namespace SFT::Renderer::VK {

#pragma region additional functions
    auto DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) -> void {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    auto CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) -> VkResult {
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
        spdlog::debug("Device: \"{}\", Score: {}", deviceProperties.deviceName, score);
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
        bool extensions_supported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensions_supported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = not swapChainSupport.formats.empty() and not swapChainSupport.presentModes.empty();
        }
        return deviceFeatures.geometryShader and indices.isComplete() and extensions_supported and swapChainAdequate;
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
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                std::string deviceName(deviceProperties.deviceName);
                spdlog::warn("Device: {} is not compatible, and won't be considered further", deviceName);
            }
            //rate each, then store them in the map as a score to device pair
        }
        if (candidates.rbegin()->first > 0) {
            this->m_physicalDevice = candidates.rbegin()->second;
            auto deviceProperties = VkPhysicalDeviceProperties{};
            vkGetPhysicalDeviceProperties(this->m_physicalDevice, &deviceProperties);
            spdlog::info("We selected a device successfully: {}", deviceProperties.deviceName);
        } else {
            return unexpected("failed to find a suitable GPU!");
        }
        return {};
    }

    auto VulkanRenderer::findQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices {
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

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->m_surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            i++;
        }

        return indices;
    }

    //Logical Device and Queues lesson

    auto VulkanRenderer::createLogicalDevice() -> expected<void, string> {
        QueueFamilyIndices indices = findQueueFamilies(this->m_physicalDevice);

        vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateDevice(this->m_physicalDevice, &createInfo, nullptr, &this->m_logicalDevice) != VK_SUCCESS) {
            return unexpected("failed to create logical device!");
        }
        vkGetDeviceQueue(this->m_logicalDevice, indices.graphicsFamily.value(), 0, &this->m_graphicsQueue);
        vkGetDeviceQueue(this->m_logicalDevice, indices.presentFamily.value(), 0, &this->m_presentQueue);
        return {};
    }

    //Surface lesson

    auto VulkanRenderer::createSurface() -> expected<void, string> {
        auto APIName = this->m_window->getAPIName();
        if (APIName == "GLFW") {
            const auto window = dynamic_cast<Window::GLFW::GLFWWindowWrapped*>(this->m_window);
            if (glfwCreateWindowSurface(this->m_instance, window->get_handle(), nullptr, &this->m_surface) != VK_SUCCESS) {
                return unexpected("failed to create window surface!");
            }
            return {};
        } else {
            return unexpected("Window API not supported");
        }
    }

    //Swapchain lesson

    auto VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& [extensionName, specVersion] : availableExtensions) {
            spdlog::debug("Found Device Extension Available: Name: \"{}\", Spec Version: {}", extensionName, specVersion);
            requiredExtensions.erase(extensionName);
        }

        return requiredExtensions.empty();
    }

    auto VulkanRenderer::querySwapChainSupport(VkPhysicalDevice device) -> SwapChainSupportDetails {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->m_surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->m_surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->m_surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->m_surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    auto VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB and availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    auto VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    auto VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)-> VkExtent2D {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            auto windowAPI = this->m_window->getAPIName();
            if (windowAPI == "GLFW") {
                const auto window = dynamic_cast<Window::GLFW::GLFWWindowWrapped*>(this->m_window);
                glfwGetFramebufferSize(window->get_handle(), &width, &height);
            }

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    auto VulkanRenderer::createSwapChain() -> expected<void, string> {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(this->m_physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->m_surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(this->m_physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->m_logicalDevice, &createInfo, nullptr, &this->m_swapChain) != VK_SUCCESS) {
            return unexpected("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(this->m_logicalDevice, this->m_swapChain, &imageCount, nullptr);
        this->m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(this->m_logicalDevice, this->m_swapChain, &imageCount, this->m_swapChainImages.data());
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
        return {};
    }

    //ImageViews lesson

    auto VulkanRenderer::createSwapchainImageViews() -> expected<void, string> {
        this->m_swapChainImageViews.resize(this->m_swapChainImages.size());
        for (size_t i = 0; i < this->m_swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = this->m_swapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(this->m_logicalDevice, &createInfo, nullptr, &this->m_swapChainImageViews[i]) != VK_SUCCESS) {
                return unexpected("failed to create image views!");
            }
        }
        return {};
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
        if (result = this->createSurface(); !result.has_value()) {
            return unexpected("Failed to create surface: " + result.error());
        }
        if (result = this->pickPhysicalDevice(); !result.has_value()) {
            return unexpected("Failed to pick physical device: " + result.error());
        }
        if (result = this->createLogicalDevice(); !result.has_value()) {
            return unexpected("Failed to create logical device: " + result.error());
        }
        if (result = this->createSwapChain(); !result.has_value()) {
            return unexpected("Failed to create swap chain: " + result.error());
        }
        if (result = this->createSwapchainImageViews(); !result.has_value()) {
            return unexpected("Failed to create swap chain image views: " + result.error());
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
        for (auto imageView : this->m_swapChainImageViews) {
            vkDestroyImageView(this->m_logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(this->m_logicalDevice, this->m_swapChain, nullptr);
        vkDestroyDevice(this->m_logicalDevice, nullptr);
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(this->m_instance, this->m_debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(this->m_instance, this->m_surface, nullptr);
        vkDestroyInstance(this->m_instance, nullptr);
    }

    auto VulkanRenderer::RenderFrame() -> expected<void, string> {
        return {};
    }

    auto VulkanRenderer::Resize(int width, int height) -> expected<void, string> {
        return {};
    }

    auto VulkanRenderer::SetWindow(Window::Window* window) -> void {
        this->m_window = window;
    }

    VKAPI_ATTR auto VKAPI_CALL VulkanRenderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            spdlog::warn("validation layer: {}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }
#pragma endregion

#pragma region QueueFamilyIndices Functions
    auto QueueFamilyIndices::isComplete() -> bool {
        return graphicsFamily.has_value() and presentFamily.has_value();
    }
#pragma endregion
}
