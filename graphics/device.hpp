#pragma once

#include "../includes/graphics.hpp"
#include <stdexcept>
#include <optional>
#include <vector>

namespace Graphics::Device {

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool is_complete() {
            return graphicsFamily.has_value();
        }
    };
    

    class Device {
    public:
        Device(VkInstance instance, const bool enableValidationLayers, const std::vector<const char*> validationLayers);
        ~Device();

    private:
        VkPhysicalDevice vk_physicalDevice = VK_NULL_HANDLE;
        VkDevice vk_logicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkQueue vk_graphicsQueue;

        void pickPhysicalDevice(VkInstance instance);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        inline bool isDeviceSuitable(VkPhysicalDevice device) { return findQueueFamilies(device).is_complete(); }
    };

}