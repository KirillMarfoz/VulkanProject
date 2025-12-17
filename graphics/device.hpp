#pragma once

#include "../includes/graphics.hpp"
#include <stdexcept>
#include <optional>
#include <set>
#include <algorithm>
#include <vector>

namespace Graphics {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkPresentModeKHR> present;
        std::vector<VkSurfaceFormatKHR> format;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        inline bool isComplete() {
            return graphicsFamily.has_value() & presentFamily.has_value();
        }
    };
    

    class Device {
    public:
        Device(
        VkInstance instance,
        VkSurfaceKHR surface,
        bool enableValidationLayers,
        const std::vector<const char*>& validationLayers
        );
        void createSwapChain(GLFWwindow* window, VkSurfaceKHR surface);
        void createImageViews();
        ~Device();

    private:
        VkPhysicalDevice vk_physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceFeatures deviceFeatures{};
        std::vector<VkImage> vk_swapChainImages;
        VkDevice vk_logicalDevice = VK_NULL_HANDLE;
        VkQueue vk_graphicsQueue;
        VkQueue vk_presentQueue;
        VkSwapchainKHR vk_swapChain;
        VkFormat vk_swapChainImageFormat;
        std::vector<VkImageView> vk_swapChainImageViews;
        VkExtent2D vk_swapChainExtent;
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    };

}