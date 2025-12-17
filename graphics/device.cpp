#include "device.hpp"

namespace Graphics {

    Device::Device(VkInstance instance, VkSurfaceKHR surface, const bool enableValidationLayers, const std::vector<const char*>& validationLayers) {

//------------------------------CREATE PHYSICAL DEVICE------------------------------

        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (!deviceCount) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device, surface)) {
                vk_physicalDevice = device;
                break;
            }
        }

        if (vk_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

//------------------------------CREATE LOGICAL DEVICE------------------------------

        QueueFamilyIndices indices = findQueueFamilies(vk_physicalDevice, surface);
        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        for (uint32_t queueFamily : uniqueQueueFamilies) {

            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(queueInfo);
        }
        
        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();

        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceInfo.pEnabledFeatures = &deviceFeatures;
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceInfo.ppEnabledLayerNames = validationLayers.data();
        } else deviceInfo.enabledLayerCount = 0;

//------------------------------CREATE DEVICE------------------------------

        if(vkCreateDevice(vk_physicalDevice, &deviceInfo, nullptr, &vk_logicalDevice) != VK_SUCCESS) throw std::runtime_error("failed to create device!");

        vkGetDeviceQueue(vk_logicalDevice, indices.presentFamily.value(), 0, &vk_presentQueue);
    }

//------------------------------CREATE QUEUE FAMILIES------------------------------

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;
        VkBool32 presentSupport;

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {

            vkGetPhysicalDeviceSurfaceSupportKHR(vk_physicalDevice, i, surface, &presentSupport);

            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)  indices.graphicsFamily = i;
            if (presentSupport) indices.presentFamily = i;
            if(indices.isComplete()) break;
            
            i++;
        }

        return indices;
    }
//------------------------------CREATE SWAP CHAIN------------------------------

    void Device::createSwapChain(GLFWwindow* window, VkSurfaceKHR surface) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vk_physicalDevice, surface);
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        QueueFamilyIndices indices = findQueueFamilies(vk_physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.format);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.present);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR swapChainInfo{};
        swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainInfo.surface = surface;
        swapChainInfo.minImageCount  = imageCount;
        swapChainInfo.imageFormat = surfaceFormat.format;
        swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainInfo.imageExtent = extent;
        swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainInfo.imageArrayLayers = 1;

        if (indices.graphicsFamily != indices.presentFamily) {
            swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainInfo.queueFamilyIndexCount = 2;
            swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainInfo.queueFamilyIndexCount = 0;
            swapChainInfo.pQueueFamilyIndices = nullptr;
        }

        swapChainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainInfo.clipped = VK_TRUE;
        swapChainInfo.presentMode = presentMode;
        swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(vk_logicalDevice, &swapChainInfo, nullptr, &vk_swapChain) != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");

        vkGetSwapchainImagesKHR(vk_logicalDevice, vk_swapChain, &imageCount, nullptr);
        vk_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vk_logicalDevice, vk_swapChain, &imageCount, vk_swapChainImages.data());

        vk_swapChainImageFormat = surfaceFormat.format;
        vk_swapChainExtent = extent;
    }

//------------------------------CREATE IMAGE VIEWS------------------------------

    void Device::createImageViews() {
        vk_swapChainImageViews.resize(vk_swapChainImages.size());

        for (size_t i = 0; i < vk_swapChainImages.size(); i++) {

            VkImageViewCreateInfo swapChainImagesViewsInfo{};
            swapChainImagesViewsInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            swapChainImagesViewsInfo.image = vk_swapChainImages[i];
            swapChainImagesViewsInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            swapChainImagesViewsInfo.format = vk_swapChainImageFormat;
            swapChainImagesViewsInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            swapChainImagesViewsInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            swapChainImagesViewsInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            swapChainImagesViewsInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            swapChainImagesViewsInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            swapChainImagesViewsInfo.subresourceRange.baseMipLevel = 0;
            swapChainImagesViewsInfo.subresourceRange.levelCount = 1;
            swapChainImagesViewsInfo.subresourceRange.baseArrayLayer = 0;
            swapChainImagesViewsInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vk_logicalDevice, &swapChainImagesViewsInfo, nullptr, &vk_swapChainImageViews[i]) != VK_SUCCESS) throw std::runtime_error("failed to create image views!");
        }
    }

//------------------------------QUERY SWAP CHAIN SUPPORT------------------------------

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount) {
            details.format.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.format.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.present.data());
        
        if (presentModeCount) {
            details.present.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.present.data());
        }

        return details;
    }

//------------------------------CHECK DEVICE EXTENSIONS SUPPORT------------------------------

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableProperties(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableProperties.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for(const auto& extension : availableProperties) requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

//------------------------------CHECK IS DEVICE SUITABLE------------------------------

    bool Device::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.format.empty() & !swapChainSupport.present.empty();
        }

        return findQueueFamilies(device, surface).isComplete() && extensionsSupported && swapChainAdequate;
    }

//------------------------------CHOOSE SWAP SURFACE FORMAT------------------------------

    VkSurfaceFormatKHR Device::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {

            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) return availableFormat;
        }

        return availableFormats[0];
    }

//------------------------------CHOOSE SWAP CHAIN MODE------------------------------

    VkPresentModeKHR Device::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

//------------------------------CHOOSE SWAP CHAIN------------------------------

    VkExtent2D Device::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {return capabilities.currentExtent;}
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }   
    }

    Device::~Device() {
        for (auto imageView : vk_swapChainImageViews) {
            vkDestroyImageView(vk_logicalDevice, imageView, nullptr);
        }
        if (vk_swapChain != VK_NULL_HANDLE) vkDestroySwapchainKHR(vk_logicalDevice, vk_swapChain, nullptr); 
        if (vk_logicalDevice != VK_NULL_HANDLE) vkDestroyDevice(vk_logicalDevice, nullptr);
    }
}