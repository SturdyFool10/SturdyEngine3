//
// Created by sturd on 2/13/2025.
//

#ifndef VULKAN_H
#define VULKAN_H

#include "../Renderer.h"
#include "Core/Window/Window.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <complex>
#include <expected>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>


using std::cerr;
using std::cout;
using std::expected;
using std::map;
using std::multimap;
using std::optional;
using std::set;
using std::string;
using std::unexpected;
using std::vector;

namespace SFT::Renderer::VK {

struct QueueFamilyIndices;
struct SwapChainSupportDetails;
class VulkanRenderer : public Renderer {
private:
#pragma region Private Member Variables
    bool m_isInitialized = false;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice;
    VkQueue m_graphicsQueue;
    VkSurfaceKHR m_surface;
    Window::Window *m_window;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapChain;
    vector<VkImage> m_swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    vector<VkImageView> m_swapChainImageViews;
    vector<VkFramebuffer> m_swapChainFramebuffers;
#pragma endregion

#pragma region Internal Functions
  auto create_instance() -> expected<void, string>;
  static auto checkValidationLayerSupport() -> bool;
  auto setupDebugMessenger() -> expected<void, string>;
  auto is_device_compatible(VkPhysicalDevice device) -> bool;
  auto pickPhysicalDevice() -> expected<void, string>;
  auto findQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices;
  auto createLogicalDevice() -> expected<void, string>;
  auto createSurface() -> expected<void, string>;
  auto checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool;
  auto querySwapChainSupport(VkPhysicalDevice device)
      -> SwapChainSupportDetails;
  auto chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats)
      -> VkSurfaceFormatKHR;
  auto chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes)
      -> VkPresentModeKHR;
  auto chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
      -> VkExtent2D;
  auto createSwapChain() -> expected<void, string>;
  auto createSwapChainImageViews() -> expected<void, string>;
    auto createShaderModule(const std::vector<char>& code) -> expected<VkShaderModule, string>;
    auto createGraphicsPipeline() -> expected<void, string>;
  auto createFramebuffers() -> void;
  auto getRequiredExtensions() -> vector<const char *>;
#pragma endregion

public:
  VulkanRenderer();
  ~VulkanRenderer() override;
  auto Initialize() -> expected<void, string> override;
  auto Shutdown() -> void override;
  auto RenderFrame() -> expected<void, string> override;
  auto Resize(int width, int height) -> expected<void, string> override;
  auto SetWindow(Window::Window *window) -> void override;
  auto getAPIName() -> string override;
  static auto
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) -> VkBool32;
};
struct QueueFamilyIndices {
  optional<uint32_t> graphicsFamily;
  optional<uint32_t> presentFamily;

  auto isComplete() -> bool;
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

} // namespace SFT::Renderer::VK

#endif // VULKAN_H
