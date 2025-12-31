#pragma once

#include <stdexcept>
#include <cstring>
#include <vector>
#include "../includes/graphics.hpp"

namespace Graphics {

    class Instance {

        public:
            Instance();
            void createSurface(GLFWwindow* window);
            ~Instance();

            inline VkInstance getInstance() const { return vk_instance; }
            inline VkSurfaceKHR getSurface() const { return vk_surface; }
            inline bool getEnableValidationLayers() const { return enableValidationLayers; }
            inline const std::vector<const char*> getValidationLayers() const { return validationLayers; }

        private:
            #ifdef NDEBUG
                const bool enableValidationLayers = false;
            #else
                const bool enableValidationLayers = true;
            #endif

            VkInstance vk_instance;
            VkSurfaceKHR vk_surface;
            const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

            bool checkValidationLayerSupport();
    };
}