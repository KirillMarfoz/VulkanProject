#include "device.hpp"

namespace Graphics::Device {

    Device::Device::Device(VkInstance instance, const bool enableValidationLayers, const std::vector<const char*> validationLayers) {

//------------------------------CREATING PHYSICAL DEVICE------------------------------

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (!deviceCount) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                vk_physicalDevice = device;
                break;
            }
        }

        if (vk_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

//------------------------------CREATING LOGICAL DEVICE------------------------------

        QueueFamilyIndices indices = findQueueFamilies(vk_physicalDevice);
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;
        
        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pQueueCreateInfos = &queueInfo;
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pEnabledFeatures = &deviceFeatures;
        deviceInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceInfo.ppEnabledLayerNames = validationLayers.data();
        } else deviceInfo.enabledLayerCount = 0;

//------------------------------CREATING DEVICE------------------------------

        if(vkCreateDevice(vk_physicalDevice, &deviceInfo, nullptr, &vk_logicalDevice) != VK_SUCCESS) throw std::runtime_error("failed to create device!");

        vkGetDeviceQueue(vk_logicalDevice, indices.graphicsFamily.value(), 0, &vk_graphicsQueue);
    }

//------------------------------CREATING QUEUE FAMILIES------------------------------

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {

            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)  indices.graphicsFamily = i;
            if(indices.is_complete()) break;
            
            i++;
        }

        return indices;
    }

    Device::~Device() {
        vkDestroyDevice(vk_logicalDevice, nullptr);
    }
}