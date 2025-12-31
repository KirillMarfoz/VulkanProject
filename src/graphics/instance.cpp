#include "instance.hpp"

namespace Graphics {

    Instance::Instance() {
//------------------------------VALIDATION LAYERS CHECK------------------------------
        if (enableValidationLayers && !checkValidationLayerSupport()) throw std::runtime_error("validation layers requested, but not available!");

//------------------------------ENGINE INFO------------------------------
        VkApplicationInfo engineInfo{};
        engineInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        engineInfo.pApplicationName = "Vulkan project";
        engineInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        engineInfo.pEngineName = "UranEngine";
        engineInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        engineInfo.apiVersion = VK_API_VERSION_1_0;

//------------------------------INSTANCE INFO------------------------------

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &engineInfo;
        instanceInfo.enabledExtensionCount = glfwExtensionCount;
        instanceInfo.ppEnabledExtensionNames = glfwExtensions;
        if (enableValidationLayers) {
            instanceInfo.enabledLayerCount = static_cast<uint32_t>(vk_validationLayers.size());
            instanceInfo.ppEnabledLayerNames = vk_validationLayers.data();
        } else instanceInfo.enabledLayerCount = 0;

//------------------------------CREATE INSTANCE------------------------------
        if (vkCreateInstance(&instanceInfo, nullptr, &vk_instance) != VK_SUCCESS) throw std::runtime_error("failed to create instance!");
    }

//------------------------------CREATE SURFACE------------------------------

    void Instance::createSurface(GLFWwindow* window) {
        if (glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface) != VK_SUCCESS) throw std::runtime_error("failde to create surface!");
    }

//------------------------------CHECK VALIDATION LAYERS SUPPORT------------------------------

    bool Instance::checkValidationLayerSupport() {
        uint32_t  layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : vk_validationLayers) {
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

//------------------------------DESTROY INSTANCE------------------------------

    Instance::~Instance() {
        if (vk_surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
        if (vk_instance != VK_NULL_HANDLE) vkDestroyInstance(vk_instance, nullptr);
    }
}